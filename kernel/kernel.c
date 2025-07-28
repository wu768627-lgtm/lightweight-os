#include "kernel.h"
#include "memory.h"
#include "process.h"
#include "interrupts.h"
#include "syscall.h"
#include "profiling.h"
#include "security.h"
#include "vm.h"
#include "scheduler.h"
#include "logger.h"
#include "config.h"
#include "exception.h"
#include "power.h"
#include "test.h"
#include "../drivers/filesystem.h"
#include "../drivers/network.h"
#include "../drivers/device.h"
#include "../drivers/graphics.h"

// 内核入口点
void kernel_main() {
    // 初始化内核各模块
    initialize_screen();
    print_string("LightweightOS Kernel Loading...\n");
    
    // 初始化日志系统
    logger_init();
    LOG_INFO("KERNEL", "Logger subsystem initialized");
    
    // 初始化配置系统
    config_init();
    LOG_INFO("KERNEL", "Configuration system initialized");
    
    // 初始化性能分析子系统
    profiling_init();
    LOG_INFO("KERNEL", "Performance profiling initialized");
    
    // 初始化安全子系统
    security_init();
    LOG_INFO("KERNEL", "Security subsystem initialized");
    
    // 初始化异常处理系统
    exception_init();
    LOG_INFO("KERNEL", "Exception handling system initialized");
    
    // 初始化内存管理
    initialize_memory();
    LOG_INFO("KERNEL", "Memory management initialized");
    
    // 初始化虚拟内存管理
    vm_init();
    LOG_INFO("KERNEL", "Virtual memory management initialized");
    
    // 初始化中断处理
    initialize_interrupts();
    LOG_INFO("KERNEL", "Interrupt handling initialized");
    
    // 初始化设备管理
    device_init();
    LOG_INFO("KERNEL", "Device management initialized");
    
    // 初始化文件系统
    fs_init();
    LOG_INFO("KERNEL", "Filesystem initialized");
    
    // 初始化网络子系统
    net_init();
    LOG_INFO("KERNEL", "Network subsystem initialized");
    
    // 初始化图形子系统
    gfx_init();
    LOG_INFO("KERNEL", "Graphics subsystem initialized");
    
    // 初始化调度器
    scheduler_init();
    LOG_INFO("KERNEL", "Process scheduler initialized");
    
    // 初始化电源管理
    power_init();
    LOG_INFO("KERNEL", "Power management initialized");
    
    // 初始化进程管理
    initialize_processes();
    LOG_INFO("KERNEL", "Process management initialized");
    
    // 检查是否需要运行测试
    if (check_test_mode()) {
        // 运行内核测试
        test_run_all();
        
        // 测试完成后停止系统
        print_string("Kernel tests completed. Halting system.\n");
        while (1) {
            __asm__ volatile ("hlt");
        }
    }
    
    // 启动第一个用户进程
    start_init_process();
    LOG_INFO("KERNEL", "Init process started");
    
    // 启动调度器
    LOG_INFO("KERNEL", "Starting scheduler");
    scheduler_loop();
    
    // 内核主循环
    while (1) {
        // 处理待决中断
        handle_pending_interrupts();
        
        // 唤醒等待的进程
        scheduler_wake_waiting();
        
        // 如果没有任务运行，则进入空闲状态
        if (no_running_processes()) {
            power_cpu_idle();
            profiling_record_system_stats();
        }
    }
}

// 检查是否处于测试模式
int check_test_mode() {
    // 在实际实现中，这里会检查启动参数或硬件开关
    // 简化实现，始终返回0（非测试模式）
    return 0;
}

// 调度器循环
void scheduler_loop() {
    while (1) {
        struct process* next = scheduler_select_next();
        if (next) {
            switch_to_process(next);
        } else {
            // 没有就绪进程，进入空闲状态
            power_cpu_idle();
        }
    }
}

// 屏幕初始化
void initialize_screen() {
    // 设置 VGA 文本模式
    char *video_memory = (char *)0xb8000;
    for (int i = 0; i < 80 * 25 * 2; i++) {
        video_memory[i] = 0;
    }
    
    // 设置光标位置
    cursor_x = 0;
    cursor_y = 0;
}

// 打印字符串到屏幕
void print_string(char *str) {
    char *video_memory = (char *)0xb8000;
    
    while (*str) {
        if (*str == '\n') {
            cursor_x = 0;
            cursor_y++;
        } else {
            video_memory[(cursor_y * 80 + cursor_x) * 2] = *str;
            video_memory[(cursor_y * 80 + cursor_x) * 2 + 1] = 0x07; // 白色字符，黑色背景
            cursor_x++;
            
            if (cursor_x >= 80) {
                cursor_x = 0;
                cursor_y++;
            }
        }
        
        if (cursor_y >= 25) {
            // 滚动屏幕
            scroll_screen();
            cursor_y = 24;
        }
        
        str++;
    }
}

// 滚动屏幕内容
void scroll_screen() {
    char *video_memory = (char *)0xb8000;
    
    // 将所有行上移一行
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 80; x++) {
            video_memory[(y * 80 + x) * 2] = video_memory[((y + 1) * 80 + x) * 2];
            video_memory[(y * 80 + x) * 2 + 1] = video_memory[((y + 1) * 80 + x) * 2 + 1];
        }
    }
    
    // 清除最后一行
    for (int x = 0; x < 80; x++) {
        video_memory[(24 * 80 + x) * 2] = 0;
        video_memory[(24 * 80 + x) * 2 + 1] = 0x07;
    }
}

// 光标位置
int cursor_x = 0;
int cursor_y = 0;

// 系统调用处理函数实现
void handle_system_call(int syscall_num, unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    // 记录系统调用
    profiling_syscall_made();
    
    syscall_handler(syscall_num, arg1, arg2, arg3);
}

// 整数转字符串辅助函数
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