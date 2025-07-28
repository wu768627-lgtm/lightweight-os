#ifndef TCP_H
#define TCP_H

#include "network.h"

// TCP最大连接数和端口数
#define MAX_TCP_CONNECTIONS 64
#define MAX_TCP_PORTS 1024

// TCP状态
#define TCP_STATE_CLOSED            0
#define TCP_STATE_LISTEN            1
#define TCP_STATE_SYN_SENT          2
#define TCP_STATE_SYN_RECEIVED      3
#define TCP_STATE_ESTABLISHED       4
#define TCP_STATE_FIN_WAIT_1        5
#define TCP_STATE_FIN_WAIT_2        6
#define TCP_STATE_CLOSE_WAIT        7
#define TCP_STATE_CLOSING           8
#define TCP_STATE_LAST_ACK          9
#define TCP_STATE_TIME_WAIT         10

// TCP标志位
#define TCP_FLAG_FIN                0x01
#define TCP_FLAG_SYN                0x02
#define TCP_FLAG_RST                0x04
#define TCP_FLAG_PSH                0x08
#define TCP_FLAG_ACK                0x10
#define TCP_FLAG_URG                0x20
#define TCP_FLAG_ECE                0x40
#define TCP_FLAG_CWR                0x80

// TCP默认窗口大小
#define TCP_DEFAULT_WINDOW_SIZE     8192

// TCP端口结构
struct tcp_port {
    unsigned short port;
    unsigned char used;
};

// TCP头部结构
struct tcp_header {
    unsigned short source_port;             // 源端口
    unsigned short dest_port;               // 目标端口
    unsigned int sequence_number;           // 序列号
    unsigned int acknowledgment_number;     // 确认号
    unsigned char header_length;            // 头部长度(4位)
    unsigned char flags;                    // 标志位
    unsigned short window_size;             // 窗口大小
    unsigned short checksum;                // 校验和
    unsigned short urgent_pointer;          // 紧急指针
    unsigned char options[];                // 选项(可变长度)
} __attribute__((packed));

// TCP连接结构
struct tcp_connection {
    struct ip_address local_ip;             // 本地IP地址
    unsigned short local_port;              // 本地端口
    struct ip_address remote_ip;            // 远程IP地址
    unsigned short remote_port;             // 远程端口
    unsigned int state;                     // 连接状态
    unsigned int sequence_number;           // 序列号
    unsigned int acknowledgment_number;     // 确认号
    unsigned char send_buffer[8192];        // 发送缓冲区
    unsigned int send_buffer_head;          // 发送缓冲区头指针
    unsigned int send_buffer_tail;          // 发送缓冲区尾指针
    unsigned char receive_buffer[8192];     // 接收缓冲区
    unsigned int receive_buffer_head;       // 接收缓冲区头指针
    unsigned int receive_buffer_tail;       // 接收缓冲区尾指针
    unsigned int window_size;               // 窗口大小
    unsigned int timeout;                   // 超时时间
};

// 函数声明
void tcp_init();
struct tcp_connection* tcp_connect(struct ip_address dest_ip, unsigned short dest_port);
void tcp_close(struct tcp_connection* conn);
int tcp_send(struct tcp_connection* conn, unsigned char* data, unsigned int length);
int tcp_receive(struct tcp_connection* conn, unsigned char* buffer, unsigned int buffer_size);
int tcp_handle_segment(struct tcp_header* header, unsigned char* data, unsigned int length);

// 内部函数
unsigned short tcp_allocate_port();
void tcp_free_port(unsigned short port);
struct tcp_connection* tcp_find_connection(unsigned short local_port, unsigned short remote_port);
int tcp_send_syn(struct tcp_connection* conn);
int tcp_send_fin(struct tcp_connection* conn);
int tcp_send_rst(struct tcp_header* received_header);
int tcp_send_segment(struct tcp_header* header, unsigned char* data, unsigned int length);
int tcp_handle_syn(struct tcp_connection* conn, struct tcp_header* header, unsigned char* data, unsigned int length);
int tcp_handle_syn_ack(struct tcp_connection* conn, struct tcp_header* header, unsigned char* data, unsigned int length);
int tcp_handle_data(struct tcp_connection* conn, struct tcp_header* header, unsigned char* data, unsigned int length);
int tcp_handle_fin(struct tcp_connection* conn, struct tcp_header* header, unsigned char* data, unsigned int length);

// 辅助函数
unsigned int generate_initial_sequence_number();
struct ip_address get_local_ip();
struct ip_address get_remote_ip();
void ip_address_to_string(struct ip_address ip, char* buffer);

#endif