// sysmon.c - 系统监控和诊断工具
#include "../libs/stdlib.h"
#include "../libs/string.h"
#include "../kernel/syscall.h"
#include "../kernel/profiling.h"
#include "../kernel/logger.h"

// 系统监控数据结构
struct system_info {
    unsigned int uptime;           // 系统运行时间(秒)
    unsigned int total_memory;     // 总内存(KB)
    unsigned int free_memory;      // 空闲内存(KB)
    unsigned int cpu_usage;        // CPU使用率(百分比)
    unsigned int process_count;    // 进程数
};

struct process_list_entry {
    unsigned int pid;
    unsigned int state;
    unsigned int priority;
    unsigned int memory_usage;
    char name[32];
};

// 函数声明
void print_help();
void show_system_info();
void show_process_list();
void show_performance_stats();
void show_log_stats();
void show_memory_map();
void show_disk_usage();
void show_network_stats();

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

static inline int syscall_get_system_info(struct system_info* info) {
    // 模拟系统信息获取
    if (info) {
        info->uptime = 3600; // 1小时
        info->total_memory = 131072; // 128MB
        info->free_memory = 65536; // 64MB
        info->cpu_usage = 15; // 15%
        info->process_count = 8;
        return 0;
    }
    return -1;
}

static inline int syscall_get_process_list(struct process_list_entry* list, unsigned int max_count) {
    // 模拟进程列表获取
    if (list && max_count >= 3) {
        // 进程1
        list[0].pid = 1;
        list[0].state = 1; // 运行
        list[0].priority = 10;
        list[0].memory_usage = 2048; // 2MB
        strcpy(list[0].name, "init");
        
        // 进程2
        list[1].pid = 2;
        list[1].state = 2; // 等待
        list[1].priority = 5;
        list[1].memory_usage = 1024; // 1MB
        strcpy(list[1].name, "shell");
        
        // 进程3
        list[2].pid = 3;
        list[2].state = 1; // 运行
        list[2].priority = 8;
        list[2].memory_usage = 512; // 0.5MB
        strcpy(list[2].name, "sysmon");
        
        return 3;
    }
    return 0;
}

// 主函数
int main(int argc, char* argv[]) {
    // 检查参数
    if (argc < 2) {
        print_help();
        return 0;
    }
    
    // 根据参数执行相应操作
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help();
    } else if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0) {
        show_system_info();
    } else if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--processes") == 0) {
        show_process_list();
    } else if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--stats") == 0) {
        show_performance_stats();
    } else if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--logs") == 0) {
        show_log_stats();
    } else if (strcmp(argv[1], "-m") == 0 || strcmp(argv[1], "--memory") == 0) {
        show_memory_map();
    } else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--disk") == 0) {
        show_disk_usage();
    } else if (strcmp(argv[1], "-n") == 0 || strcmp(argv[1], "--network") == 0) {
        show_network_stats();
    } else {
        syscall_print_string("Unknown option: ");
        syscall_print_string(argv[1]);
        syscall_putchar('\n');
        print_help();
    }
    
    return 0;
}

// 打印帮助信息
void print_help() {
    syscall_print_string("System Monitor (sysmon) - LightweightOS Diagnostic Tool\n");
    syscall_print_string("Usage: sysmon [option]\n\n");
    syscall_print_string("Options:\n");
    syscall_print_string("  -h, --help        Show this help message\n");
    syscall_print_string("  -i, --info        Show system information\n");
    syscall_print_string("  -p, --processes   Show process list\n");
    syscall_print_string("  -s, --stats       Show performance statistics\n");
    syscall_print_string("  -l, --logs        Show log statistics\n");
    syscall_print_string("  -m, --memory      Show memory map\n");
    syscall_print_string("  -d, --disk        Show disk usage\n");
    syscall_print_string("  -n, --network     Show network statistics\n");
}

// 显示系统信息
void show_system_info() {
    struct system_info info;
    
    syscall_print_string("=== System Information ===\n");
    
    if (syscall_get_system_info(&info) == 0) {
        char buffer[32];
        
        syscall_print_string("Uptime: ");
        int_to_string(info.uptime, buffer);
        syscall_print_string(buffer);
        syscall_print_string(" seconds\n");
        
        syscall_print_string("Total Memory: ");
        int_to_string(info.total_memory, buffer);
        syscall_print_string(buffer);
        syscall_print_string(" KB\n");
        
        syscall_print_string("Free Memory: ");
        int_to_string(info.free_memory, buffer);
        syscall_print_string(buffer);
        syscall_print_string(" KB\n");
        
        syscall_print_string("CPU Usage: ");
        int_to_string(info.cpu_usage, buffer);
        syscall_print_string(buffer);
        syscall_print_string("%\n");
        
        syscall_print_string("Processes: ");
        int_to_string(info.process_count, buffer);
        syscall_print_string(buffer);
        syscall_print_string("\n");
    } else {
        syscall_print_string("Failed to get system information\n");
    }
}

// 显示进程列表
void show_process_list() {
    struct process_list_entry processes[16];
    int count = syscall_get_process_list(processes, 16);
    
    syscall_print_string("=== Process List ===\n");
    syscall_print_string("PID  STATE  PRI  MEMORY   NAME\n");
    syscall_print_string("---- -----  ---  ------   ----\n");
    
    char buffer[32];
    for (int i = 0; i < count; i++) {
        // PID
        int_to_string(processes[i].pid, buffer);
        syscall_print_string(buffer);
        syscall_print_string("   ");
        
        // STATE
        switch (processes[i].state) {
            case 0:
                syscall_print_string("STOP ");
                break;
            case 1:
                syscall_print_string("RUN  ");
                break;
            case 2:
                syscall_print_string("WAIT ");
                break;
            default:
                syscall_print_string("UNKN ");
                break;
        }
        
        // PRIORITY
        int_to_string(processes[i].priority, buffer);
        if (processes[i].priority < 10) syscall_print_string(" ");
        syscall_print_string(buffer);
        syscall_print_string("   ");
        
        // MEMORY
        int_to_string(processes[i].memory_usage, buffer);
        int len = strlen(buffer);
        for (int j = 0; j < 6 - len; j++) {
            syscall_putchar(' ');
        }
        syscall_print_string(buffer);
        syscall_print_string("   ");
        
        // NAME
        syscall_print_string(processes[i].name);
        syscall_putchar('\n');
    }
}

// 显示性能统计
void show_performance_stats() {
    syscall_print_string("=== Performance Statistics ===\n");
    syscall_print_string("Note: Detailed statistics would be shown in a full implementation\n");
    
    // 在完整实现中，这里会显示实际的性能统计数据
    syscall_print_string("- CPU usage by component\n");
    syscall_print_string("- Memory allocation statistics\n");
    syscall_print_string("- Disk I/O performance\n");
    syscall_print_string("- Network throughput\n");
    syscall_print_string("- Context switch frequency\n");
    syscall_print_string("- Interrupt handling stats\n");
}

// 显示日志统计
void show_log_stats() {
    syscall_print_string("=== Log Statistics ===\n");
    syscall_print_string("Note: Log statistics would be shown in a full implementation\n");
    
    // 在完整实现中，这里会显示实际的日志统计数据
    syscall_print_string("- Total log entries: N/A\n");
    syscall_print_string("- Debug messages: N/A\n");
    syscall_print_string("- Info messages: N/A\n");
    syscall_print_string("- Warning messages: N/A\n");
    syscall_print_string("- Error messages: N/A\n");
    syscall_print_string("- Critical messages: N/A\n");
    syscall_print_string("- Buffer usage: N/A%\n");
}

// 显示内存映射
void show_memory_map() {
    syscall_print_string("=== Memory Map ===\n");
    syscall_print_string("Note: Memory map would be shown in a full implementation\n");
    
    // 在完整实现中，这里会显示实际的内存映射
    syscall_print_string("Address Range     Type       Status\n");
    syscall_print_string("-------------     ----       ------\n");
    syscall_print_string("0x00000000-       Kernel     Used\n");
    syscall_print_string("0x00100000-       Available  Free\n");
    syscall_print_string("0x00200000-       Process    Used\n");
    syscall_print_string("0x00400000-       Available  Free\n");
    syscall_print_string("...\n");
}

// 显示磁盘使用情况
void show_disk_usage() {
    syscall_print_string("=== Disk Usage ===\n");
    syscall_print_string("Note: Disk usage would be shown in a full implementation\n");
    
    // 在完整实现中，这里会显示实际的磁盘使用情况
    syscall_print_string("Filesystem  Size  Used  Available  Use%\n");
    syscall_print_string("----------  ----  ----  ---------  ----\n");
    syscall_print_string("/dev/sda1   100M  45M   55M        45%\n");
    syscall_print_string("/dev/sda2   512M  210M  302M       41%\n");
}

// 显示网络统计
void show_network_stats() {
    syscall_print_string("=== Network Statistics ===\n");
    syscall_print_string("Note: Network stats would be shown in a full implementation\n");
    
    // 在完整实现中，这里会显示实际的网络统计
    syscall_print_string("Interface  RX Bytes  TX Bytes  Packets  Errors\n");
    syscall_print_string("---------  --------  --------  -------  ------\n");
    syscall_print_string("eth0       123456    789012    1234     0\n");
    syscall_print_string("lo         0         0         0        0\n");
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