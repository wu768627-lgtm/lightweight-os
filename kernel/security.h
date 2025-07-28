#ifndef SECURITY_H
#define SECURITY_H

#include "../drivers/network.h"

// 安全级别
#define SECURITY_LEVEL_LOW      0
#define SECURITY_LEVEL_MEDIUM   1
#define SECURITY_LEVEL_HIGH     2

// 权限类型
#define PERMISSION_READ         0x01
#define PERMISSION_WRITE        0x02
#define PERMISSION_EXECUTE      0x04
#define PERMISSION_DELETE       0x08

// 最大安全上下文数
#define MAX_SECURITY_CONTEXTS   256

// 用户ID和组ID
typedef unsigned int uid_t;
typedef unsigned int gid_t;

// 进程凭证结构
struct credentials {
    uid_t uid;          // 用户ID
    uid_t euid;         // 有效用户ID
    gid_t gid;          // 组ID
    gid_t egid;         // 有效组ID
    unsigned int permissions; // 进程权限
};

// 安全上下文结构
struct security_context {
    unsigned int level;         // 安全级别
    unsigned int flags;         // 安全标志
    struct credentials creds;   // 进程凭证
    char selinux_context[64];   // SELinux安全上下文
};

// 访问控制列表项
struct acl_entry {
    uid_t uid;
    unsigned int permissions;
    struct acl_entry* next;
};

// 文件安全属性
struct file_security {
    struct security_context context;
    struct acl_entry* acl;
    char selinux_context[64];   // SELinux安全上下文
};

// SELinux安全ID
struct security_id {
    unsigned int sid;
    char context[64];
};

// SELinux策略结构
struct selinux_policy {
    unsigned int enforcing;             // 强制模式标志
    unsigned int policy_version;        // 策略版本
    struct security_id contexts[MAX_SECURITY_CONTEXTS]; // 安全上下文映射
};

// 函数声明
void security_init();
void selinux_init();
int security_check_permission(struct security_context* ctx, unsigned int permission);
int security_check_file_access(struct security_context* ctx, struct file_security* fsec, unsigned int permission);
int security_check_network_access(struct security_context* ctx, struct ip_address dest_ip, unsigned short port);
int selinux_check_permission(struct security_context* ctx, struct file_security* fsec, unsigned int permission);
int selinux_check_network_access(struct security_context* ctx, struct ip_address dest_ip, unsigned short port);
void security_set_level(unsigned int level);
unsigned int security_get_level();
void security_set_credentials(struct credentials* creds, uid_t uid, gid_t gid);
int security_change_owner(struct security_context* ctx, uid_t new_uid);
int security_change_group(struct security_context* ctx, gid_t new_gid);
int security_set_permissions(struct security_context* ctx, unsigned int permissions);
struct security_context* security_get_current_context();
void security_enforce_mandatory_access();

// 加密相关函数
void crypto_init();
unsigned int crypto_hash_md5(unsigned char* data, unsigned int len, unsigned char* hash);
unsigned int crypto_hash_sha256(unsigned char* data, unsigned int len, unsigned char* hash);
int crypto_encrypt_aes(unsigned char* plaintext, unsigned int len, unsigned char* key, unsigned char* ciphertext);
int crypto_decrypt_aes(unsigned char* ciphertext, unsigned int len, unsigned char* key, unsigned char* plaintext);
int crypto_generate_random(unsigned char* buffer, unsigned int len);

// 审计相关函数
void audit_init();
void audit_log_event(const char* event, struct security_context* ctx);
void audit_log_file_access(const char* filename, struct security_context* ctx, unsigned int permission, int allowed);
void audit_log_network_access(struct security_context* ctx, const char* address, unsigned short port, int allowed);

// 辅助函数
char* strstr(const char* haystack, const char* needle);
int strncmp(const char* str1, const char* str2, unsigned int n);
void int_to_string(int value, char* str);

#endif