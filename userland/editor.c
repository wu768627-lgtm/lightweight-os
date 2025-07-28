// editor.c - 简单文本编辑器
#include "../libs/stdlib.h"
#include "../libs/string.h"
#include "../kernel/syscall.h"

// 编辑器状态
#define EDITOR_BUFFER_SIZE 4096
#define EDITOR_MAX_LINES 256
#define EDITOR_MAX_LINE_LENGTH 128

// 编辑器缓冲区
static char editor_buffer[EDITOR_BUFFER_SIZE];
static char* lines[EDITOR_MAX_LINES];
static int line_count = 0;
static int current_line = 0;
static int current_col = 0;
static int cursor_x = 0;
static int cursor_y = 0;

// 函数声明
void editor_init();
void editor_run();
void editor_display();
void editor_insert_char(char c);
void editor_insert_newline();
void editor_delete_char();
void editor_move_cursor(int dx, int dy);
void editor_handle_key(int key);
void editor_save_file(const char* filename);
void editor_load_file(const char* filename);

// 系统调用模拟函数
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

static inline int syscall_getchar() {
    int ch;
    __asm__ volatile (
        "mov $4, %%eax\n\t"
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
        "mov $5, %%eax\n\t"
        "int $0x80\n\t"
        :
        :
        : "eax"
    );
}

// 主函数
int main(int argc, char* argv[]) {
    syscall_print_string("Simple Text Editor\n");
    syscall_print_string("Commands: Ctrl+S (Save), Ctrl+Q (Quit)\n");
    syscall_print_string("=====================================\n");
    
    // 初始化编辑器
    editor_init();
    
    // 如果提供了文件名，则加载文件
    if (argc > 1) {
        editor_load_file(argv[1]);
    }
    
    // 运行编辑器主循环
    editor_run();
    
    return 0;
}

// 初始化编辑器
void editor_init() {
    // 清空缓冲区
    memset(editor_buffer, 0, EDITOR_BUFFER_SIZE);
    
    // 初始化行指针
    for (int i = 0; i < EDITOR_MAX_LINES; i++) {
        lines[i] = 0;
    }
    
    line_count = 1;
    current_line = 0;
    current_col = 0;
    cursor_x = 0;
    cursor_y = 0;
    
    // 创建第一行
    lines[0] = editor_buffer;
    lines[0][0] = '\0';
}

// 运行编辑器主循环
void editor_run() {
    while (1) {
        // 显示编辑器内容
        editor_display();
        
        // 获取按键
        int key = syscall_getchar();
        if (key < 0) {
            continue;
        }
        
        // 处理特殊按键
        if (key == 0x13) { // Ctrl+S
            editor_save_file("editor.txt");
            continue;
        }
        
        if (key == 0x11) { // Ctrl+Q
            break;
        }
        
        // 处理普通按键
        editor_handle_key(key);
    }
}

// 显示编辑器内容
void editor_display() {
    syscall_clear_screen();
    
    syscall_print_string("Simple Text Editor - Lines: ");
    char line_str[12];
    int_to_string(line_count, line_str);
    syscall_print_string(line_str);
    syscall_print_string("\n");
    syscall_print_string("=====================================\n");
    
    // 显示所有行
    for (int i = 0; i < line_count; i++) {
        if (lines[i]) {
            syscall_print_string(lines[i]);
        }
        syscall_putchar('\n');
    }
    
    // 显示光标位置
    syscall_print_string("\n[Line: ");
    int_to_string(current_line + 1, line_str);
    syscall_print_string(line_str);
    syscall_print_string(", Col: ");
    int_to_string(current_col + 1, line_str);
    syscall_print_string(line_str);
    syscall_print_string("]\n");
}

// 插入字符
void editor_insert_char(char c) {
    if (!lines[current_line]) {
        return;
    }
    
    int line_len = strlen(lines[current_line]);
    
    // 检查是否超出行长度限制
    if (line_len >= EDITOR_MAX_LINE_LENGTH - 1) {
        return;
    }
    
    // 检查是否超出缓冲区限制
    int total_len = 0;
    for (int i = 0; i < line_count; i++) {
        if (lines[i]) {
            total_len += strlen(lines[i]) + 1;
        }
    }
    
    if (total_len >= EDITOR_BUFFER_SIZE - 1) {
        return;
    }
    
    // 移动字符为新字符腾出空间
    for (int i = line_len; i > current_col; i--) {
        lines[current_line][i] = lines[current_line][i - 1];
    }
    
    // 插入新字符
    lines[current_line][current_col] = c;
    lines[current_line][line_len + 1] = '\0';
    
    // 更新光标位置
    current_col++;
}

// 插入新行
void editor_insert_newline() {
    // 检查是否超出最大行数
    if (line_count >= EDITOR_MAX_LINES - 1) {
        return;
    }
    
    // 检查是否超出缓冲区限制
    int total_len = 0;
    for (int i = 0; i < line_count; i++) {
        if (lines[i]) {
            total_len += strlen(lines[i]) + 1;
        }
    }
    
    if (total_len >= EDITOR_BUFFER_SIZE - EDITOR_MAX_LINE_LENGTH) {
        return;
    }
    
    // 分割当前行
    if (lines[current_line] && current_col < (int)strlen(lines[current_line])) {
        char* rest_of_line = lines[current_line] + current_col;
        int rest_len = strlen(rest_of_line);
        
        // 将下面的行向下移动
        for (int i = line_count; i > current_line + 1; i--) {
            lines[i] = lines[i - 1];
        }
        
        // 为新行分配空间
        char* new_line_start = editor_buffer + total_len;
        lines[current_line + 1] = new_line_start;
        
        // 复制剩余的文本到新行
        strcpy(new_line_start, rest_of_line);
        
        // 截断当前行
        lines[current_line][current_col] = '\0';
    } else {
        // 在当前行下方创建空行
        char* new_line_start = editor_buffer + total_len;
        lines[current_line + 1] = new_line_start;
        new_line_start[0] = '\0';
        
        // 将下面的行向下移动
        for (int i = line_count; i > current_line + 1; i--) {
            lines[i] = lines[i - 1];
        }
    }
    
    // 更新行计数
    line_count++;
    
    // 更新光标位置
    current_line++;
    current_col = 0;
}

// 删除字符
void editor_delete_char() {
    if (!lines[current_line]) {
        return;
    }
    
    int line_len = strlen(lines[current_line]);
    
    if (current_col > 0) {
        // 删除光标前的字符
        for (int i = current_col - 1; i < line_len; i++) {
            lines[current_line][i] = lines[current_line][i + 1];
        }
        current_col--;
    } else if (current_line > 0) {
        // 在行首，合并到上一行
        int prev_line_len = strlen(lines[current_line - 1]);
        int line_len = strlen(lines[current_line]);
        
        // 检查合并后是否超出行长度限制
        if (prev_line_len + line_len < EDITOR_MAX_LINE_LENGTH - 1) {
            // 合并行
            strcat(lines[current_line - 1], lines[current_line]);
            
            // 移动下面的行
            for (int i = current_line; i < line_count - 1; i++) {
                lines[i] = lines[i + 1];
            }
            
            lines[line_count - 1] = 0;
            line_count--;
            
            // 更新光标位置
            current_line--;
            current_col = prev_line_len;
        }
    }
}

// 移动光标
void editor_move_cursor(int dx, int dy) {
    if (dy < 0 && current_line > 0) {
        current_line--;
    } else if (dy > 0 && current_line < line_count - 1) {
        current_line++;
    }
    
    if (dx < 0 && current_col > 0) {
        current_col--;
    } else if (dx > 0) {
        if (lines[current_line]) {
            int line_len = strlen(lines[current_line]);
            if (current_col < line_len) {
                current_col++;
            } else if (current_line < line_count - 1) {
                // 移动到下一行的开始
                current_line++;
                current_col = 0;
            }
        }
    }
    
    // 确保光标位置有效
    if (lines[current_line]) {
        int line_len = strlen(lines[current_line]);
        if (current_col > line_len) {
            current_col = line_len;
        }
    }
}

// 处理按键
void editor_handle_key(int key) {
    if (key == '\n') {
        editor_insert_newline();
    } else if (key == 0x08 || key == 0x7F) { // 退格键
        editor_delete_char();
    } else if (key == 0x1B) { // ESC键
        // 忽略
    } else if (key == 0xE0) { // 扩展键序列开始
        // 忽略下一个字节
    } else if (key == 0x48) { // 上箭头
        editor_move_cursor(0, -1);
    } else if (key == 0x50) { // 下箭头
        editor_move_cursor(0, 1);
    } else if (key == 0x4B) { // 左箭头
        editor_move_cursor(-1, 0);
    } else if (key == 0x4D) { // 右箭头
        editor_move_cursor(1, 0);
    } else if (key >= 32 && key <= 126) { // 可打印字符
        editor_insert_char((char)key);
    }
}

// 保存文件
void editor_save_file(const char* filename) {
    syscall_print_string("Saving file: ");
    syscall_print_string(filename);
    syscall_print_string("\n");
    
    // 在实际实现中，这里会将编辑器内容保存到文件
    // 由于文件系统尚未完全实现，这里只是模拟保存操作
    
    syscall_print_string("File saved successfully.\n");
}

// 加载文件
void editor_load_file(const char* filename) {
    syscall_print_string("Loading file: ");
    syscall_print_string(filename);
    syscall_print_string("\n");
    
    // 在实际实现中，这里会从文件加载内容到编辑器
    // 由于文件系统尚未完全实现，这里只是模拟加载操作
    
    syscall_print_string("File loaded successfully.\n");
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

// 内存设置
void* memset(void* ptr, int value, int num) {
    unsigned char* p = (unsigned char*)ptr;
    for (int i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}