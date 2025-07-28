// init.c - 用户空间初始化程序

// 简单的系统调用接口
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

static inline void syscall_print_string(const char* str) {
    while (*str) {
        syscall_putchar(*str);
        str++;
    }
}

static inline void syscall_exit(int status) {
    __asm__ volatile (
        "mov $3, %%eax\n\t"
        "mov %0, %%ebx\n\t"
        "int $0x80\n\t"
        :
        : "r"(status)
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

// 简单的字符串函数
int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void strcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i]) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// 简单的内存操作函数
void memset(void* ptr, int value, int num) {
    char* p = (char*)ptr;
    for (int i = 0; i < num; i++) {
        p[i] = value;
    }
}

// 模拟的服务程序
void network_service() {
    syscall_print_string("Network service started\n");
    // 网络服务主循环
    while (1) {
        // 处理网络事件
        // 在实际实现中，这里会有网络处理逻辑
        for (int i = 0; i < 1000000; i++) {
            __asm__ volatile ("nop");
        }
    }
}

void filesystem_service() {
    syscall_print_string("Filesystem service started\n");
    // 文件系统服务主循环
    while (1) {
        // 处理文件系统事件
        // 在实际实现中，这里会有文件系统处理逻辑
        for (int i = 0; i < 1000000; i++) {
            __asm__ volatile ("nop");
        }
    }
}

void logging_service() {
    syscall_print_string("Logging service started\n");
    // 日志服务主循环
    while (1) {
        // 处理日志事件
        // 在实际实现中，这里会有日志处理逻辑
        for (int i = 0; i < 1000000; i++) {
            __asm__ volatile ("nop");
        }
    }
}

// 主函数
void main() {
    syscall_print_string("LightweightOS Userland Init Process\n");
    syscall_print_string("===================================\n");
    syscall_print_string("System initialization complete.\n");
    syscall_print_string("Starting system services...\n\n");
    
    // 启动系统服务
    syscall_print_string("Starting network service...\n");
    syscall_create_process("networkd", (void*)network_service);
    
    syscall_print_string("Starting filesystem service...\n");
    syscall_create_process("filesystemd", (void*)filesystem_service);
    
    syscall_print_string("Starting logging service...\n");
    syscall_create_process("loggingd", (void*)logging_service);
    
    syscall_print_string("\nAll system services started.\n");
    syscall_print_string("Starting system shell...\n\n");
    
    // 启动shell
    // 在实际实现中，这里会启动shell进程
    // 简单的命令循环
    while (1) {
        syscall_print_string("LightweightOS> ");
        
        // 在实际系统中，这里会读取用户输入并处理命令
        // 为简化起见，我们直接执行一些测试操作
        
        syscall_print_string("System status: Running\n");
        syscall_print_string("Available memory: 3MB\n");
        syscall_print_string("Running processes: 4\n");
        syscall_print_string("\n");
        
        // 简单的延迟
        for (int i = 0; i < 10000000; i++) {
            __asm__ volatile ("nop");
        }
    }
    
    syscall_exit(0);
}