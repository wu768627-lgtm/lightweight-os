// stress.c - 系统压力测试工具
#include "../libs/stdlib.h"
#include "../libs/string.h"
#include "../kernel/syscall.h"
#include "../kernel/profiling.h"

// 压力测试配置
struct stress_config {
    int memory_test;
    int cpu_test;
    int disk_test;
    int network_test;
    int duration; // 测试持续时间（秒）
    int intensity; // 测试强度（1-10）
};

// 测试结果
struct stress_result {
    unsigned long long memory_ops;
    unsigned long long cpu_ops;
    unsigned long long disk_ops;
    unsigned long long network_ops;
    unsigned int max_memory;
    unsigned int max_cpu;
    unsigned int max_disk;
    unsigned int max_network;
};

// 函数声明
void print_help();
void run_stress_test(struct stress_config* config);
void run_memory_stress(int intensity);
void run_cpu_stress(int intensity);
void run_disk_stress(int intensity);
void run_network_stress(int intensity);
void print_results(struct stress_result* result);
int parse_arguments(int argc, char* argv[], struct stress_config* config);

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

static inline void* syscall_malloc(unsigned int size) {
    void* ptr;
    __asm__ volatile (
        "mov $9, %%eax\n\t"
        "mov %1, %%ebx\n\t"
        "int $0x80\n\t"
        "mov %%eax, %0\n\t"
        : "=r"(ptr)
        : "r"(size)
        : "eax", "ebx"
    );
    return ptr;
}

static inline void syscall_free(void* ptr) {
    __asm__ volatile (
        "mov $10, %%eax\n\t"
        "mov %0, %%ebx\n\t"
        "int $0x80\n\t"
        :
        : "r"(ptr)
        : "eax", "ebx"
    );
}

static inline int syscall_gettimeofday(struct timeval* tv) {
    int result;
    __asm__ volatile (
        "mov $38, %%eax\n\t"
        "mov %1, %%ebx\n\t"
        "mov $0, %%ecx\n\t"
        "int $0x80\n\t"
        "mov %%eax, %0\n\t"
        : "=r"(result)
        : "r"(tv)
        : "eax", "ebx", "ecx"
    );
    return result;
}

// 时间结构
struct timeval {
    unsigned int tv_sec;
    unsigned int tv_usec;
};

// 主函数
int main(int argc, char* argv[]) {
    struct stress_config config = {1, 1, 1, 1, 10, 5}; // 默认配置
    
    // 解析命令行参数
    if (parse_arguments(argc, argv, &config) < 0) {
        return 1;
    }
    
    // 运行压力测试
    run_stress_test(&config);
    
    return 0;
}

// 打印帮助信息
void print_help() {
    syscall_print_string("System Stress Test Tool - LightweightOS\n");
    syscall_print_string("Usage: stress [options]\n\n");
    syscall_print_string("Options:\n");
    syscall_print_string("  -h, --help          Show this help message\n");
    syscall_print_string("  -m, --memory        Enable memory stress test\n");
    syscall_print_string("  -c, --cpu           Enable CPU stress test\n");
    syscall_print_string("  -d, --disk          Enable disk stress test\n");
    syscall_print_string("  -n, --network       Enable network stress test\n");
    syscall_print_string("  -t, --time <sec>    Test duration in seconds (default: 10)\n");
    syscall_print_string("  -i, --intensity <n> Test intensity 1-10 (default: 5)\n");
    syscall_print_string("  --no-memory         Disable memory stress test\n");
    syscall_print_string("  --no-cpu            Disable CPU stress test\n");
    syscall_print_string("  --no-disk           Disable disk stress test\n");
    syscall_print_string("  --no-network        Disable network stress test\n");
}

// 解析命令行参数
int parse_arguments(int argc, char* argv[], struct stress_config* config) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return -1;
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--memory") == 0) {
            config->memory_test = 1;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cpu") == 0) {
            config->cpu_test = 1;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--disk") == 0) {
            config->disk_test = 1;
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--network") == 0) {
            config->network_test = 1;
        } else if (strcmp(argv[i], "--no-memory") == 0) {
            config->memory_test = 0;
        } else if (strcmp(argv[i], "--no-cpu") == 0) {
            config->cpu_test = 0;
        } else if (strcmp(argv[i], "--no-disk") == 0) {
            config->disk_test = 0;
        } else if (strcmp(argv[i], "--no-network") == 0) {
            config->network_test = 0;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) {
            if (i + 1 < argc) {
                config->duration = atoi(argv[++i]);
            } else {
                syscall_print_string("Missing argument for --time\n");
                return -1;
            }
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--intensity") == 0) {
            if (i + 1 < argc) {
                config->intensity = atoi(argv[++i]);
                if (config->intensity < 1) config->intensity = 1;
                if (config->intensity > 10) config->intensity = 10;
            } else {
                syscall_print_string("Missing argument for --intensity\n");
                return -1;
            }
        } else {
            syscall_print_string("Unknown option: ");
            syscall_print_string(argv[i]);
            syscall_putchar('\n');
            return -1;
        }
    }
    
    return 0;
}

// 运行压力测试
void run_stress_test(struct stress_config* config) {
    struct stress_result result = {0, 0, 0, 0, 0, 0, 0, 0};
    
    syscall_print_string("Starting system stress test...\n");
    char buffer[16];
    syscall_print_string("Duration: ");
    int_to_string(config->duration, buffer);
    syscall_print_string(buffer);
    syscall_print_string(" seconds\n");
    syscall_print_string("Intensity: ");
    int_to_string(config->intensity, buffer);
    syscall_print_string(buffer);
    syscall_putchar('\n');
    
    // 获取开始时间
    struct timeval start_time;
    syscall_gettimeofday(&start_time);
    
    // 运行各种压力测试
    if (config->memory_test) {
        syscall_print_string("Running memory stress test...\n");
        run_memory_stress(config->intensity);
    }
    
    if (config->cpu_test) {
        syscall_print_string("Running CPU stress test...\n");
        run_cpu_stress(config->intensity);
    }
    
    if (config->disk_test) {
        syscall_print_string("Running disk stress test...\n");
        run_disk_stress(config->intensity);
    }
    
    if (config->network_test) {
        syscall_print_string("Running network stress test...\n");
        run_network_stress(config->intensity);
    }
    
    // 获取结束时间
    struct timeval end_time;
    syscall_gettimeofday(&end_time);
    
    // 计算测试结果
    // (在实际实现中，这里会收集详细的性能数据)
    
    // 显示结果
    print_results(&result);
}

// 运行内存压力测试
void run_memory_stress(int intensity) {
    // 根据强度调整分配的内存量
    unsigned int alloc_size = 1024 * intensity; // 1KB 到 10KB
    
    // 进行多次内存分配和释放
    for (int i = 0; i < 100 * intensity; i++) {
        void* ptr = syscall_malloc(alloc_size);
        if (ptr) {
            // 使用分配的内存
            char* bytes = (char*)ptr;
            for (unsigned int j = 0; j < alloc_size; j++) {
                bytes[j] = (char)(j % 256);
            }
            
            syscall_free(ptr);
        }
    }
}

// 运行CPU压力测试
void run_cpu_stress(int intensity) {
    // 执行计算密集型操作
    volatile unsigned long long result = 0;
    
    for (int i = 0; i < 10000 * intensity; i++) {
        for (int j = 0; j < 1000; j++) {
            result += i * j;
        }
    }
}

// 运行磁盘压力测试
void run_disk_stress(int intensity) {
    // 在实际实现中，这里会执行磁盘读写操作
    // 由于我们没有文件系统完整实现，这里只是模拟
    
    for (int i = 0; i < 10 * intensity; i++) {
        // 模拟磁盘操作
        for (int j = 0; j < 1000000; j++) {
            __asm__ volatile ("nop");
        }
    }
}

// 运行网络压力测试
void run_network_stress(int intensity) {
    // 在实际实现中，这里会执行网络数据传输
    // 由于我们没有网络完整实现，这里只是模拟
    
    for (int i = 0; i < 10 * intensity; i++) {
        // 模拟网络操作
        for (int j = 0; j < 1000000; j++) {
            __asm__ volatile ("nop");
        }
    }
}

// 打印测试结果
void print_results(struct stress_result* result) {
    syscall_print_string("==========================================\n");
    syscall_print_string("Stress Test Results:\n");
    syscall_print_string("==========================================\n");
    
    char buffer[32];
    
    syscall_print_string("Memory operations: ");
    long_long_to_string(result->memory_ops, buffer);
    syscall_print_string(buffer);
    syscall_putchar('\n');
    
    syscall_print_string("CPU operations: ");
    long_long_to_string(result->cpu_ops, buffer);
    syscall_print_string(buffer);
    syscall_putchar('\n');
    
    syscall_print_string("Disk operations: ");
    long_long_to_string(result->disk_ops, buffer);
    syscall_print_string(buffer);
    syscall_putchar('\n');
    
    syscall_print_string("Network operations: ");
    long_long_to_string(result->network_ops, buffer);
    syscall_print_string(buffer);
    syscall_putchar('\n');
    
    syscall_print_string("\nTest completed.\n");
}

// 字符串比较
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// 字符串转整数
int atoi(const char* str) {
    if (!str) return 0;
    
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // 处理符号
    if (str[0] == '-') {
        sign = -1;
        i = 1;
    } else if (str[0] == '+') {
        i = 1;
    }
    
    // 转换数字
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return result * sign;
}

// 长整数转字符串
void long_long_to_string(unsigned long long value, char* str) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    char temp[32];
    int i = 0;
    
    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    int j = 0;
    for (int k = i - 1; k >= 0; k--) {
        str[j++] = temp[k];
    }
    str[j] = '\0';
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