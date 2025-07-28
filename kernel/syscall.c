#include "kernel.h"
#include "syscall.h"
#include "process.h"
#include "memory.h"
#include "vm.h"
#include "scheduler.h"
#include "logger.h"
#include "profiling.h"
#include "../drivers/filesystem.h"
#include "../drivers/network.h"
#include "../drivers/device.h"
#include "../libs/stdlib.h"

// 系统调用表
syscall_t syscall_table[] = {
    NULL,                    // 0 - 未使用
    (syscall_t)syscall_putchar,     // 1
    (syscall_t)syscall_print_string, // 2
    (syscall_t)syscall_exit,        // 3
    (syscall_t)syscall_fork,        // 4
    (syscall_t)syscall_exec,        // 5
    (syscall_t)syscall_wait,        // 6
    (syscall_t)syscall_sleep,       // 7
    (syscall_t)syscall_getpid,      // 8
    (syscall_t)syscall_malloc,      // 9
    (syscall_t)syscall_free,        // 10
    (syscall_t)syscall_open,        // 11
    (syscall_t)syscall_read,        // 12
    (syscall_t)syscall_write,       // 13
    (syscall_t)syscall_close,       // 14
    (syscall_t)syscall_ioctl,       // 15
    (syscall_t)syscall_getchar,     // 16
    (syscall_t)syscall_clear_screen, // 17
    (syscall_t)syscall_create_process, // 18
    (syscall_t)syscall_kill_process,   // 19
    (syscall_t)syscall_get_process_info, // 20
    (syscall_t)syscall_chdir,        // 21
    (syscall_t)syscall_getcwd,       // 22
    (syscall_t)syscall_mkdir,        // 23
    (syscall_t)syscall_rmdir,        // 24
    (syscall_t)syscall_unlink,       // 25
    (syscall_t)syscall_stat,         // 26
    (syscall_t)syscall_opendir,      // 27
    (syscall_t)syscall_readdir,      // 28
    (syscall_t)syscall_closedir,     // 29
    (syscall_t)syscall_network_socket,   // 30
    (syscall_t)syscall_network_bind,     // 31
    (syscall_t)syscall_network_connect,  // 32
    (syscall_t)syscall_network_listen,   // 33
    (syscall_t)syscall_network_accept,   // 34
    (syscall_t)syscall_network_send,     // 35
    (syscall_t)syscall_network_recv,     // 36
    (syscall_t)syscall_network_close,    // 37
    (syscall_t)syscall_gettimeofday,     // 38
    (syscall_t)syscall_logger_log        // 39
};

// 系统调用处理函数
void syscall_handler(int syscall_num, unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    // 记录系统调用
    profiling_syscall_made();
    
    // 检查系统调用号是否有效
    if (syscall_num < 0 || syscall_num >= SYSCALL_MAX) {
        LOG_ERROR("SYSCALL", "Invalid system call number: %d", syscall_num);
        __asm__ volatile ("mov $-1, %%eax" : : : "eax");  // 返回错误码-1
        return;
    }
    
    // 调用相应的系统调用处理函数
    switch (syscall_num) {
        case SYSCALL_PUTCHAR:
            syscall_putchar((char)arg1);
            break;
            
        case SYSCALL_PRINT_STRING:
            syscall_print_string((const char*)arg1);
            break;
            
        case SYSCALL_EXIT:
            // 增加退出码边界检查
            if (arg1 < 0 || arg1 > 255) {
                LOG_WARNING("SYSCALL", "Invalid exit code: %u", arg1);
                syscall_exit(1); // 使用错误码1
            } else {
                syscall_exit((int)arg1);
            }
            break;
            
        case SYSCALL_FORK:
            syscall_fork();
            break;
            
        case SYSCALL_EXEC:
            // 增加空指针检查
            if ((const char*)arg1 == NULL) {
                LOG_ERROR("SYSCALL", "exec called with NULL path");
                __asm__ volatile ("mov $-1, %%eax" : : : "eax");
                break;
            }
            syscall_exec((const char*)arg1);
            break;
            
        case SYSCALL_WAIT:
            syscall_wait((int)arg1, (int*)arg2);
            break;
            
        case SYSCALL_SLEEP:
            // 增加休眠时间上限检查
            if (arg1 > 1000000) { // 限制最大休眠时间1秒
                LOG_WARNING("SYSCALL", "Sleep time too long: %u ms", arg1);
                __asm__ volatile ("mov $-1, %%eax" : : : "eax");
                break;
            }
            syscall_sleep(arg1);
            break;
            
        case SYSCALL_GETPID:
            syscall_getpid();
            break;
            
        case SYSCALL_MALLOC: {
            // 分配内存系统调用增加边界检查
            if (arg1 == 0 || arg1 > 1024*1024) { // 限制最大分配1MB且不能为0
                LOG_WARNING("SYSCALL", "Invalid malloc size: %u", arg1);
                __asm__ volatile ("mov $0, %%eax" : : : "eax");
                break;
            }
            
            void* ptr = syscall_malloc(arg1);
            // 将分配的指针返回给调用者
            __asm__ volatile ("mov %0, %%eax" : : "r"(ptr));
            break;
        }
            
        case SYSCALL_FREE:
            syscall_free((void*)arg1);
            break;
            
        case SYSCALL_OPEN:
            // 增加空指针检查
            if ((const char*)arg1 == NULL) {
                LOG_ERROR("SYSCALL", "open called with NULL path");
                __asm__ volatile ("mov $-1, %%eax" : : : "eax");
                break;
            }
            syscall_open((const char*)arg1, (int)arg2);
            break;
            
        case SYSCALL_READ:
            // 增加参数有效性检查
            if (arg2 == 0 || arg3 == 0) {
                LOG_ERROR("SYSCALL", "read called with invalid buffer or size");
                __asm__ volatile ("mov $-1, %%eax" : : : "eax");
                break;
            }
            syscall_read((int)arg1, (void*)arg2, arg3);
            break;
            
        case SYSCALL_WRITE:
            // 增加参数有效性检查
            if (arg2 == 0 || arg3 == 0) {
                LOG_ERROR("SYSCALL", "write called with invalid buffer or size");
                __asm__ volatile ("mov $-1, %%eax" : : : "eax");
                break;
            }
            syscall_write((int)arg1, (const void*)arg2, arg3);
            break;
            
        case SYSCALL_CLOSE:
            syscall_close((int)arg1);
            break;
            
        case SYSCALL_IOCTL:
            syscall_ioctl((int)arg1, arg2, (void*)arg3);
            break;
            
        case SYSCALL_GETCHAR:
            syscall_getchar();
            break;
            
        case SYSCALL_CLEAR_SCREEN:
            syscall_clear_screen();
            break;
            
        case SYSCALL_CREATE_PROCESS:
            syscall_create_process((const char*)arg1, (void*)arg2);
            break;
            
        case SYSCALL_KILL_PROCESS:
            syscall_kill_process((int)arg1);
            break;
            
        case SYSCALL_GET_PROCESS_INFO:
            syscall_get_process_info((int)arg1, (struct process_info*)arg2);
            break;
            
        case SYSCALL_CHDIR:
            syscall_chdir((const char*)arg1);
            break;
            
        case SYSCALL_GETCWD:
            syscall_getcwd((char*)arg1, (unsigned int)arg2);
            break;
            
        case SYSCALL_MKDIR:
            syscall_mkdir((const char*)arg1, (unsigned short)arg2);
            break;
            
        case SYSCALL_RMDIR:
            syscall_rmdir((const char*)arg1);
            break;
            
        case SYSCALL_UNLINK:
            syscall_unlink((const char*)arg1);
            break;
            
        case SYSCALL_STAT:
            syscall_stat((const char*)arg1, (struct stat*)arg2);
            break;
            
        case SYSCALL_OPENDIR:
            syscall_opendir((const char*)arg1);
            break;
            
        case SYSCALL_READDIR:
            syscall_readdir((struct fs_node*)arg1, (struct dirent*)arg2);
            break;
            
        case SYSCALL_CLOSEDIR:
            syscall_closedir((struct fs_node*)arg1);
            break;
            
        case SYSCALL_NETWORK_SOCKET:
            syscall_network_socket((int)arg1, (int)arg2, (int)arg3);
            break;
            
        case SYSCALL_NETWORK_BIND:
            syscall_network_bind((int)arg1, (const struct sockaddr*)arg2, (unsigned int)arg3);
            break;
            
        case SYSCALL_NETWORK_CONNECT:
            syscall_network_connect((int)arg1, (const struct sockaddr*)arg2, (unsigned int)arg3);
            break;
            
        case SYSCALL_NETWORK_LISTEN:
            syscall_network_listen((int)arg1, (int)arg2);
            break;
            
        case SYSCALL_NETWORK_ACCEPT:
            syscall_network_accept((int)arg1, (struct sockaddr*)arg2, (unsigned int*)arg3);
            break;
            
        case SYSCALL_NETWORK_SEND:
            // 增加参数有效性检查
            if (arg2 == 0 || arg3 == 0) {
                LOG_ERROR("SYSCALL", "send called with invalid buffer or size");
                __asm__ volatile ("mov $-1, %%eax" : : : "eax");
                break;
            }
            syscall_network_send((int)arg1, (const void*)arg2, (unsigned int)arg3, (int)arg4);
            break;
            
        case SYSCALL_NETWORK_RECV:
            // 增加参数有效性检查
            if (arg2 == 0 || arg3 == 0) {
                LOG_ERROR("SYSCALL", "recv called with invalid buffer or size");
                __asm__ volatile ("mov $-1, %%eax" : : : "eax");
                break;
            }
            syscall_network_recv((int)arg1, (void*)arg2, (unsigned int)arg3, (int)arg4);
            break;
            
        case SYSCALL_NETWORK_CLOSE:
            syscall_network_close((int)arg1);
            break;
            
        case SYSCALL_GETTIMEOFDAY:
            syscall_gettimeofday((struct timeval*)arg1, (void*)arg2);
            break;
            
        case SYSCALL_LOGGER_LOG:
            syscall_logger_log((int)arg1, (const char*)arg2, (const char*)arg3);
            break;
            
        default:
            LOG_WARNING("SYSCALL", "Unhandled system call");
            print_string("Unhandled system call: ");
            char num_str[12];
            int_to_string(syscall_num, num_str);
            print_string(num_str);
            print_string("\n");
            break;
    }
}

// 系统调用实现函数
void syscall_putchar(char c) {
    // 直接调用内核的打印函数
    char str[2] = {c, '\0'};
    print_string(str);
}

void syscall_print_string(const char* str) {
    if (str == NULL) {
        LOG_ERROR("SYSCALL", "print_string received NULL pointer");
        return;
    }
    
    // 限制最大打印长度，防止无限循环
    int len = 0;
    while (str[len] && len < 1024) {
        len++;
    }
    
    for (int i = 0; i < len; i++) {
        syscall_putchar(str[i]);
    }
}

void syscall_exit(int status) {
    // 验证退出状态
    if (status < 0 || status > 255) {
        LOG_WARNING("SYSCALL", "Invalid exit status %d, using 1 instead", status);
        status = 1;
    }
    
    LOG_INFO("PROCESS", "Process exiting with status: %d", status);
    print_string("Process exited with status: ");
    char status_str[12];
    int_to_string(status, status_str);
    print_string(status_str);
    print_string("\n");
    
    // 在实际实现中，这里会终止当前进程并调度下一个进程
    // 简化实现，仅打印信息
}

int syscall_fork() {
    LOG_INFO("PROCESS", "Fork system call called");
    print_string("Fork system call called\n");
    // 在实际实现中，这里会创建一个新进程
    return 0;
}

int syscall_exec(const char* path) {
    // 验证参数
    if (!path) {
        LOG_ERROR("SYSCALL", "exec called with NULL path");
        return -1;
    }
    
    LOG_INFO("PROCESS", "Exec system call called for: %s", path);
    print_string("Exec system call called for: ");
    print_string((char*)path);
    print_string("\n");
    
    // 在实际实现中，这里会加载并执行指定程序
    return 0;
}

int syscall_wait(int pid, int* status) {
    LOG_INFO("PROCESS", "Wait system call called");
    print_string("Wait system call called for PID: ");
    char pid_str[12];
    int_to_string(pid, pid_str);
    print_string(pid_str);
    print_string("\n");
    // 在实际实现中，这里会等待指定进程结束
    return 0;
}

void syscall_sleep(unsigned int milliseconds) {
    LOG_INFO("PROCESS", "Sleep system call called");
    print_string("Sleep system call called for: ");
    char ms_str[12];
    int_to_string(milliseconds, ms_str);
    print_string(ms_str);
    print_string(" ms\n");
    // 在实际实现中，这里会将当前进程置于等待状态指定时间
}

int syscall_getpid() {
    LOG_INFO("PROCESS", "GetPID system call called");
    print_string("GetPID system call called\n");
    // 在实际实现中，这里会返回当前进程ID
    return 1; // 简化实现，返回固定值
}

void* syscall_malloc(unsigned int size) {
    LOG_INFO("MEMORY", "Malloc system call called for %u bytes", size);
    profiling_memory_alloc(size);
    
    // 调用内核内存分配函数
    void* ptr = allocate_memory(size);
    
    if (ptr == NULL) {
        LOG_ERROR("MEMORY", "Failed to allocate %u bytes", size);
    } else {
        LOG_DEBUG("MEMORY", "Allocated %u bytes at %p", size, ptr);
    }
    
    return ptr;
}

void syscall_free(void* ptr) {
    LOG_INFO("MEMORY", "Free system call called");
    print_string("Free system call called\n");
    // 调用内核内存释放函数
    free_memory(ptr);
    // 在实际实现中，我们会知道释放的内存大小
    profiling_memory_free(0);
}

int syscall_open(const char* pathname, int flags) {
    LOG_INFO("FILESYSTEM", "Open system call called");
    print_string("Open system call called for: ");
    print_string((char*)pathname);
    print_string("\n");
    // 在实际实现中，这里会打开指定文件
    return 1; // 简化实现，返回固定文件描述符
}

int syscall_read(int fd, void* buf, unsigned int count) {
    // 验证参数
    if (!buf || count == 0) {
        LOG_ERROR("SYSCALL", "read called with invalid buffer or count");
        return -1;
    }
    
    if (fd < 0) {
        LOG_ERROR("SYSCALL", "read called with invalid file descriptor");
        return -1;
    }
    
    LOG_INFO("FILESYSTEM", "Read system call called for FD: %d", fd);
    print_string("Read system call called\n");
    
    // 在实际实现中，这里会从文件描述符读取数据
    return 0; // 简化实现，返回读取字节数
}

int syscall_write(int fd, const void* buf, unsigned int count) {
    // 验证参数
    if (!buf || count == 0) {
        LOG_ERROR("SYSCALL", "write called with invalid buffer or count");
        return -1;
    }
    
    if (fd < 0) {
        LOG_ERROR("SYSCALL", "write called with invalid file descriptor");
        return -1;
    }
    
    LOG_INFO("FILESYSTEM", "Write system call called for FD: %d", fd);
    print_string("Write system call called\n");
    
    // 在实际实现中，这里会向文件描述符写入数据
    return count; // 简化实现，返回写入字节数
}

int syscall_close(int fd) {
    LOG_INFO("FILESYSTEM", "Close system call called");
    print_string("Close system call called for FD: ");
    char fd_str[12];
    int_to_string(fd, fd_str);
    print_string(fd_str);
    print_string("\n");
    // 在实际实现中，这里会关闭文件描述符
    return 0;
}

int syscall_ioctl(int fd, unsigned int request, void* argp) {
    LOG_INFO("DEVICE", "IOCTL system call called");
    print_string("IOCTL system call called\n");
    // 在实际实现中，这里会执行设备特定的I/O控制操作
    return 0;
}

int syscall_getchar() {
    // 在实际实现中，这里会从键盘缓冲区获取字符
    // 简化实现，返回固定值
    return 0;
}

void syscall_clear_screen() {
    // 清屏操作
    char *video_memory = (char *)0xb8000;
    for (int i = 0; i < 80 * 25 * 2; i++) {
        video_memory[i] = 0;
    }
    cursor_x = 0;
    cursor_y = 0;
}

int syscall_create_process(const char* name, void* entry_point) {
    LOG_INFO("PROCESS", "Create process system call called");
    print_string("Create process system call called for: ");
    print_string((char*)name);
    print_string("\n");
    // 在实际实现中，这里会创建一个新进程
    return 0;
}

int syscall_kill_process(int pid) {
    LOG_INFO("PROCESS", "Kill process system call called");
    print_string("Kill process system call called for PID: ");
    char pid_str[12];
    int_to_string(pid, pid_str);
    print_string(pid_str);
    print_string("\n");
    // 在实际实现中，这里会终止指定进程
    return 0;
}

int syscall_get_process_info(int pid, struct process_info* info) {
    LOG_INFO("PROCESS", "Get process info system call called");
    // 在实际实现中，这里会获取进程信息
    return 0;
}

int syscall_chdir(const char* path) {
    LOG_INFO("FILESYSTEM", "Change directory system call called");
    print_string("Change directory system call called for: ");
    print_string((char*)path);
    print_string("\n");
    // 在实际实现中，这里会改变当前工作目录
    return 0;
}

int syscall_getcwd(char* buf, unsigned int size) {
    LOG_INFO("FILESYSTEM", "Get current directory system call called");
    // 在实际实现中，这里会获取当前工作目录
    return 0;
}

int syscall_mkdir(const char* pathname, unsigned short mode) {
    LOG_INFO("FILESYSTEM", "Make directory system call called");
    print_string("Make directory system call called for: ");
    print_string((char*)pathname);
    print_string("\n");
    // 在实际实现中，这里会创建目录
    return 0;
}

int syscall_rmdir(const char* pathname) {
    LOG_INFO("FILESYSTEM", "Remove directory system call called");
    print_string("Remove directory system call called for: ");
    print_string((char*)pathname);
    print_string("\n");
    // 在实际实现中，这里会删除空目录
    return 0;
}

int syscall_unlink(const char* pathname) {
    LOG_INFO("FILESYSTEM", "Unlink system call called");
    print_string("Unlink system call called for: ");
    print_string((char*)pathname);
    print_string("\n");
    // 在实际实现中，这里会删除文件
    return 0;
}

int syscall_stat(const char* pathname, struct stat* statbuf) {
    LOG_INFO("FILESYSTEM", "Stat system call called");
    // 在实际实现中，这里会获取文件状态信息
    return 0;
}

struct fs_node* syscall_opendir(const char* name) {
    LOG_INFO("FILESYSTEM", "Open directory system call called");
    // 在实际实现中，这里会打开目录
    return 0;
}

struct dirent* syscall_readdir(struct fs_node* dirp, struct dirent* entry) {
    LOG_INFO("FILESYSTEM", "Read directory system call called");
    // 在实际实现中，这里会读取目录项
    return 0;
}

int syscall_closedir(struct fs_node* dirp) {
    LOG_INFO("FILESYSTEM", "Close directory system call called");
    // 在实际实现中，这里会关闭目录
    return 0;
}

int syscall_network_socket(int domain, int type, int protocol) {
    LOG_INFO("NETWORK", "Socket system call called");
    // 在实际实现中，这里会创建套接字
    return 0;
}

int syscall_network_bind(int sockfd, const struct sockaddr* addr, unsigned int addrlen) {
    LOG_INFO("NETWORK", "Bind system call called");
    // 在实际实现中，这里会绑定套接字
    return 0;
}

int syscall_network_connect(int sockfd, const struct sockaddr* addr, unsigned int addrlen) {
    LOG_INFO("NETWORK", "Connect system call called");
    // 在实际实现中，这里会连接到远程主机
    return 0;
}

int syscall_network_listen(int sockfd, int backlog) {
    LOG_INFO("NETWORK", "Listen system call called");
    // 在实际实现中，这里会使套接字进入监听状态
    return 0;
}

int syscall_network_accept(int sockfd, struct sockaddr* addr, unsigned int* addrlen) {
    LOG_INFO("NETWORK", "Accept system call called");
    // 在实际实现中，这里会接受连接请求
    return 0;
}

int syscall_network_send(int sockfd, const void* buf, unsigned int len, int flags) {
    LOG_INFO("NETWORK", "Send system call called");
    // 在实际实现中，这里会发送数据
    return len;
}

int syscall_network_recv(int sockfd, void* buf, unsigned int len, int flags) {
    LOG_INFO("NETWORK", "Receive system call called");
    // 在实际实现中，这里会接收数据
    return 0;
}

int syscall_network_close(int sockfd) {
    LOG_INFO("NETWORK", "Close socket system call called");
    // 在实际实现中，这里会关闭套接字
    return 0;
}

int syscall_gettimeofday(struct timeval* tv, void* tz) {
    LOG_INFO("TIME", "Get time of day system call called");
    // 在实际实现中，这里会获取当前时间
    if (tv) {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
    }
    return 0;
}

void syscall_logger_log(int level, const char* module, const char* message) {
    logger_log(level, module, message);
}

// 整数转字符串辅助函数
void int_to_string(int value, char* str) {
    if (!str) {
        LOG_ERROR("SYSCALL", "int_to_string called with NULL buffer");
        return;
    }
    
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
    while (value > 0 && i < 11) {  // 增加边界检查
        temp[i] = (value % 10) + '0';
        value /= 10;
        i++;
    }
    temp[i] = '\0';
    
    int j = 0;
    if (is_negative) {
        str[j++] = '-';
    }
    
    for (int k = i - 1; k >= 0; k--) {
        str[j++] = temp[k];
    }
    str[j] = '\0';
}