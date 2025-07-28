#include "scheduler.h"
#include "kernel.h"
#include "process.h"
#include "profiling.h"

// 进程队列
static struct process_queue ready_queue[MAX_PRIORITY_LEVELS];
static struct process_queue waiting_queue;
static struct process_queue terminated_queue;

// 当前运行的进程
static struct process* current_process = 0;

// 调度器统计信息
static struct scheduler_stats sched_stats;

// 时间片计数器
static unsigned int time_slice_counter = 0;

// 初始化调度器
void scheduler_init() {
    // 初始化队列
    for (int i = 0; i < MAX_PRIORITY_LEVELS; i++) {
        ready_queue[i].head = 0;
        ready_queue[i].tail = 0;
        ready_queue[i].count = 0;
    }
    
    waiting_queue.head = 0;
    waiting_queue.tail = 0;
    waiting_queue.count = 0;
    
    terminated_queue.head = 0;
    terminated_queue.tail = 0;
    terminated_queue.count = 0;
    
    // 初始化统计信息
    sched_stats.total_context_switches = 0;
    sched_stats.preemptive_switches = 0;
    sched_stats.voluntary_switches = 0;
    sched_stats.process_created = 0;
    sched_stats.process_terminated = 0;
    
    current_process = 0;
    time_slice_counter = 0;
    
    print_string("Advanced scheduler initialized\n");
}

// 添加进程到就绪队列
void scheduler_add_to_ready(struct process* proc) {
    if (!proc) {
        return;
    }
    
    // 设置进程状态
    proc->state = PROCESS_READY;
    
    // 获取优先级队列
    int priority = proc->priority;
    if (priority >= MAX_PRIORITY_LEVELS) {
        priority = MAX_PRIORITY_LEVELS - 1;
    }
    
    // 添加到队列尾部
    if (ready_queue[priority].tail) {
        ready_queue[priority].tail->next = proc;
    } else {
        ready_queue[priority].head = proc;
    }
    
    proc->next = 0;
    ready_queue[priority].tail = proc;
    ready_queue[priority].count++;
    
    sched_stats.process_created++;
    
    print_string("Process ");
    char pid_str[12];
    int_to_string(proc->pid, pid_str);
    print_string(pid_str);
    print_string(" added to ready queue (priority ");
    int_to_string(priority, pid_str);
    print_string(pid_str);
    print_string(")\n");
}

// 从就绪队列移除进程
struct process* scheduler_remove_from_ready() {
    // 按优先级查找进程
    for (int i = 0; i < MAX_PRIORITY_LEVELS; i++) {
        if (ready_queue[i].head) {
            struct process* proc = ready_queue[i].head;
            ready_queue[i].head = proc->next;
            
            if (!ready_queue[i].head) {
                ready_queue[i].tail = 0;
            }
            
            ready_queue[i].count--;
            proc->next = 0;
            
            return proc;
        }
    }
    
    return 0; // 没有就绪进程
}

// 添加进程到等待队列
void scheduler_add_to_waiting(struct process* proc) {
    if (!proc) {
        return;
    }
    
    // 设置进程状态
    proc->state = PROCESS_WAITING;
    
    // 添加到等待队列尾部
    if (waiting_queue.tail) {
        waiting_queue.tail->next = proc;
    } else {
        waiting_queue.head = proc;
    }
    
    proc->next = 0;
    waiting_queue.tail = proc;
    waiting_queue.count++;
    
    print_string("Process ");
    char pid_str[12];
    int_to_string(proc->pid, pid_str);
    print_string(pid_str);
    print_string(" added to waiting queue\n");
}

// 从等待队列移除进程
void scheduler_remove_from_waiting(struct process* proc) {
    if (!proc) {
        return;
    }
    
    // 查找进程在等待队列中的位置
    struct process* prev = 0;
    struct process* current = waiting_queue.head;
    
    while (current && current != proc) {
        prev = current;
        current = current->next;
    }
    
    if (!current) {
        return; // 进程不在等待队列中
    }
    
    // 从队列中移除
    if (prev) {
        prev->next = proc->next;
    } else {
        waiting_queue.head = proc->next;
    }
    
    if (waiting_queue.tail == proc) {
        waiting_queue.tail = prev;
    }
    
    waiting_queue.count--;
    proc->next = 0;
    
    print_string("Process ");
    char pid_str[12];
    int_to_string(proc->pid, pid_str);
    print_string(pid_str);
    print_string(" removed from waiting queue\n");
}

// 添加进程到终止队列
void scheduler_add_to_terminated(struct process* proc) {
    if (!proc) {
        return;
    }
    
    // 设置进程状态
    proc->state = PROCESS_STOPPED;
    
    // 添加到终止队列尾部
    if (terminated_queue.tail) {
        terminated_queue.tail->next = proc;
    } else {
        terminated_queue.head = proc;
    }
    
    proc->next = 0;
    terminated_queue.tail = proc;
    terminated_queue.count++;
    
    sched_stats.process_terminated++;
    
    print_string("Process ");
    char pid_str[12];
    int_to_string(proc->pid, pid_str);
    print_string(pid_str);
    print_string(" added to terminated queue\n");
}

// 调度器主函数
struct process* scheduler_select_next() {
    // 记录上下文切换
    sched_stats.total_context_switches++;
    profiling_context_switch();
    
    // 如果当前进程仍在运行且时间片未用完，继续运行
    if (current_process && current_process->state == PROCESS_RUNNING) {
        time_slice_counter++;
        
        // 检查时间片是否用完
        if (time_slice_counter < TIME_SLICE_QUANTUM) {
            return current_process;
        }
        
        // 时间片用完，进行抢占式切换
        sched_stats.preemptive_switches++;
        time_slice_counter = 0;
        
        // 将当前进程放回就绪队列
        scheduler_add_to_ready(current_process);
    }
    
    // 选择下一个进程
    struct process* next_process = scheduler_remove_from_ready();
    
    if (next_process) {
        // 设置为运行状态
        next_process->state = PROCESS_RUNNING;
        current_process = next_process;
        time_slice_counter = 0;
        
        print_string("Switching to process ");
        char pid_str[12];
        int_to_string(next_process->pid, pid_str);
        print_string(pid_str);
        print_string("\n");
    } else {
        current_process = 0;
    }
    
    return current_process;
}

// 进程睡眠
void scheduler_sleep(struct process* proc, unsigned int ticks) {
    if (!proc) {
        return;
    }
    
    // 设置唤醒时间
    proc->wake_time = get_current_tick() + ticks;
    
    // 添加到等待队列
    scheduler_add_to_waiting(proc);
    
    print_string("Process ");
    char pid_str[12];
    int_to_string(proc->pid, pid_str);
    print_string(pid_str);
    print_string(" sleeping for ");
    int_to_string(ticks, pid_str);
    print_string(pid_str);
    print_string(" ticks\n");
}

// 唤醒等待的进程
void scheduler_wake_waiting() {
    struct process* proc = waiting_queue.head;
    struct process* prev = 0;
    
    while (proc) {
        // 检查是否应该唤醒
        if (get_current_tick() >= proc->wake_time) {
            struct process* next = proc->next;
            
            // 从等待队列移除
            if (prev) {
                prev->next = next;
            } else {
                waiting_queue.head = next;
            }
            
            if (waiting_queue.tail == proc) {
                waiting_queue.tail = prev;
            }
            
            waiting_queue.count--;
            
            // 添加到就绪队列
            scheduler_add_to_ready(proc);
            
            proc = next;
        } else {
            prev = proc;
            proc = proc->next;
        }
    }
}

// 获取调度器统计信息
struct scheduler_stats* scheduler_get_stats() {
    return &sched_stats;
}

// 显示调度器统计信息
void scheduler_print_stats() {
    print_string("=== Scheduler Statistics ===\n");
    
    char stat_str[16];
    
    print_string("Total context switches: ");
    int_to_string(sched_stats.total_context_switches, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Preemptive switches: ");
    int_to_string(sched_stats.preemptive_switches, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Voluntary switches: ");
    int_to_string(sched_stats.voluntary_switches, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Processes created: ");
    int_to_string(sched_stats.process_created, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Processes terminated: ");
    int_to_string(sched_stats.process_terminated, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    // 显示队列状态
    print_string("Ready queue counts: ");
    for (int i = 0; i < MAX_PRIORITY_LEVELS; i++) {
        if (i > 0) print_string(", ");
        int_to_string(ready_queue[i].count, stat_str);
        print_string(stat_str);
    }
    print_string("\n");
    
    print_string("Waiting queue count: ");
    int_to_string(waiting_queue.count, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Terminated queue count: ");
    int_to_string(terminated_queue.count, stat_str);
    print_string(stat_str);
    print_string("\n");
}

// 获取当前tick计数
unsigned int get_current_tick() {
    // 在实际实现中，这会从系统时钟获取当前tick
    static unsigned int tick = 0;
    return tick++;
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