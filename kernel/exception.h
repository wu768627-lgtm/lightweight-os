#ifndef EXCEPTION_H
#define EXCEPTION_H

// 异常处理函数指针类型
typedef void (*exception_handler_t)(int exception_num, unsigned int error_code, unsigned int eip);

// 异常统计结构
struct exception_stats {
    unsigned int total_exceptions;
    unsigned int divide_by_zero;
    unsigned int debug;
    unsigned int non_maskable_interrupt;
    unsigned int breakpoint;
    unsigned int overflow;
    unsigned int bound_range;
    unsigned int invalid_opcode;
    unsigned int device_not_available;
    unsigned int double_fault;
    unsigned int invalid_tss;
    unsigned int segment_not_present;
    unsigned int stack_segment_fault;
    unsigned int general_protection_fault;
    unsigned int page_fault;
    unsigned int x87_floating_point;
    unsigned int alignment_check;
    unsigned int machine_check;
    unsigned int simd_floating_point;
};

// 函数声明
void exception_init();
int exception_register_handler(int exception_num, exception_handler_t handler);
void exception_handler(int exception_num, unsigned int error_code, unsigned int eip);
void default_exception_handler(int exception_num, unsigned int error_code, unsigned int eip);
struct exception_stats* exception_get_stats();
void exception_print_stats();

// 各种异常处理程序
void divide_by_zero_handler(int exception_num, unsigned int error_code, unsigned int eip);
void debug_handler(int exception_num, unsigned int error_code, unsigned int eip);
void non_maskable_interrupt_handler(int exception_num, unsigned int error_code, unsigned int eip);
void breakpoint_handler(int exception_num, unsigned int error_code, unsigned int eip);
void overflow_handler(int exception_num, unsigned int error_code, unsigned int eip);
void bound_range_handler(int exception_num, unsigned int error_code, unsigned int eip);
void invalid_opcode_handler(int exception_num, unsigned int error_code, unsigned int eip);
void device_not_available_handler(int exception_num, unsigned int error_code, unsigned int eip);
void double_fault_handler(int exception_num, unsigned int error_code, unsigned int eip);
void invalid_tss_handler(int exception_num, unsigned int error_code, unsigned int eip);
void segment_not_present_handler(int exception_num, unsigned int error_code, unsigned int eip);
void stack_segment_fault_handler(int exception_num, unsigned int error_code, unsigned int eip);
void general_protection_fault_handler(int exception_num, unsigned int error_code, unsigned int eip);
void page_fault_handler(int exception_num, unsigned int error_code, unsigned int eip);
void x87_floating_point_handler(int exception_num, unsigned int error_code, unsigned int eip);
void alignment_check_handler(int exception_num, unsigned int error_code, unsigned int eip);
void machine_check_handler(int exception_num, unsigned int error_code, unsigned int eip);
void simd_floating_point_handler(int exception_num, unsigned int error_code, unsigned int eip);

// 辅助函数
void hex_to_string(unsigned int value, char* str);
void int_to_string(int value, char* str);

#endif