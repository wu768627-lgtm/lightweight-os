#include "tcp.h"
#include "network.h"
#include "../kernel/kernel.h"
#include "../libs/string.h"

// TCP连接表
static struct tcp_connection tcp_connections[MAX_TCP_CONNECTIONS];

// TCP端口表
static struct tcp_port tcp_ports[MAX_TCP_PORTS];

// 初始化TCP协议栈
void tcp_init() {
    // 初始化连接表
    for (int i = 0; i < MAX_TCP_CONNECTIONS; i++) {
        tcp_connections[i].state = TCP_STATE_CLOSED;
    }
    
    // 初始化端口表
    for (int i = 0; i < MAX_TCP_PORTS; i++) {
        tcp_ports[i].used = 0;
    }
    
    print_string("TCP protocol stack initialized\n");
}

// 创建TCP连接
struct tcp_connection* tcp_connect(struct ip_address dest_ip, unsigned short dest_port) {
    print_string("TCP: Establishing connection to ");
    char ip_str[16];
    ip_address_to_string(dest_ip, ip_str);
    print_string(ip_str);
    print_string(":");
    char port_str[8];
    int_to_string(dest_port, port_str);
    print_string(port_str);
    print_string("\n");
    
    // 查找一个空闲的连接槽
    struct tcp_connection* conn = 0;
    for (int i = 0; i < MAX_TCP_CONNECTIONS; i++) {
        if (tcp_connections[i].state == TCP_STATE_CLOSED) {
            conn = &tcp_connections[i];
            break;
        }
    }
    
    if (!conn) {
        print_string("TCP: No available connection slots\n");
        return 0;
    }
    
    // 初始化连接
    conn->local_ip = get_local_ip(); // 获取本地IP地址
    conn->local_port = tcp_allocate_port(); // 分配本地端口
    conn->remote_ip = dest_ip;
    conn->remote_port = dest_port;
    conn->state = TCP_STATE_SYN_SENT;
    conn->sequence_number = generate_initial_sequence_number();
    conn->acknowledgment_number = 0;
    
    // 发送SYN包
    if (tcp_send_syn(conn) < 0) {
        conn->state = TCP_STATE_CLOSED;
        print_string("TCP: Failed to send SYN packet\n");
        return 0;
    }
    
    // 等待连接建立 (简化实现，实际应等待ACK)
    conn->state = TCP_STATE_ESTABLISHED;
    
    print_string("TCP: Connection established\n");
    return conn;
}

// 关闭TCP连接
void tcp_close(struct tcp_connection* conn) {
    if (!conn) {
        return;
    }
    
    print_string("TCP: Closing connection\n");
    
    // 如果连接已建立，执行四次挥手
    if (conn->state == TCP_STATE_ESTABLISHED) {
        conn->state = TCP_STATE_FIN_WAIT_1;
        // 发送FIN包
        tcp_send_fin(conn);
        // 等待ACK和对端FIN (简化实现)
        conn->state = TCP_STATE_CLOSED;
    } else {
        conn->state = TCP_STATE_CLOSED;
    }
}

// 发送TCP数据
int tcp_send(struct tcp_connection* conn, unsigned char* data, unsigned int length) {
    if (!conn || !data || length == 0) {
        return -1;
    }
    
    if (conn->state != TCP_STATE_ESTABLISHED) {
        print_string("TCP: Connection not established\n");
        return -1;
    }
    
    print_string("TCP: Sending ");
    char len_str[12];
    int_to_string(length, len_str);
    print_string(len_str);
    print_string(" bytes\n");
    
    // 构造TCP包头
    struct tcp_header header;
    header.source_port = conn->local_port;
    header.dest_port = conn->remote_port;
    header.sequence_number = conn->sequence_number;
    header.acknowledgment_number = conn->acknowledgment_number;
    header.header_length = sizeof(struct tcp_header) / 4;
    header.flags = TCP_FLAG_ACK;
    header.window_size = TCP_DEFAULT_WINDOW_SIZE;
    header.checksum = 0; // 计算校验和
    header.urgent_pointer = 0;
    
    // 发送TCP段
    int result = tcp_send_segment(&header, data, length);
    if (result >= 0) {
        conn->sequence_number += length;
    }
    
    return result;
}

// 接收TCP数据
int tcp_receive(struct tcp_connection* conn, unsigned char* buffer, unsigned int buffer_size) {
    if (!conn || !buffer || buffer_size == 0) {
        return -1;
    }
    
    if (conn->state != TCP_STATE_ESTABLISHED) {
        print_string("TCP: Connection not established\n");
        return -1;
    }
    
    // 在实际实现中，这里会从接收缓冲区读取数据
    // 简化实现，返回0表示没有数据
    return 0;
}

// 处理接收到的TCP段
int tcp_handle_segment(struct tcp_header* header, unsigned char* data, unsigned int length) {
    if (!header) {
        return -1;
    }
    
    print_string("TCP: Received segment\n");
    
    // 查找对应的连接
    struct tcp_connection* conn = tcp_find_connection(header->dest_port, header->source_port);
    if (!conn) {
        print_string("TCP: No matching connection found\n");
        // 发送RST包
        tcp_send_rst(header);
        return -1;
    }
    
    // 根据连接状态处理段
    switch (conn->state) {
        case TCP_STATE_LISTEN:
            if (header->flags & TCP_FLAG_SYN) {
                return tcp_handle_syn(conn, header, data, length);
            }
            break;
            
        case TCP_STATE_SYN_SENT:
            if (header->flags & TCP_FLAG_SYN && header->flags & TCP_FLAG_ACK) {
                return tcp_handle_syn_ack(conn, header, data, length);
            } else if (header->flags & TCP_FLAG_SYN) {
                return tcp_handle_syn(conn, header, data, length);
            }
            break;
            
        case TCP_STATE_ESTABLISHED:
            if (header->flags & TCP_FLAG_ACK) {
                return tcp_handle_data(conn, header, data, length);
            } else if (header->flags & TCP_FLAG_FIN) {
                return tcp_handle_fin(conn, header, data, length);
            }
            break;
            
        case TCP_STATE_FIN_WAIT_1:
            if (header->flags & TCP_FLAG_ACK) {
                conn->state = TCP_STATE_FIN_WAIT_2;
            }
            if (header->flags & TCP_FLAG_FIN) {
                return tcp_handle_fin(conn, header, data, length);
            }
            break;
            
        case TCP_STATE_FIN_WAIT_2:
            if (header->flags & TCP_FLAG_FIN) {
                return tcp_handle_fin(conn, header, data, length);
            }
            break;
            
        default:
            break;
    }
    
    return 0;
}

// 分配TCP端口
unsigned short tcp_allocate_port() {
    static unsigned short next_port = 1024;
    
    for (int i = 0; i < MAX_TCP_PORTS; i++) {
        int port_index = (next_port + i) % MAX_TCP_PORTS;
        if (!tcp_ports[port_index].used) {
            tcp_ports[port_index].used = 1;
            next_port = (port_index + 1) % MAX_TCP_PORTS;
            return 1024 + port_index;
        }
    }
    
    // 没有可用端口
    return 0;
}

// 释放TCP端口
void tcp_free_port(unsigned short port) {
    if (port >= 1024 && port < 1024 + MAX_TCP_PORTS) {
        int port_index = port - 1024;
        tcp_ports[port_index].used = 0;
    }
}

// 查找TCP连接
struct tcp_connection* tcp_find_connection(unsigned short local_port, unsigned short remote_port) {
    for (int i = 0; i < MAX_TCP_CONNECTIONS; i++) {
        if (tcp_connections[i].state != TCP_STATE_CLOSED &&
            tcp_connections[i].local_port == local_port &&
            tcp_connections[i].remote_port == remote_port) {
            return &tcp_connections[i];
        }
    }
    return 0;
}

// 发送SYN包
int tcp_send_syn(struct tcp_connection* conn) {
    if (!conn) {
        return -1;
    }
    
    struct tcp_header header;
    header.source_port = conn->local_port;
    header.dest_port = conn->remote_port;
    header.sequence_number = conn->sequence_number;
    header.acknowledgment_number = 0;
    header.header_length = sizeof(struct tcp_header) / 4;
    header.flags = TCP_FLAG_SYN;
    header.window_size = TCP_DEFAULT_WINDOW_SIZE;
    header.checksum = 0;
    header.urgent_pointer = 0;
    
    return tcp_send_segment(&header, 0, 0);
}

// 发送FIN包
int tcp_send_fin(struct tcp_connection* conn) {
    if (!conn) {
        return -1;
    }
    
    struct tcp_header header;
    header.source_port = conn->local_port;
    header.dest_port = conn->remote_port;
    header.sequence_number = conn->sequence_number;
    header.acknowledgment_number = conn->acknowledgment_number;
    header.header_length = sizeof(struct tcp_header) / 4;
    header.flags = TCP_FLAG_FIN | TCP_FLAG_ACK;
    header.window_size = TCP_DEFAULT_WINDOW_SIZE;
    header.checksum = 0;
    header.urgent_pointer = 0;
    
    return tcp_send_segment(&header, 0, 0);
}

// 发送RST包
int tcp_send_rst(struct tcp_header* received_header) {
    if (!received_header) {
        return -1;
    }
    
    struct tcp_header header;
    header.source_port = received_header->dest_port;
    header.dest_port = received_header->source_port;
    header.sequence_number = received_header->acknowledgment_number;
    header.acknowledgment_number = received_header->sequence_number + 1;
    header.header_length = sizeof(struct tcp_header) / 4;
    header.flags = TCP_FLAG_RST | TCP_FLAG_ACK;
    header.window_size = TCP_DEFAULT_WINDOW_SIZE;
    header.checksum = 0;
    header.urgent_pointer = 0;
    
    return tcp_send_segment(&header, 0, 0);
}

// 发送TCP段
int tcp_send_segment(struct tcp_header* header, unsigned char* data, unsigned int length) {
    if (!header) {
        return -1;
    }
    
    // 在实际实现中，这里会构造完整的TCP段并发送
    print_string("TCP: Sending segment\n");
    return length;
}

// 处理SYN包
int tcp_handle_syn(struct tcp_connection* conn, struct tcp_header* header, unsigned char* data, unsigned int length) {
    if (!conn || !header) {
        return -1;
    }
    
    conn->remote_ip = get_remote_ip(); // 获取远程IP
    conn->remote_port = header->source_port;
    conn->sequence_number = generate_initial_sequence_number();
    conn->acknowledgment_number = header->sequence_number + 1;
    
    // 发送SYN-ACK
    struct tcp_header response;
    response.source_port = conn->local_port;
    response.dest_port = conn->remote_port;
    response.sequence_number = conn->sequence_number;
    response.acknowledgment_number = conn->acknowledgment_number;
    response.header_length = sizeof(struct tcp_header) / 4;
    response.flags = TCP_FLAG_SYN | TCP_FLAG_ACK;
    response.window_size = TCP_DEFAULT_WINDOW_SIZE;
    response.checksum = 0;
    response.urgent_pointer = 0;
    
    int result = tcp_send_segment(&response, 0, 0);
    if (result >= 0) {
        conn->state = TCP_STATE_SYN_RECEIVED;
    }
    
    return result;
}

// 处理SYN-ACK包
int tcp_handle_syn_ack(struct tcp_connection* conn, struct tcp_header* header, unsigned char* data, unsigned int length) {
    if (!conn || !header) {
        return -1;
    }
    
    conn->acknowledgment_number = header->sequence_number + 1;
    conn->sequence_number = header->acknowledgment_number;
    conn->state = TCP_STATE_ESTABLISHED;
    
    // 发送ACK
    struct tcp_header response;
    response.source_port = conn->local_port;
    response.dest_port = conn->remote_port;
    response.sequence_number = conn->sequence_number;
    response.acknowledgment_number = conn->acknowledgment_number;
    response.header_length = sizeof(struct tcp_header) / 4;
    response.flags = TCP_FLAG_ACK;
    response.window_size = TCP_DEFAULT_WINDOW_SIZE;
    response.checksum = 0;
    response.urgent_pointer = 0;
    
    return tcp_send_segment(&response, 0, 0);
}

// 处理数据包
int tcp_handle_data(struct tcp_connection* conn, struct tcp_header* header, unsigned char* data, unsigned int length) {
    if (!conn || !header) {
        return -1;
    }
    
    // 更新确认号
    conn->acknowledgment_number = header->sequence_number + length;
    
    // 如果有数据，将其放入接收缓冲区
    if (data && length > 0) {
        // 在实际实现中，这里会将数据放入接收缓冲区
        print_string("TCP: Received ");
        char len_str[12];
        int_to_string(length, len_str);
        print_string(len_str);
        print_string(" bytes of data\n");
    }
    
    // 发送ACK
    struct tcp_header response;
    response.source_port = conn->local_port;
    response.dest_port = conn->remote_port;
    response.sequence_number = conn->sequence_number;
    response.acknowledgment_number = conn->acknowledgment_number;
    response.header_length = sizeof(struct tcp_header) / 4;
    response.flags = TCP_FLAG_ACK;
    response.window_size = TCP_DEFAULT_WINDOW_SIZE;
    response.checksum = 0;
    response.urgent_pointer = 0;
    
    return tcp_send_segment(&response, 0, 0);
}

// 处理FIN包
int tcp_handle_fin(struct tcp_connection* conn, struct tcp_header* header, unsigned char* data, unsigned int length) {
    if (!conn || !header) {
        return -1;
    }
    
    // 更新确认号
    conn->acknowledgment_number = header->sequence_number + 1;
    
    // 发送ACK
    struct tcp_header response;
    response.source_port = conn->local_port;
    response.dest_port = conn->remote_port;
    response.sequence_number = conn->sequence_number;
    response.acknowledgment_number = conn->acknowledgment_number;
    response.header_length = sizeof(struct tcp_header) / 4;
    response.flags = TCP_FLAG_ACK;
    response.window_size = TCP_DEFAULT_WINDOW_SIZE;
    response.checksum = 0;
    response.urgent_pointer = 0;
    
    tcp_send_segment(&response, 0, 0);
    
    // 根据当前状态处理
    switch (conn->state) {
        case TCP_STATE_ESTABLISHED:
            conn->state = TCP_STATE_CLOSE_WAIT;
            // 通知应用程序连接正在关闭
            break;
            
        case TCP_STATE_FIN_WAIT_1:
        case TCP_STATE_FIN_WAIT_2:
            conn->state = TCP_STATE_TIME_WAIT;
            // 等待一段时间后关闭连接
            conn->state = TCP_STATE_CLOSED;
            break;
            
        default:
            conn->state = TCP_STATE_CLOSED;
            break;
    }
    
    return 0;
}

// 生成初始序列号
unsigned int generate_initial_sequence_number() {
    // 在实际实现中，这里会生成一个随机的初始序列号
    static unsigned int isn = 0;
    return isn++;
}

// 获取本地IP地址
struct ip_address get_local_ip() {
    // 在实际实现中，这里会返回本地IP地址
    struct ip_address ip = {{192, 168, 1, 100}};
    return ip;
}

// 获取远程IP地址
struct ip_address get_remote_ip() {
    // 在实际实现中，这里会返回远程IP地址
    struct ip_address ip = {{192, 168, 1, 1}};
    return ip;
}

// IP地址转字符串
void ip_address_to_string(struct ip_address ip, char* buffer) {
    char part_str[4];
    int_to_string(ip.octet[0], part_str);
    strcpy(buffer, part_str);
    strcat(buffer, ".");
    int_to_string(ip.octet[1], part_str);
    strcat(buffer, part_str);
    strcat(buffer, ".");
    int_to_string(ip.octet[2], part_str);
    strcat(buffer, part_str);
    strcat(buffer, ".");
    int_to_string(ip.octet[3], part_str);
    strcat(buffer, part_str);
}

// 字符串比较
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// 字符串复制
char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
}

// 字符串连接
char* strcat(char* dest, const char* src) {
    char* ret = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
    return ret;
}

// 整数转字符串
void int_to_string(int value, char* str) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    int i = 0;
    int is_negative = 0;

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    char temp[12];
    while (value > 0) {
        temp[i] = (value % 10) + '0';
        value /= 10;
        i++;
    }

    int j = 0;
    if (is_negative) {
        str[j++] = '-';
    }

    for (int k = i - 1; k >= 0; k--) {
        str[j++] = temp[k];
    }
    str[j] = '\0';
}