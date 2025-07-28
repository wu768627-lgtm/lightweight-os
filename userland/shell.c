// shell.c - LightweightOS 系统Shell
#include "../libs/stdlib.h"
#include "../libs/string.h"
#include "../kernel/syscall.h"

// 命令历史记录
#define COMMAND_HISTORY_SIZE 16
static char command_history[COMMAND_HISTORY_SIZE][128];
static int history_count = 0;
static int history_index = 0;

// 当前工作目录
static char current_directory[256] = "/";

// 命令结构
struct command {
    char name[32];
    void (*function)(int argc, char* argv[]);
    char description[128];
};

// 函数声明
void print_prompt();
void read_command(char* buffer, int buffer_size);
void parse_command(char* command, char* argv[], int* argc);
void execute_command(int argc, char* argv[]);
void add_to_history(const char* command);

// 内置命令函数
void cmd_help(int argc, char* argv[]);
void cmd_clear(int argc, char* argv[]);
void cmd_echo(int argc, char* argv[]);
void cmd_ls(int argc, char* argv[]);
void cmd_cd(int argc, char* argv[]);
void cmd_pwd(int argc, char* argv[]);
void cmd_ps(int argc, char* argv[]);
void cmd_kill(int argc, char* argv[]);
void cmd_cat(int argc, char* argv[]);
void cmd_mkdir(int argc, char* argv[]);
void cmd_rmdir(int argc, char* argv[]);
void cmd_rm(int argc, char* argv[]);
void cmd_sysmon(int argc, char* argv[]);
void cmd_exit(int argc, char* argv[]);

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

static inline int syscall_getchar() {
    int ch;
    __asm__ volatile (
        "mov $16, %%eax\n\t"
        "int $0x80\n\t"
        "mov %%eax, %0\n\t"
        : "=r"(ch)
        :
        : "eax"
    );
    return ch;
}

static inline void syscall_clear_screen() {
    __asm__ volatile (
        "mov $17, %%eax\n\t"
        "int $0x80\n\t"
        :
        :
        : "eax"
    );
}

// 命令列表
struct command commands[] = {
    {"help", cmd_help, "显示帮助信息"},
    {"clear", cmd_clear, "清屏"},
    {"echo", cmd_echo, "显示文本"},
    {"ls", cmd_ls, "列出目录内容"},
    {"cd", cmd_cd, "更改目录"},
    {"pwd", cmd_pwd, "显示当前目录"},
    {"ps", cmd_ps, "显示进程列表"},
    {"kill", cmd_kill, "终止进程"},
    {"cat", cmd_cat, "显示文件内容"},
    {"mkdir", cmd_mkdir, "创建目录"},
    {"rmdir", cmd_rmdir, "删除目录"},
    {"rm", cmd_rm, "删除文件"},
    {"sysmon", cmd_sysmon, "系统监控"},
    {"exit", cmd_exit, "退出shell"},
    {"", 0, ""} // 结束标记
};

// 主函数
int main(int argc, char* argv[]) {
    char command_buffer[256];
    
    syscall_print_string("LightweightOS Shell\n");
    syscall_print_string("Type 'help' for available commands.\n\n");
    
    while (1) {
        print_prompt();
        read_command(command_buffer, sizeof(command_buffer));
        
        if (strlen(command_buffer) > 0) {
            add_to_history(command_buffer);
            
            char* argv[32];
            int argc;
            parse_command(command_buffer, argv, &argc);
            
            if (argc > 0) {
                execute_command(argc, argv);
            }
        }
    }
    
    return 0;
}

// 打印提示符
void print_prompt() {
    syscall_print_string("[user@lightweightos ");
    syscall_print_string(current_directory);
    syscall_print_string("]$ ");
}

// 读取命令
void read_command(char* buffer, int buffer_size) {
    int pos = 0;
    int history_pos = history_count;
    
    while (1) {
        int ch = syscall_getchar();
        
        if (ch < 0) {
            continue;
        }
        
        // 处理特殊键
        if (ch == '\n') {
            // 回车键
            buffer[pos] = '\0';
            syscall_putchar('\n');
            break;
        } else if (ch == 0x08 || ch == 0x7F) {
            // 退格键
            if (pos > 0) {
                pos--;
                syscall_putchar(0x08);
                syscall_putchar(' ');
                syscall_putchar(0x08);
            }
        } else if (ch == 0x09) {
            // Tab键 - 简单补全
            syscall_putchar(' ');
            syscall_putchar(' ');
            buffer[pos++] = ' ';
            buffer[pos++] = ' ';
        } else if (ch >= 32 && ch <= 126) {
            // 可打印字符
            if (pos < buffer_size - 1) {
                buffer[pos++] = ch;
                syscall_putchar(ch);
            }
        }
    }
}

// 解析命令
void parse_command(char* command, char* argv[], int* argc) {
    *argc = 0;
    int i = 0;
    
    // 跳过前导空格
    while (command[i] == ' ') i++;
    
    // 解析参数
    while (command[i] != '\0' && *argc < 31) {
        argv[*argc] = &command[i];
        (*argc)++;
        
        // 找到参数结尾
        while (command[i] != ' ' && command[i] != '\0') i++;
        
        // 如果不是结束符，将空格替换为字符串结束符
        if (command[i] != '\0') {
            command[i] = '\0';
            i++;
        }
        
        // 跳过后续空格
        while (command[i] == ' ') i++;
    }
    
    argv[*argc] = 0;
}

// 执行命令
void execute_command(int argc, char* argv[]) {
    // 查找命令
    for (int i = 0; commands[i].function != 0; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].function(argc, argv);
            return;
        }
    }
    
    // 未找到命令
    syscall_print_string("Command not found: ");
    syscall_print_string(argv[0]);
    syscall_putchar('\n');
}

// 添加到历史记录
void add_to_history(const char* command) {
    if (history_count < COMMAND_HISTORY_SIZE) {
        strcpy(command_history[history_count], command);
        history_count++;
    } else {
        // 循环覆盖旧记录
        for (int i = 0; i < COMMAND_HISTORY_SIZE - 1; i++) {
            strcpy(command_history[i], command_history[i + 1]);
        }
        strcpy(command_history[COMMAND_HISTORY_SIZE - 1], command);
    }
    history_index = history_count;
}

// 内置命令实现
void cmd_help(int argc, char* argv[]) {
    syscall_print_string("Available commands:\n");
    for (int i = 0; commands[i].function != 0; i++) {
        syscall_print_string("  ");
        syscall_print_string(commands[i].name);
        syscall_print_string(" - ");
        syscall_print_string(commands[i].description);
        syscall_putchar('\n');
    }
}

void cmd_clear(int argc, char* argv[]) {
    syscall_clear_screen();
}

void cmd_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) syscall_putchar(' ');
        syscall_print_string(argv[i]);
    }
    syscall_putchar('\n');
}

void cmd_ls(int argc, char* argv[]) {
    const char* path = (argc > 1) ? argv[1] : current_directory;
    syscall_print_string("Contents of ");
    syscall_print_string((char*)path);
    syscall_print_string(":\n");
    
    // 模拟目录内容
    syscall_print_string(".\n");
    syscall_print_string("..\n");
    if (strcmp(path, "/") == 0) {
        syscall_print_string("bin\n");
        syscall_print_string("etc\n");
        syscall_print_string("home\n");
        syscall_print_string("usr\n");
        syscall_print_string("tmp\n");
    }
}

void cmd_cd(int argc, char* argv[]) {
    if (argc < 2) {
        strcpy(current_directory, "/");
    } else {
        if (strcmp(argv[1], "..") == 0) {
            // 向上一级目录
            if (strlen(current_directory) > 1) {
                int len = strlen(current_directory);
                // 找到最后一个 '/'
                for (int i = len - 2; i >= 0; i--) {
                    if (current_directory[i] == '/') {
                        current_directory[i + 1] = '\0';
                        break;
                    }
                }
            }
        } else if (argv[1][0] == '/') {
            // 绝对路径
            strcpy(current_directory, argv[1]);
        } else {
            // 相对路径
            if (strlen(current_directory) > 1) {
                strcat(current_directory, "/");
            }
            strcat(current_directory, argv[1]);
        }
    }
}

void cmd_pwd(int argc, char* argv[]) {
    syscall_print_string(current_directory);
    syscall_putchar('\n');
}

void cmd_ps(int argc, char* argv[]) {
    syscall_print_string("PID  STATE  PRI  NAME\n");
    syscall_print_string("---  -----  ---  ----\n");
    syscall_print_string("1    RUN    10   init\n");
    syscall_print_string("2    WAIT   5    shell\n");
}

void cmd_kill(int argc, char* argv[]) {
    if (argc < 2) {
        syscall_print_string("Usage: kill <pid>\n");
        return;
    }
    
    int pid = atoi(argv[1]);
    syscall_print_string("Terminating process ");
    syscall_print_string(argv[1]);
    syscall_print_string("\n");
}

void cmd_cat(int argc, char* argv[]) {
    if (argc < 2) {
        syscall_print_string("Usage: cat <filename>\n");
        return;
    }
    
    syscall_print_string("Contents of ");
    syscall_print_string(argv[1]);
    syscall_print_string(":\n");
    
    // 模拟文件内容
    if (strcmp(argv[1], "README") == 0 || strcmp(argv[1], "README.txt") == 0) {
        syscall_print_string("LightweightOS - A lightweight, high-performance operating system\n");
        syscall_print_string("==============================================================\n");
        syscall_print_string("\n");
        syscall_print_string("Features:\n");
        syscall_print_string("- Microkernel architecture\n");
        syscall_print_string("- Advanced security mechanisms\n");
        syscall_print_string("- High performance optimizations\n");
        syscall_print_string("- Small memory footprint\n");
    } else {
        syscall_print_string("File not found: ");
        syscall_print_string(argv[1]);
        syscall_putchar('\n');
    }
}

void cmd_mkdir(int argc, char* argv[]) {
    if (argc < 2) {
        syscall_print_string("Usage: mkdir <directory>\n");
        return;
    }
    
    syscall_print_string("Creating directory ");
    syscall_print_string(argv[1]);
    syscall_print_string("\n");
}

void cmd_rmdir(int argc, char* argv[]) {
    if (argc < 2) {
        syscall_print_string("Usage: rmdir <directory>\n");
        return;
    }
    
    syscall_print_string("Removing directory ");
    syscall_print_string(argv[1]);
    syscall_print_string("\n");
}

void cmd_rm(int argc, char* argv[]) {
    if (argc < 2) {
        syscall_print_string("Usage: rm <file>\n");
        return;
    }
    
    syscall_print_string("Removing file ");
    syscall_print_string(argv[1]);
    syscall_print_string("\n");
}

void cmd_sysmon(int argc, char* argv[]) {
    syscall_print_string("System Monitor:\n");
    syscall_print_string("CPU Usage: 15%\n");
    syscall_print_string("Memory: 64MB free of 128MB\n");
    syscall_print_string("Processes: 2 running\n");
}

void cmd_exit(int argc, char* argv[]) {
    syscall_print_string("Exiting shell...\n");
    syscall_exit(0);
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

// 字符串连接
char* strcat(char* dest, const char* src) {
    char* ret = dest;
    while (*dest)
        dest++;
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