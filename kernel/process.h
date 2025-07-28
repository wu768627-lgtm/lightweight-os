#ifndef PROCESS_H
#define PROCESS_H

// 进程状态
#define PROCESS_RUNNING 0
#define PROCESS_READY   1
#define PROCESS_WAITING 2
#define PROCESS_STOPPED 3

// 最大进程数
#define MAX_PROCESSES 64

// 进程控制块
struct process {
    unsigned int pid;           // 进程ID
    unsigned int state;         // 进程状态
    unsigned int priority;      // 进程优先级
    unsigned int stack_pointer; // 栈指针
    unsigned int program_counter; // 程序计数器
    unsigned int registers[8];  // 通用寄存器快照
};

// 函数声明
void initialize_processes();
void scheduler();
void create_process(void (*entry_point)());
void start_init_process();
int no_running_processes();
void switch_to_process(struct process* proc);

#endif