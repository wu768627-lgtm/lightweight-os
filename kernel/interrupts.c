#include "kernel.h"
#include "interrupts.h"
#include "logger.h"
#include "../libs/stdlib.h"

// 中断处理程序数组
static interrupt_handler_t interrupt_handlers[256];

// 初始化中断处理
void initialize_interrupts() {
    // 初始化中断处理程序数组
    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = 0;
    }
    
    // 注册默认中断处理程序
    register_interrupt_handler(0, divide_by_zero_handler);
    register_interrupt_handler(1, debug_handler);
    register_interrupt_handler(2, non_maskable_interrupt_handler);
    register_interrupt_handler(3, breakpoint_handler);
    register_interrupt_handler(4, overflow_handler);
    register_interrupt_handler(5, bound_range_exceeded_handler);
    register_interrupt_handler(6, invalid_opcode_handler);
    register_interrupt_handler(7, device_not_available_handler);
    register_interrupt_handler(8, double_fault_handler);
    register_interrupt_handler(10, invalid_tss_handler);
    register_interrupt_handler(11, segment_not_present_handler);
    register_interrupt_handler(12, stack_segment_fault_handler);
    register_interrupt_handler(13, general_protection_fault_handler);
    register_interrupt_handler(14, page_fault_handler);
    register_interrupt_handler(16, x87_floating_point_exception_handler);
    register_interrupt_handler(17, alignment_check_handler);
    register_interrupt_handler(18, machine_check_handler);
    register_interrupt_handler(19, simd_floating_point_exception_handler);
    register_interrupt_handler(20, virtualization_exception_handler);
    register_interrupt_handler(30, security_exception_handler);
    
    LOG_INFO("INTERRUPTS", "Interrupt handling initialized");
}

// 注册中断处理程序
void register_interrupt_handler(unsigned char interrupt, interrupt_handler_t handler) {
    if (interrupt < 256) {
        interrupt_handlers[interrupt] = handler;
        LOG_DEBUG("INTERRUPTS", "Registered handler for interrupt %d", interrupt);
    }
}

// 通用中断处理程序
void interrupt_handler(struct interrupt_registers* regs) {
    // 获取中断号
    unsigned char interrupt_number = regs->int_no & 0xFF;
    
    // 调用特定中断处理程序
    if (interrupt_handlers[interrupt_number] != 0) {
        interrupt_handlers[interrupt_number](regs);
    } else {
        // 未处理的中断
        char buffer[16];
        int_to_string(interrupt_number, buffer);
        LOG_WARNING("INTERRUPTS", "Unhandled interrupt: %s", buffer);
    }
}

// 除零错误处理程序
void divide_by_zero_handler(struct interrupt_registers* regs) {
    LOG_ERROR("INTERRUPTS", "Divide by zero error occurred");
    LOG_ERROR("INTERRUPTS", "Error code: %d", regs->err_code);
    // 在实际系统中，这里应该终止导致错误的进程
}

// ISR处理函数实现（汇编）
// 注意：这些函数需要在单独的汇编文件中实现
// 为简洁起见，这里只提供注释说明
/*
isr0:  // 除零错误
isr1:  // 调试异常
isr2:  // 非屏蔽中断
isr3:  // 断点异常
isr4:  // 溢出
isr5:  // 越界
isr6:  // 无效操作码
isr7:  // 设备不可用
isr8:  // 双重错误
isr9:  // 协处理器段溢出
isr10: // 无效TSS
isr11: // 段不存在
isr12: // 栈段错误
isr13: // 一般保护错误
isr14: // 页面错误
isr15: // 未知中断
isr16: // x87浮点异常
isr17: // 对齐检查
isr18: // 机器检查
isr19: // SIMD浮点异常
isr20-31: // 保留
*/