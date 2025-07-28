#ifndef KERNEL_H
#define KERNEL_H

// 函数声明
void kernel_main();
void initialize_screen();
void print_string(char *str);
void scroll_screen();

// 全局变量声明
extern int cursor_x;
extern int cursor_y;

// 内联汇编函数
static inline void cpu_idle() {
    __asm__ volatile ("hlt");
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// 整数转字符串函数
void int_to_string(int value, char* str);

#endif