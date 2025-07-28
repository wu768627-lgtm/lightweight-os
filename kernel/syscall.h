#ifndef SYSCALL_H
#define SYSCALL_H

// 系统调用号定义
#define SYSCALL_PUTCHAR          1
#define SYSCALL_PRINT_STRING     2
#define SYSCALL_EXIT             3
#define SYSCALL_FORK             4
#define SYSCALL_EXEC             5
#define SYSCALL_WAIT             6
#define SYSCALL_SLEEP            7
#define SYSCALL_GETPID           8
#define SYSCALL_MALLOC           9
#define SYSCALL_FREE             10
#define SYSCALL_OPEN             11
#define SYSCALL_READ             12
#define SYSCALL_WRITE            13
#define SYSCALL_CLOSE            14
#define SYSCALL_IOCTL            15
#define SYSCALL_GETCHAR          16
#define SYSCALL_CLEAR_SCREEN     17
#define SYSCALL_CREATE_PROCESS   18
#define SYSCALL_KILL_PROCESS     19
#define SYSCALL_GET_PROCESS_INFO 20
#define SYSCALL_CHDIR            21
#define SYSCALL_GETCWD           22
#define SYSCALL_MKDIR            23
#define SYSCALL_RMDIR            24
#define SYSCALL_UNLINK           25
#define SYSCALL_STAT             26
#define SYSCALL_OPENDIR          27
#define SYSCALL_READDIR          28
#define SYSCALL_CLOSEDIR         29
#define SYSCALL_NETWORK_SOCKET   30
#define SYSCALL_NETWORK_BIND     31
#define SYSCALL_NETWORK_CONNECT  32
#define SYSCALL_NETWORK_LISTEN   33
#define SYSCALL_NETWORK_ACCEPT   34
#define SYSCALL_NETWORK_SEND     35
#define SYSCALL_NETWORK_RECV     36
#define SYSCALL_NETWORK_CLOSE    37
#define SYSCALL_GETTIMEOFDAY     38
#define SYSCALL_LOGGER_LOG       39

// 进程信息结构
struct process_info {
    unsigned int pid;
    unsigned int state;
    unsigned int priority;
    unsigned int memory_usage;
    char name[32];
};

// 文件状态结构
struct stat {
    unsigned int st_dev;
    unsigned int st_ino;
    unsigned int st_mode;
    unsigned int st_nlink;
    unsigned int st_uid;
    unsigned int st_gid;
    unsigned int st_rdev;
    unsigned int st_size;
    unsigned int st_blksize;
    unsigned int st_blocks;
    unsigned int st_atime;
    unsigned int st_mtime;
    unsigned int st_ctime;
};

// 网络地址结构
struct sockaddr {
    unsigned short sa_family;
    char sa_data[14];
};

// 时间结构
struct timeval {
    unsigned int tv_sec;
    unsigned int tv_usec;
};

// 系统调用处理函数声明
void syscall_handler();
void syscall_putchar(char c);
void syscall_print_string(const char* str);
void syscall_exit(int status);
int syscall_fork();
int syscall_exec(const char* path);
int syscall_wait(int pid, int* status);
void syscall_sleep(unsigned int milliseconds);
int syscall_getpid();
void* syscall_malloc(unsigned int size);
void syscall_free(void* ptr);
int syscall_open(const char* pathname, int flags);
int syscall_read(int fd, void* buf, unsigned int count);
int syscall_write(int fd, const void* buf, unsigned int count);
int syscall_close(int fd);
int syscall_ioctl(int fd, unsigned int request, void* argp);
int syscall_getchar();
void syscall_clear_screen();
int syscall_create_process(const char* name, void* entry_point);
int syscall_kill_process(int pid);
int syscall_get_process_info(int pid, struct process_info* info);
int syscall_chdir(const char* path);
int syscall_getcwd(char* buf, unsigned int size);
int syscall_mkdir(const char* pathname, unsigned short mode);
int syscall_rmdir(const char* pathname);
int syscall_unlink(const char* pathname);
int syscall_stat(const char* pathname, struct stat* statbuf);
struct fs_node* syscall_opendir(const char* name);
struct dirent* syscall_readdir(struct fs_node* dirp, struct dirent* entry);
int syscall_closedir(struct fs_node* dirp);
int syscall_network_socket(int domain, int type, int protocol);
int syscall_network_bind(int sockfd, const struct sockaddr* addr, unsigned int addrlen);
int syscall_network_connect(int sockfd, const struct sockaddr* addr, unsigned int addrlen);
int syscall_network_listen(int sockfd, int backlog);
int syscall_network_accept(int sockfd, struct sockaddr* addr, unsigned int* addrlen);
int syscall_network_send(int sockfd, const void* buf, unsigned int len, int flags);
int syscall_network_recv(int sockfd, void* buf, unsigned int len, int flags);
int syscall_network_close(int sockfd);
int syscall_gettimeofday(struct timeval* tv, void* tz);
void syscall_logger_log(int level, const char* module, const char* message);

// 系统调用表
typedef void (*syscall_t)();
extern syscall_t syscall_table[];

#endif