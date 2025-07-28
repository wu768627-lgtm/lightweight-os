#include "exception.h"
#include "kernel.h"
#include "logger.h"
#include "process.h"
#include "profiling.h"

// 异常处理表
static exception_handler_t exception_handlers[32];

// 异常统计信息
static struct exception_stats exc_stats;

// 初始化异常处理系统
void exception_init() {
    // 初始化异常处理表
    for (int i = 0; i < 32; i++) {
        exception_handlers[i] = 0;
    }
    
    // 注册默认异常处理程序
    exception_register_handler(0, divide_by_zero_handler);
    exception_register_handler(1, debug_handler);
    exception_register_handler(2, non_maskable_interrupt_handler);
    exception_register_handler(3, breakpoint_handler);
    exception_register_handler(4, overflow_handler);
    exception_register_handler(5, bound_range_handler);
    exception_register_handler(6, invalid_opcode_handler);
    exception_register_handler(7, device_not_available_handler);
    exception_register_handler(8, double_fault_handler);
    exception_register_handler(10, invalid_tss_handler);
    exception_register_handler(11, segment_not_present_handler);
    exception_register_handler(12, stack_segment_fault_handler);
    exception_register_handler(13, general_protection_fault_handler);
    exception_register_handler(14, page_fault_handler);
    exception_register_handler(16, x87_floating_point_handler);
    exception_register_handler(17, alignment_check_handler);
    exception_register_handler(18, machine_check_handler);
    exception_register_handler(19, simd_floating_point_handler);
    
    // 初始化统计信息
    exc_stats.total_exceptions = 0;
    exc_stats.divide_by_zero = 0;
    exc_stats.debug = 0;
    exc_stats.non_maskable_interrupt = 0;
    exc_stats.breakpoint = 0;
    exc_stats.overflow = 0;
    exc_stats.bound_range = 0;
    exc_stats.invalid_opcode = 0;
    exc_stats.device_not_available = 0;
    exc_stats.double_fault = 0;
    exc_stats.invalid_tss = 0;
    exc_stats.segment_not_present = 0;
    exc_stats.stack_segment_fault = 0;
    exc_stats.general_protection_fault = 0;
    exc_stats.page_fault = 0;
    exc_stats.x87_floating_point = 0;
    exc_stats.alignment_check = 0;
    exc_stats.machine_check = 0;
    exc_stats.simd_floating_point = 0;
    
    LOG_INFO("EXCEPTION", "Exception handling system initialized");
    print_string("Exception handling system initialized\n");
}

// 注册异常处理程序
int exception_register_handler(int exception_num, exception_handler_t handler) {
    if (exception_num < 0 || exception_num >= 32) {
        LOG_ERROR("EXCEPTION", "Invalid exception number");
        return -1;
    }
    
    exception_handlers[exception_num] = handler;
    LOG_INFO("EXCEPTION", "Exception handler registered");
    return 0;
}

// 处理异常
void exception_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    // 更新统计信息
    exc_stats.total_exceptions++;
    
    // 记录日志
    LOG_CRITICAL("EXCEPTION", "CPU Exception occurred");
    
    // 显示异常信息
    print_string("CPU Exception #");
    char num_str[12];
    int_to_string(exception_num, num_str);
    print_string(num_str);
    print_string(" at 0x");
    hex_to_string(eip, num_str);
    print_string(num_str);
    print_string("\n");
    
    // 根据异常类型更新特定统计信息
    switch (exception_num) {
        case 0: exc_stats.divide_by_zero++; break;
        case 1: exc_stats.debug++; break;
        case 2: exc_stats.non_maskable_interrupt++; break;
        case 3: exc_stats.breakpoint++; break;
        case 4: exc_stats.overflow++; break;
        case 5: exc_stats.bound_range++; break;
        case 6: exc_stats.invalid_opcode++; break;
        case 7: exc_stats.device_not_available++; break;
        case 8: exc_stats.double_fault++; break;
        case 10: exc_stats.invalid_tss++; break;
        case 11: exc_stats.segment_not_present++; break;
        case 12: exc_stats.stack_segment_fault++; break;
        case 13: exc_stats.general_protection_fault++; break;
        case 14: exc_stats.page_fault++; break;
        case 16: exc_stats.x87_floating_point++; break;
        case 17: exc_stats.alignment_check++; break;
        case 18: exc_stats.machine_check++; break;
        case 19: exc_stats.simd_floating_point++; break;
    }
    
    // 调用特定异常处理程序
    if (exception_handlers[exception_num]) {
        exception_handlers[exception_num](exception_num, error_code, eip);
    } else {
        // 使用默认处理程序
        default_exception_handler(exception_num, error_code, eip);
    }
}

// 默认异常处理程序
void default_exception_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Unhandled exception #");
    char num_str[12];
    int_to_string(exception_num, num_str);
    print_string(num_str);
    print_string("\n");
    
    // 显示错误代码（如果有的话）
    if (exception_num == 8 || (exception_num >= 10 && exception_num <= 14)) {
        print_string("Error code: 0x");
        hex_to_string(error_code, num_str);
        print_string(num_str);
        print_string("\n");
    }
    
    // 显示当前进程信息
    print_string("Current process: ");
    // 在实际实现中，这里会显示当前进程信息
    
    // 记录致命错误
    LOG_CRITICAL("EXCEPTION", "System halted due to unhandled exception");
    
    // 终止系统
    print_string("System halted.\n");
    while (1) {
        __asm__ volatile ("hlt");
    }
}

// 获取异常统计信息
struct exception_stats* exception_get_stats() {
    return &exc_stats;
}

// 显示异常统计信息
void exception_print_stats() {
    print_string("=== Exception Statistics ===\n");
    
    char stat_str[16];
    
    print_string("Total exceptions: ");
    int_to_string(exc_stats.total_exceptions, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Divide by zero: ");
    int_to_string(exc_stats.divide_by_zero, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Debug exceptions: ");
    int_to_string(exc_stats.debug, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Page faults: ");
    int_to_string(exc_stats.page_fault, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("General protection faults: ");
    int_to_string(exc_stats.general_protection_fault, stat_str);
    print_string(stat_str);
    print_string("\n");
}

// 各种异常处理程序
void divide_by_zero_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Divide by zero exception at 0x");
    char addr_str[16];
    hex_to_string(eip, addr_str);
    print_string(addr_str);
    print_string("\n");
    
    // 在实际实现中，这里可能会终止导致异常的进程
    // 而不是终止整个系统
    LOG_ERROR("EXCEPTION", "Divide by zero error");
}

void debug_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Debug exception\n");
    LOG_INFO("EXCEPTION", "Debug exception occurred");
}

void non_maskable_interrupt_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Non-maskable interrupt\n");
    LOG_WARNING("EXCEPTION", "NMI occurred");
}

void breakpoint_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Breakpoint exception\n");
    LOG_INFO("EXCEPTION", "Breakpoint hit");
}

void overflow_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Overflow exception\n");
    LOG_ERROR("EXCEPTION", "Overflow error");
}

void bound_range_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Bound range exceeded exception\n");
    LOG_ERROR("EXCEPTION", "Bound range exceeded");
}

void invalid_opcode_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Invalid opcode exception at 0x");
    char addr_str[16];
    hex_to_string(eip, addr_str);
    print_string(addr_str);
    print_string("\n");
    LOG_ERROR("EXCEPTION", "Invalid opcode");
}

void device_not_available_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Device not available exception\n");
    LOG_WARNING("EXCEPTION", "Device not available");
}

void double_fault_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Double fault exception\n");
    LOG_CRITICAL("EXCEPTION", "Double fault occurred");
    
    // 双重错误通常是致命的
    print_string("System halted due to double fault.\n");
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void invalid_tss_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Invalid TSS exception\n");
    LOG_ERROR("EXCEPTION", "Invalid TSS");
}

void segment_not_present_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Segment not present exception\n");
    LOG_ERROR("EXCEPTION", "Segment not present");
}

void stack_segment_fault_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Stack segment fault exception\n");
    LOG_ERROR("EXCEPTION", "Stack segment fault");
}

void general_protection_fault_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("General protection fault at 0x");
    char addr_str[16];
    hex_to_string(eip, addr_str);
    print_string(addr_str);
    print_string("\n");
    
    print_string("Error code: 0x");
    hex_to_string(error_code, addr_str);
    print_string(addr_str);
    print_string("\n");
    
    LOG_ERROR("EXCEPTION", "General protection fault");
}

void page_fault_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    // 获取导致页面错误的地址
    unsigned int faulting_address;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(faulting_address));
    
    print_string("Page fault at 0x");
    char addr_str[16];
    hex_to_string(faulting_address, addr_str);
    print_string(addr_str);
    print_string("\n");
    
    print_string("Error code: 0x");
    hex_to_string(error_code, addr_str);
    print_string(addr_str);
    print_string("\n");
    
    // 分析错误代码
    if (error_code & 0x1) {
        print_string("Page protection violation\n");
    } else {
        print_string("Page not present\n");
    }
    
    if (error_code & 0x2) {
        print_string("Write operation\n");
    } else {
        print_string("Read operation\n");
    }
    
    if (error_code & 0x4) {
        print_string("User mode access\n");
    } else {
        print_string("Supervisor mode access\n");
    }
    
    LOG_ERROR("EXCEPTION", "Page fault occurred");
    
    // 在实际实现中，这里会处理页面错误
    // 可能包括分配物理页帧或从磁盘加载页面
}

void x87_floating_point_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("x87 floating point exception\n");
    LOG_ERROR("EXCEPTION", "x87 floating point error");
}

void alignment_check_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Alignment check exception\n");
    LOG_ERROR("EXCEPTION", "Alignment check failed");
}

void machine_check_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("Machine check exception\n");
    LOG_CRITICAL("EXCEPTION", "Machine check error");
    
    // 机器检查错误通常是致命的
    print_string("System halted due to machine check error.\n");
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void simd_floating_point_handler(int exception_num, unsigned int error_code, unsigned int eip) {
    print_string("SIMD floating point exception\n");
    LOG_ERROR("EXCEPTION", "SIMD floating point error");
}

// 整数转十六进制字符串
void hex_to_string(unsigned int value, char* str) {
    const char hex_chars[] = "0123456789ABCDEF";
    int pos = 0;
    
    // 处理特殊情况
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    // 转换为十六进制
    char temp[16];
    int i = 0;
    while (value > 0) {
        temp[i++] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    // 反转字符串
    for (int j = i - 1; j >= 0; j--) {
        str[pos++] = temp[j];
    }
    str[pos] = '\0';
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