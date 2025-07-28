#include "test.h"
#include "kernel.h"
#include "memory.h"
#include "process.h"
#include "vm.h"
#include "scheduler.h"
#include "logger.h"
#include "config.h"
#include "exception.h"
#include "power.h"
#include "profiling.h"
#include "security.h"

// 测试结果统计
static struct test_stats global_test_stats = {0, 0, 0};

// 测试用例数组
static struct test_case test_cases[] = {
    {"Memory Allocation Test", test_memory_allocation},
    {"Process Creation Test", test_process_creation},
    {"Virtual Memory Test", test_virtual_memory},
    {"Scheduler Test", test_scheduler},
    {"Logger Test", test_logger},
    {"Configuration Test", test_config},
    {"Exception Handler Test", test_exception_handler},
    {"Power Management Test", test_power_management},
    {"Performance Profiling Test", test_profiling},
    {"Security Test", test_security},
    {0, 0} // 终止标记
};

// 初始化测试框架
void test_init() {
    print_string("Initializing test framework...\n");
    
    // 重置测试统计
    global_test_stats.passed = 0;
    global_test_stats.failed = 0;
    global_test_stats.skipped = 0;
    
    LOG_INFO("TEST", "Test framework initialized");
}

// 运行所有测试
void test_run_all() {
    print_string("==========================================\n");
    print_string("Running all kernel tests...\n");
    print_string("==========================================\n");
    
    // 初始化测试框架
    test_init();
    
    // 运行每个测试用例
    for (int i = 0; test_cases[i].name != 0; i++) {
        test_run_case(test_cases[i].name, test_cases[i].function);
    }
    
    // 显示测试总结
    test_print_summary();
}

// 运行单个测试用例
int test_run_case(const char* name, test_func_t func) {
    print_string("Running test: ");
    print_string((char*)name);
    print_string("... ");
    
    int result = func();
    
    if (result == TEST_PASS) {
        print_string("PASS\n");
        global_test_stats.passed++;
    } else if (result == TEST_FAIL) {
        print_string("FAIL\n");
        global_test_stats.failed++;
    } else {
        print_string("SKIP\n");
        global_test_stats.skipped++;
    }
    
    return result;
}

// 打印测试总结
void test_print_summary() {
    print_string("==========================================\n");
    print_string("Test Summary:\n");
    print_string("==========================================\n");
    
    char buffer[16];
    
    print_string("Passed: ");
    int_to_string(global_test_stats.passed, buffer);
    print_string(buffer);
    print_string("\n");
    
    print_string("Failed: ");
    int_to_string(global_test_stats.failed, buffer);
    print_string(buffer);
    print_string("\n");
    
    print_string("Skipped: ");
    int_to_string(global_test_stats.skipped, buffer);
    print_string(buffer);
    print_string("\n");
    
    int total = global_test_stats.passed + global_test_stats.failed + global_test_stats.skipped;
    print_string("Total: ");
    int_to_string(total, buffer);
    print_string(buffer);
    print_string("\n");
    
    if (global_test_stats.failed == 0) {
        print_string("\nAll tests passed!\n");
    } else {
        print_string("\nSome tests failed.\n");
    }
}

// 测试内存分配功能
int test_memory_allocation() {
    // 测试内存初始化
    initialize_memory();
    
    // 测试分配内存
    void* ptr1 = allocate_memory(1024);
    if (!ptr1) {
        return TEST_FAIL;
    }
    
    void* ptr2 = allocate_memory(2048);
    if (!ptr2) {
        return TEST_FAIL;
    }
    
    // 测试释放内存
    free_memory(ptr1);
    free_memory(ptr2);
    
    // 测试边界情况
    void* ptr3 = allocate_memory(0);
    if (ptr3) {
        return TEST_FAIL; // 不应该分配0字节内存
    }
    
    return TEST_PASS;
}

// 测试进程创建功能
int test_process_creation() {
    // 初始化进程管理
    initialize_processes();
    
    // 测试创建进程
    struct process* proc = create_process("test_process", 0, 0);
    if (!proc) {
        return TEST_FAIL;
    }
    
    // 检查进程属性
    if (proc->state != PROCESS_READY) {
        return TEST_FAIL;
    }
    
    // 测试进程调度
    scheduler_add_to_ready(proc);
    
    return TEST_PASS;
}

// 测试虚拟内存功能
int test_virtual_memory() {
    // 初始化虚拟内存
    vm_init();
    
    // 测试页帧分配
    unsigned int frame = vm_allocate_frame();
    if (frame == 0) {
        return TEST_FAIL;
    }
    
    // 测试页帧释放
    vm_free_frame(frame);
    
    return TEST_PASS;
}

// 测试调度器功能
int test_scheduler() {
    // 初始化调度器
    scheduler_init();
    
    // 创建测试进程
    struct process* proc1 = create_process("test1", 0, 0);
    struct process* proc2 = create_process("test2", 0, 0);
    
    if (!proc1 || !proc2) {
        return TEST_FAIL;
    }
    
    // 添加到就绪队列
    scheduler_add_to_ready(proc1);
    scheduler_add_to_ready(proc2);
    
    // 测试选择下一个进程
    struct process* next = scheduler_select_next();
    if (!next) {
        return TEST_FAIL;
    }
    
    return TEST_PASS;
}

// 测试日志功能
int test_logger() {
    // 初始化日志系统
    logger_init();
    
    // 测试记录日志
    logger_log(LOG_LEVEL_INFO, "TEST", "This is a test log message");
    
    // 测试设置日志级别
    logger_set_level(LOG_LEVEL_DEBUG);
    
    // 获取统计信息
    struct log_stats* stats = logger_get_stats();
    if (!stats) {
        return TEST_FAIL;
    }
    
    return TEST_PASS;
}

// 测试配置管理功能
int test_config() {
    // 初始化配置系统
    config_init();
    
    // 获取配置
    struct system_config* config = config_get();
    if (!config) {
        return TEST_FAIL;
    }
    
    // 测试设置配置项
    int old_max_processes = config_get_max_processes();
    if (config_set_max_processes(32) < 0) {
        return TEST_FAIL;
    }
    
    // 恢复原值
    config_set_max_processes(old_max_processes);
    
    return TEST_PASS;
}

// 测试异常处理功能
int test_exception_handler() {
    // 初始化异常处理系统
    exception_init();
    
    // 测试注册异常处理程序
    if (exception_register_handler(0, divide_by_zero_handler) < 0) {
        return TEST_FAIL;
    }
    
    // 获取统计信息
    struct exception_stats* stats = exception_get_stats();
    if (!stats) {
        return TEST_FAIL;
    }
    
    return TEST_PASS;
}

// 测试电源管理功能
int test_power_management() {
    // 初始化电源管理
    power_init();
    
    // 测试获取电源状态
    int state = power_get_state();
    if (state < 0) {
        return TEST_FAIL;
    }
    
    // 测试启用/禁用电源管理
    power_enable_management();
    power_disable_management();
    power_enable_management();
    
    return TEST_PASS;
}

// 测试性能分析功能
int test_profiling() {
    // 初始化性能分析
    profiling_init();
    
    // 测试记录统计信息
    profiling_record_system_stats();
    
    // 获取统计信息
    struct system_stats* stats = profiling_get_system_stats();
    if (!stats) {
        return TEST_FAIL;
    }
    
    return TEST_PASS;
}

// 测试安全功能
int test_security() {
    // 初始化安全子系统
    security_init();
    
    // 获取当前安全上下文
    struct security_context* ctx = security_get_current_context();
    if (!ctx) {
        return TEST_FAIL;
    }
    
    // 测试权限检查
    if (!security_check_permission(ctx, PERMISSION_READ)) {
        return TEST_FAIL;
    }
    
    return TEST_PASS;
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