#include "kernel.h"
#include "process.h"

// 进程表
static struct process process_table[MAX_PROCESSES];
static int process_count = 0;
static int current_process = -1;

// 初始化进程管理
void initialize_processes() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].pid = 0;
        process_table[i].state = PROCESS_STOPPED;
    }
    
    process_count = 0;
    current_process = -1;
}

// 创建进程
void create_process(void (*entry_point)()) {
    if (process_count >= MAX_PROCESSES) {
        print_string("Error: Maximum process limit reached.\n");
        return;
    }
    
    struct process* proc = &process_table[process_count];
    proc->pid = process_count + 1;
    proc->state = PROCESS_READY;
    proc->priority = 1;
    proc->program_counter = (unsigned int)entry_point;
    
    // 为进程分配栈空间
    void* stack = allocate_memory(4096); // 4KB 栈空间
    if (stack != 0) {
        proc->stack_pointer = (unsigned int)stack + 4096;
    } else {
        proc->state = PROCESS_STOPPED;
        print_string("Error: Failed to allocate stack for process.\n");
        return;
    }
    
    process_count++;
}

// 启动初始进程
void start_init_process() {
    // 创建一个简单的初始化进程
    create_process(&init_process_entry);
}

// 初始化进程入口点
void init_process_entry() {
    print_string("Init process started.\n");
    
    // 这里应该是系统初始化代码
    // 例如：启动系统服务、挂载文件系统等
    
    while (1) {
        // 空闲循环
        __asm__ volatile ("hlt");
    }
}

// 调度器
void scheduler() {
    if (process_count == 0) return;
    
    // 寻找下一个就绪进程
    int next_process = -1;
    for (int i = 0; i < process_count; i++) {
        int index = (current_process + i + 1) % process_count;
        if (process_table[index].state == PROCESS_READY) {
            next_process = index;
            break;
        }
    }
    
    // 如果找到就绪进程，则切换到该进程
    if (next_process != -1 && next_process != current_process) {
        current_process = next_process;
        process_table[current_process].state = PROCESS_RUNNING;
        switch_to_process(&process_table[current_process]);
    }
}

// 切换到指定进程
void switch_to_process(struct process* proc) {
    // 这里应该保存当前进程的上下文并恢复目标进程的上下文
    // 简化实现，仅打印信息
    print_string("Switching to process ");
    // 简单的数字转字符串
    char pid_str[12];
    int_to_string(proc->pid, pid_str);
    print_string(pid_str);
    print_string("\n");
}

// 检查是否没有运行中的进程
int no_running_processes() {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].state == PROCESS_RUNNING || 
            process_table[i].state == PROCESS_READY) {
            return 0;
        }
    }
    return 1;
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