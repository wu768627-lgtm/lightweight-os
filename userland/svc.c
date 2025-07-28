// svc.c - 系统服务管理器
#include "../libs/stdlib.h"
#include "../libs/string.h"
#include "../kernel/syscall.h"

// 服务结构
struct service {
    char name[32];
    char path[128];
    int pid;
    int enabled;
    int autostart;
};

// 系统服务列表
#define MAX_SERVICES 16
static struct service services[MAX_SERVICES];
static int service_count = 0;

// 函数声明
void print_help();
void list_services();
void start_service(const char* name);
void stop_service(const char* name);
void enable_service(const char* name);
void disable_service(const char* name);
void reload_service(const char* name);
void show_status(const char* name);
int find_service(const char* name);
void init_services();

// 系统调用模拟函数
static inline void syscall_print_string(const char* str) {
    while (*str) {
        syscall_putchar(*str);
        str++;
    }
}

static inline void syscall_putchar(char c) {
    __asm__ volatile (
        "mov $1, %%eax\n\t"
        "mov %0, %%ebx\n\t"
        "int $0x80\n\t"
        :
        : "r"(c)
        : "eax", "ebx"
    );
}

static inline int syscall_create_process(const char* name, void* entry_point) {
    int pid;
    __asm__ volatile (
        "mov $18, %%eax\n\t"
        "mov %1, %%ebx\n\t"
        "mov %2, %%ecx\n\t"
        "int $0x80\n\t"
        "mov %%eax, %0\n\t"
        : "=r"(pid)
        : "r"(name), "r"(entry_point)
        : "eax", "ebx", "ecx"
    );
    return pid;
}

static inline int syscall_kill_process(int pid) {
    int result;
    __asm__ volatile (
        "mov $19, %%eax\n\t"
        "mov %1, %%ebx\n\t"
        "int $0x80\n\t"
        "mov %%eax, %0\n\t"
        : "=r"(result)
        : "r"(pid)
        : "eax", "ebx"
    );
    return result;
}

// 主函数
int main(int argc, char* argv[]) {
    // 初始化服务
    init_services();
    
    // 检查参数
    if (argc < 2) {
        print_help();
        return 0;
    }
    
    // 根据参数执行相应操作
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help();
    } else if (strcmp(argv[1], "list") == 0) {
        list_services();
    } else if (strcmp(argv[1], "start") == 0) {
        if (argc < 3) {
            syscall_print_string("Usage: svc start <service>\n");
        } else {
            start_service(argv[2]);
        }
    } else if (strcmp(argv[1], "stop") == 0) {
        if (argc < 3) {
            syscall_print_string("Usage: svc stop <service>\n");
        } else {
            stop_service(argv[2]);
        }
    } else if (strcmp(argv[1], "enable") == 0) {
        if (argc < 3) {
            syscall_print_string("Usage: svc enable <service>\n");
        } else {
            enable_service(argv[2]);
        }
    } else if (strcmp(argv[1], "disable") == 0) {
        if (argc < 3) {
            syscall_print_string("Usage: svc disable <service>\n");
        } else {
            disable_service(argv[2]);
        }
    } else if (strcmp(argv[1], "reload") == 0) {
        if (argc < 3) {
            syscall_print_string("Usage: svc reload <service>\n");
        } else {
            reload_service(argv[2]);
        }
    } else if (strcmp(argv[1], "status") == 0) {
        if (argc < 3) {
            syscall_print_string("Usage: svc status <service>\n");
        } else {
            show_status(argv[2]);
        }
    } else {
        syscall_print_string("Unknown command: ");
        syscall_print_string(argv[1]);
        syscall_putchar('\n');
        print_help();
    }
    
    return 0;
}

// 打印帮助信息
void print_help() {
    syscall_print_string("Service Manager (svc) - LightweightOS Service Control\n");
    syscall_print_string("Usage: svc [command] [service]\n\n");
    syscall_print_string("Commands:\n");
    syscall_print_string("  list              List all services\n");
    syscall_print_string("  start <service>   Start a service\n");
    syscall_print_string("  stop <service>    Stop a service\n");
    syscall_print_string("  enable <service>  Enable a service\n");
    syscall_print_string("  disable <service> Disable a service\n");
    syscall_print_string("  reload <service>  Reload a service\n");
    syscall_print_string("  status <service>  Show service status\n");
    syscall_print_string("  -h, --help        Show this help message\n");
}

// 初始化服务
void init_services() {
    // 添加一些示例服务
    strcpy(services[0].name, "network");
    strcpy(services[0].path, "/bin/networkd");
    services[0].pid = -1;
    services[0].enabled = 1;
    services[0].autostart = 1;
    
    strcpy(services[1].name, "filesystem");
    strcpy(services[1].path, "/bin/filesystemd");
    services[1].pid = -1;
    services[1].enabled = 1;
    services[1].autostart = 1;
    
    strcpy(services[2].name, "logging");
    strcpy(services[2].path, "/bin/loggingd");
    services[2].pid = -1;
    services[2].enabled = 1;
    services[2].autostart = 1;
    
    service_count = 3;
}

// 列出所有服务
void list_services() {
    syscall_print_string("Services:\n");
    syscall_print_string("Name        Status   PID   Enabled  Autostart\n");
    syscall_print_string("----        ------   ---   -------  ---------\n");
    
    char buffer[32];
    for (int i = 0; i < service_count; i++) {
        // 服务名称
        syscall_print_string(services[i].name);
        int name_len = strlen(services[i].name);
        for (int j = 0; j < 12 - name_len; j++) {
            syscall_putchar(' ');
        }
        
        // 状态
        if (services[i].pid > 0) {
            syscall_print_string("running  ");
        } else if (services[i].pid == 0) {
            syscall_print_string("starting ");
        } else {
            syscall_print_string("stopped  ");
        }
        
        // PID
        if (services[i].pid > 0) {
            int_to_string(services[i].pid, buffer);
            syscall_print_string(buffer);
        } else {
            syscall_print_string("N/A");
        }
        syscall_print_string("   ");
        
        // 启用状态
        if (services[i].enabled) {
            syscall_print_string("yes      ");
        } else {
            syscall_print_string("no       ");
        }
        
        // 自启动状态
        if (services[i].autostart) {
            syscall_print_string("yes\n");
        } else {
            syscall_print_string("no\n");
        }
    }
}

// 启动服务
void start_service(const char* name) {
    int index = find_service(name);
    if (index < 0) {
        syscall_print_string("Service not found: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    if (!services[index].enabled) {
        syscall_print_string("Service is disabled: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    if (services[index].pid > 0) {
        syscall_print_string("Service already running: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    syscall_print_string("Starting service ");
    syscall_print_string((char*)name);
    syscall_print_string("...\n");
    
    // 在实际实现中，这里会创建进程来运行服务
    services[index].pid = 100 + index; // 模拟PID
    
    syscall_print_string("Service ");
    syscall_print_string((char*)name);
    syscall_print_string(" started with PID ");
    char pid_str[12];
    int_to_string(services[index].pid, pid_str);
    syscall_print_string(pid_str);
    syscall_putchar('\n');
}

// 停止服务
void stop_service(const char* name) {
    int index = find_service(name);
    if (index < 0) {
        syscall_print_string("Service not found: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    if (services[index].pid <= 0) {
        syscall_print_string("Service not running: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    syscall_print_string("Stopping service ");
    syscall_print_string((char*)name);
    syscall_print_string("...\n");
    
    // 在实际实现中，这里会终止服务进程
    int result = syscall_kill_process(services[index].pid);
    if (result == 0) {
        services[index].pid = -1;
        syscall_print_string("Service ");
        syscall_print_string((char*)name);
        syscall_print_string(" stopped\n");
    } else {
        syscall_print_string("Failed to stop service ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
    }
}

// 启用服务
void enable_service(const char* name) {
    int index = find_service(name);
    if (index < 0) {
        syscall_print_string("Service not found: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    if (services[index].enabled) {
        syscall_print_string("Service already enabled: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    services[index].enabled = 1;
    syscall_print_string("Service ");
    syscall_print_string((char*)name);
    syscall_print_string(" enabled\n");
}

// 禁用服务
void disable_service(const char* name) {
    int index = find_service(name);
    if (index < 0) {
        syscall_print_string("Service not found: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    if (!services[index].enabled) {
        syscall_print_string("Service already disabled: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    // 如果服务正在运行，先停止它
    if (services[index].pid > 0) {
        stop_service(name);
    }
    
    services[index].enabled = 0;
    syscall_print_string("Service ");
    syscall_print_string((char*)name);
    syscall_print_string(" disabled\n");
}

// 重新加载服务
void reload_service(const char* name) {
    int index = find_service(name);
    if (index < 0) {
        syscall_print_string("Service not found: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    syscall_print_string("Reloading service ");
    syscall_print_string((char*)name);
    syscall_print_string("...\n");
    
    // 如果服务正在运行，重启它
    if (services[index].pid > 0) {
        stop_service(name);
        start_service(name);
    }
    
    syscall_print_string("Service ");
    syscall_print_string((char*)name);
    syscall_print_string(" reloaded\n");
}

// 显示服务状态
void show_status(const char* name) {
    int index = find_service(name);
    if (index < 0) {
        syscall_print_string("Service not found: ");
        syscall_print_string((char*)name);
        syscall_putchar('\n');
        return;
    }
    
    syscall_print_string("Service: ");
    syscall_print_string(services[index].name);
    syscall_putchar('\n');
    
    syscall_print_string("Path: ");
    syscall_print_string(services[index].path);
    syscall_putchar('\n');
    
    syscall_print_string("Status: ");
    if (services[index].pid > 0) {
        syscall_print_string("running (PID: ");
        char pid_str[12];
        int_to_string(services[index].pid, pid_str);
        syscall_print_string(pid_str);
        syscall_print_string(")\n");
    } else if (services[index].pid == 0) {
        syscall_print_string("starting\n");
    } else {
        syscall_print_string("stopped\n");
    }
    
    syscall_print_string("Enabled: ");
    if (services[index].enabled) {
        syscall_print_string("yes\n");
    } else {
        syscall_print_string("no\n");
    }
    
    syscall_print_string("Autostart: ");
    if (services[index].autostart) {
        syscall_print_string("yes\n");
    } else {
        syscall_print_string("no\n");
    }
}

// 查找服务
int find_service(const char* name) {
    for (int i = 0; i < service_count; i++) {
        if (strcmp(services[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// 字符串长度
int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// 字符串复制
char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
}

// 字符串比较
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
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