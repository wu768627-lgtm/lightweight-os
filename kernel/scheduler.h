#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

// 最大优先级级别数
#define MAX_PRIORITY_LEVELS 8

// 时间片量子（ticks）
#define TIME_SLICE_QUANTUM 10

// 进程队列结构
struct process_queue {
    struct process* head;
    struct process* tail;
    int count;
};

// 调度器统计结构
struct scheduler_stats {
    unsigned int total_context_switches;  // 总上下文切换次数
    unsigned int preemptive_switches;     // 抢占式切换次数
    unsigned int voluntary_switches;      // 自愿切换次数
    unsigned int process_created;         // 创建的进程数
    unsigned int process_terminated;      // 终止的进程数
};

// 函数声明
void scheduler_init();
void scheduler_add_to_ready(struct process* proc);
struct process* scheduler_remove_from_ready();
void scheduler_add_to_waiting(struct process* proc);
void scheduler_remove_from_waiting(struct process* proc);
void scheduler_add_to_terminated(struct process* proc);
struct process* scheduler_select_next();
void scheduler_sleep(struct process* proc, unsigned int ticks);
void scheduler_wake_waiting();
struct scheduler_stats* scheduler_get_stats();
void scheduler_print_stats();

// 辅助函数
unsigned int get_current_tick();
void int_to_string(int value, char* str);

#endif