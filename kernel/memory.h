#ifndef MEMORY_H
#define MEMORY_H

// 内存统计信息结构
struct memory_stats {
    unsigned int total_allocated;
    unsigned int total_freed;
    unsigned int current_usage;
    unsigned int peak_usage;
    unsigned int allocation_count;
    unsigned int free_count;
};

// 初始化内存管理
void initialize_memory();

// 分配内存（带调试信息）
void* allocate_memory_debug(unsigned int size, const char* file, unsigned int line);

// 释放内存（带调试信息）
void free_memory_debug(void* ptr, const char* file, unsigned int line);

// 重新分配内存（带调试信息）
void* realloc_memory_debug(void* ptr, unsigned int size, const char* file, unsigned int line);

// 获取内存统计信息
void get_memory_stats(struct memory_stats* stats);

// 打印内存统计信息
void print_memory_stats();

// 检查内存完整性
int check_memory_integrity();

// 为了方便使用，定义宏来自动传递文件和行号
#define allocate_memory(size) allocate_memory_debug(size, __FILE__, __LINE__)
#define free_memory(ptr) free_memory_debug(ptr, __FILE__, __LINE__)
#define realloc_memory(ptr, size) realloc_memory_debug(ptr, size, __FILE__, __LINE__)

#endif