#ifndef NETWORK_H
#define NETWORK_H

// 网络协议类型
#define PROTOCOL_ICMP   1
#define PROTOCOL_TCP    6
#define PROTOCOL_UDP    17

// IP地址结构
struct ip_address {
    unsigned char octet[4];
};

// MAC地址结构
struct mac_address {
    unsigned char octet[6];
};

// 网络接口结构
struct net_interface {
    char name[16];                      // 接口名称
    struct mac_address mac;             // MAC地址
    struct ip_address ip;               // IP地址
    struct ip_address netmask;          // 子网掩码
    struct ip_address gateway;          // 网关地址
    unsigned int flags;                 // 接口标志
    void* device;                       // 指向设备特定数据的指针
    struct net_interface* next;         // 链表中的下一个接口
};

// 网络数据包结构
struct net_packet {
    unsigned char* data;        // 数据包数据
    unsigned int length;        // 数据包长度
    unsigned int capacity;      // 数据包容量
    struct net_interface* interface; // 接收/发送接口
};

// TCP连接结构
struct tcp_connection {
    struct ip_address local_ip;
    unsigned short local_port;
    struct ip_address remote_ip;
    unsigned short remote_port;
    unsigned int state;
    unsigned int sequence_number;
    unsigned int acknowledgment_number;
    struct tcp_connection* next;
};

// UDP套接字结构
struct udp_socket {
    struct ip_address local_ip;
    unsigned short local_port;
    struct ip_address remote_ip;
    unsigned short remote_port;
    struct udp_socket* next;
};

// 函数声明
void net_init();
int net_interface_register(struct net_interface* interface);
int net_interface_unregister(const char* name);
struct net_interface* net_interface_get(const char* name);
struct net_interface* net_interface_get_by_ip(struct ip_address ip);
void net_interface_list();
int net_send_packet(struct net_interface* interface, struct net_packet* packet);
int net_receive_packet(struct net_interface* interface, struct net_packet* packet);
void net_set_ip_address(struct net_interface* interface, struct ip_address ip);
void net_set_gateway(struct net_interface* interface, struct ip_address gateway);
void net_set_netmask(struct net_interface* interface, struct ip_address netmask);

// ARP相关函数
void arp_init();
int arp_resolve(struct net_interface* interface, struct ip_address ip, struct mac_address* mac);
void arp_cache_add(struct ip_address ip, struct mac_address mac);
void arp_request(struct net_interface* interface, struct ip_address ip);

// IP相关函数
void ip_init();
int ip_send_packet(struct ip_address dest_ip, unsigned char protocol, unsigned char* data, unsigned int length);
int ip_receive_packet(struct net_packet* packet);

// ICMP相关函数
void icmp_init();
int icmp_send_echo_request(struct ip_address dest_ip, unsigned short id, unsigned short sequence, unsigned char* data, unsigned int length);
int icmp_receive_echo_request(struct net_packet* packet);
int icmp_receive_echo_reply(struct net_packet* packet);

// TCP相关函数
void tcp_init();
struct tcp_connection* tcp_connect(struct ip_address dest_ip, unsigned short dest_port);
int tcp_send(struct tcp_connection* conn, unsigned char* data, unsigned int length);
int tcp_receive(struct tcp_connection* conn, unsigned char* buffer, unsigned int buffer_size);
void tcp_close(struct tcp_connection* conn);

// UDP相关函数
void udp_init();
struct udp_socket* udp_open(unsigned short local_port);
int udp_send(struct udp_socket* socket, struct ip_address dest_ip, unsigned short dest_port, unsigned char* data, unsigned int length);
int udp_receive(struct udp_socket* socket, unsigned char* buffer, unsigned int buffer_size);
void udp_close(struct udp_socket* socket);

// DNS相关函数
void dns_init();
int dns_resolve(const char* hostname, struct ip_address* ip);

// DHCP相关函数
void dhcp_init();
int dhcp_request_address(struct net_interface* interface);

// 实用函数
void ip_address_to_string(struct ip_address ip, char* buffer);
void mac_address_to_string(struct mac_address mac, char* buffer);
int string_to_ip_address(const char* str, struct ip_address* ip);
int string_to_mac_address(const char* str, struct mac_address* mac);

#endif