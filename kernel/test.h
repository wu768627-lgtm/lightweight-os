#ifndef TEST_H
#define TEST_H

// 测试结果定义
#define TEST_PASS  0
#define TEST_FAIL  1
#define TEST_SKIP  2

// 测试统计结构
struct test_stats {
    int passed;
    int failed;
    int skipped;
};

// 测试用例结构
struct test_case {
    const char* name;
    int (*function)();
};

// 测试函数指针类型
typedef int (*test_func_t)();

// 函数声明
void test_init();
void test_run_all();
int test_run_case(const char* name, test_func_t func);
void test_print_summary();

// 各个模块的测试函数
int test_memory_allocation();
int test_process_creation();
int test_virtual_memory();
int test_scheduler();
int test_logger();
int test_config();
int test_exception_handler();
int test_power_management();
int test_profiling();
int test_security();

// 辅助函数
void int_to_string(int value, char* str);

#endif