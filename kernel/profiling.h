#ifndef PROFILING_H
#define PROFILING_H

// 最大性能计数器数
#define MAX_PERF_COUNTERS 64

// 性能计数器结构
struct perf_counter {
    unsigned long long count;
    unsigned long long start_time;
    unsigned int enabled;
    char name[32];
};

// 系统性能统计结构
struct system_stats {
    unsigned long long total_ticks;
    unsigned long long idle_ticks;
    unsigned long long kernel_ticks;
    unsigned long long user_ticks;
    unsigned long long context_switches;
    unsigned long long interrupts_handled;
    unsigned long long syscalls_made;
    unsigned long long page_faults;
    unsigned long long memory_allocated;
    unsigned long long memory_free;
    unsigned long long network_packets_sent;
    unsigned long long network_packets_received;
    unsigned long long disk_reads;
    unsigned long long disk_writes;
};

// 函数声明
void profiling_init();
void profiling_start_counter(struct perf_counter* counter, const char* name);
void profiling_stop_counter(struct perf_counter* counter);
struct perf_counter* profiling_create_counter(const char* name);
void profiling_print_counter(struct perf_counter* counter);
void profiling_record_system_stats();
void profiling_print_system_stats();
struct system_stats* profiling_get_system_stats();
unsigned long long profiling_get_timestamp();
unsigned long long profiling_get_cpu_frequency();

// 内存性能统计
void profiling_memory_alloc(unsigned int size);
void profiling_memory_free(unsigned int size);

// 调度器性能统计
void profiling_context_switch();
void profiling_scheduler_tick();

// 中断性能统计
void profiling_interrupt_occurred();

// 系统调用性能统计
void profiling_syscall_made();

// 页面错误统计
void profiling_page_fault();

// 网络性能统计
void profiling_network_packet_sent();
void profiling_network_packet_received();

// 磁盘I/O统计
void profiling_disk_read();
void profiling_disk_write();

// 性能优化
void profiling_print_optimization_suggestions();

// 辅助函数
void int_to_string(int value, char* str);

#endif