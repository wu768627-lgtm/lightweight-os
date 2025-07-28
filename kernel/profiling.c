#include "profiling.h"
#include "kernel.h"
#include "process.h"

// 系统性能统计
static struct system_stats g_stats;

// CPU频率（Hz）
static unsigned long long g_cpu_frequency = 0;

// 性能计数器数组
static struct perf_counter g_counters[MAX_PERF_COUNTERS];
static int g_counter_count = 0;

// 初始化性能分析子系统
void profiling_init() {
    // 初始化系统统计
    g_stats.total_ticks = 0;
    g_stats.idle_ticks = 0;
    g_stats.kernel_ticks = 0;
    g_stats.user_ticks = 0;
    g_stats.context_switches = 0;
    g_stats.interrupts_handled = 0;
    g_stats.syscalls_made = 0;
    g_stats.page_faults = 0;
    g_stats.memory_allocated = 0;
    g_stats.memory_free = 0;
    g_stats.network_packets_sent = 0;
    g_stats.network_packets_received = 0;
    g_stats.disk_reads = 0;
    g_stats.disk_writes = 0;
    
    // 初始化性能计数器
    g_counter_count = 0;
    for (int i = 0; i < MAX_PERF_COUNTERS; i++) {
        g_counters[i].count = 0;
        g_counters[i].start_time = 0;
        g_counters[i].enabled = 0;
        g_counters[i].name[0] = '\0';
    }
    
    // 估算CPU频率（在实际实现中应该通过硬件获取）
    g_cpu_frequency = 1000000000ULL; // 假设1GHz
    
    print_string("Performance profiling subsystem initialized.\n");
}

// 启动性能计数器
void profiling_start_counter(struct perf_counter* counter, const char* name) {
    if (!counter) return;
    
    counter->count = 0;
    counter->enabled = 1;
    counter->start_time = profiling_get_timestamp();
    if (name) {
        // 复制名称
        int i;
        for (i = 0; i < 31 && name[i]; i++) {
            counter->name[i] = name[i];
        }
        counter->name[i] = '\0';
    } else {
        counter->name[0] = '\0';
    }
}

// 停止性能计数器
void profiling_stop_counter(struct perf_counter* counter) {
    if (!counter || !counter->enabled) return;
    
    unsigned long long end_time = profiling_get_timestamp();
    counter->count = end_time - counter->start_time;
    counter->enabled = 0;
}

// 创建并启动新的性能计数器
struct perf_counter* profiling_create_counter(const char* name) {
    if (g_counter_count >= MAX_PERF_COUNTERS) {
        return 0; // 计数器数组已满
    }
    
    struct perf_counter* counter = &g_counters[g_counter_count++];
    profiling_start_counter(counter, name);
    return counter;
}

// 打印性能计数器结果
void profiling_print_counter(struct perf_counter* counter) {
    if (!counter) return;
    
    print_string("Performance counter '");
    print_string(counter->name);
    print_string("': ");
    
    char count_str[32];
    int_to_string(counter->count, count_str);
    print_string(count_str);
    print_string(" cycles\n");
}

// 记录系统统计信息
void profiling_record_system_stats() {
    // 在实际实现中，这里会更新系统统计信息
    g_stats.total_ticks++;
}

// 打印系统统计信息
void profiling_print_system_stats() {
    print_string("=== System Performance Statistics ===\n");
    
    char stat_str[32];
    
    print_string("Total ticks: ");
    int_to_string(g_stats.total_ticks, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Idle ticks: ");
    int_to_string(g_stats.idle_ticks, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Context switches: ");
    int_to_string(g_stats.context_switches, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Interrupts handled: ");
    int_to_string(g_stats.interrupts_handled, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("System calls: ");
    int_to_string(g_stats.syscalls_made, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Page faults: ");
    int_to_string(g_stats.page_faults, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Memory allocated: ");
    int_to_string(g_stats.memory_allocated, stat_str);
    print_string(stat_str);
    print_string(" bytes\n");
    
    print_string("Network packets sent: ");
    int_to_string(g_stats.network_packets_sent, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Network packets received: ");
    int_to_string(g_stats.network_packets_received, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Disk reads: ");
    int_to_string(g_stats.disk_reads, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Disk writes: ");
    int_to_string(g_stats.disk_writes, stat_str);
    print_string(stat_str);
    print_string("\n");
}

// 获取系统统计信息
struct system_stats* profiling_get_system_stats() {
    return &g_stats;
}

// 获取时间戳
unsigned long long profiling_get_timestamp() {
    // 在实际实现中，应该使用RDTSC指令或其他高精度计时器
    static unsigned long long fake_timestamp = 0;
    return fake_timestamp++;
}

// 获取CPU频率
unsigned long long profiling_get_cpu_frequency() {
    return g_cpu_frequency;
}

// 记录内存分配
void profiling_memory_alloc(unsigned int size) {
    g_stats.memory_allocated += size;
}

// 记录内存释放
void profiling_memory_free(unsigned int size) {
    g_stats.memory_free += size;
}

// 记录上下文切换
void profiling_context_switch() {
    g_stats.context_switches++;
}

// 记录调度器滴答
void profiling_scheduler_tick() {
    g_stats.total_ticks++;
}

// 记录中断发生
void profiling_interrupt_occurred() {
    g_stats.interrupts_handled++;
}

// 记录系统调用
void profiling_syscall_made() {
    g_stats.syscalls_made++;
}

// 记录页面错误
void profiling_page_fault() {
    g_stats.page_faults++;
}

// 记录网络数据包发送
void profiling_network_packet_sent() {
    g_stats.network_packets_sent++;
}

// 记录网络数据包接收
void profiling_network_packet_received() {
    g_stats.network_packets_received++;
}

// 记录磁盘读取
void profiling_disk_read() {
    g_stats.disk_reads++;
}

// 记录磁盘写入
void profiling_disk_write() {
    g_stats.disk_writes++;
}

// 性能优化建议
void profiling_print_optimization_suggestions() {
    print_string("=== Performance Optimization Suggestions ===\n");
    
    struct system_stats* stats = &g_stats;
    
    // 检查上下文切换频率
    if (stats->total_ticks > 0) {
        unsigned int ctx_switch_rate = (stats->context_switches * 1000) / stats->total_ticks;
        if (ctx_switch_rate > 100) { // 每tick超过100次上下文切换
            print_string("High context switch rate detected. Consider reducing number of processes or threads.\n");
        }
    }
    
    // 检查系统调用频率
    if (stats->total_ticks > 0) {
        unsigned int syscall_rate = (stats->syscalls_made * 1000) / stats->total_ticks;
        if (syscall_rate > 200) { // 每tick超过200次系统调用
            print_string("High system call rate detected. Consider optimizing system call usage.\n");
        }
    }
    
    // 检查页面错误频率
    if (stats->total_ticks > 0) {
        unsigned int page_fault_rate = (stats->page_faults * 1000) / stats->total_ticks;
        if (page_fault_rate > 50) { // 每tick超过50次页面错误
            print_string("High page fault rate detected. Consider optimizing memory usage patterns.\n");
        }
    }
    
    // 检查空闲时间
    if (stats->total_ticks > 0) {
        unsigned int idle_percentage = (stats->idle_ticks * 100) / stats->total_ticks;
        if (idle_percentage > 90) { // 空闲时间超过90%
            print_string("High idle time detected. System may be underutilized.\n");
        } else if (idle_percentage < 10) { // 空闲时间少于10%
            print_string("Low idle time detected. System may be overloaded.\n");
        }
    }
}

// 整数转字符串函数
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