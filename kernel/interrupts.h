#ifndef INTERRUPTS_H
#define INTERRUPTS_H

// IDT条目结构
struct idt_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));

// IDT指针结构
struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

// 函数声明
void initialize_interrupts();
void handle_pending_interrupts();
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
void install_isr_handlers();

// ISR处理函数声明
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#endif