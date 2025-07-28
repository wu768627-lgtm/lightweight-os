#include "memory.h"
#include "logger.h"

// 内存块结构
struct memory_block {
    unsigned int size;
    int free;
    struct memory_block* next;
    // 添加用于跟踪的额外信息
    const char* allocated_file;
    unsigned int allocated_line;
    unsigned int magic; // 用于检测内存损坏
};

// 内存统计信息
struct memory_stats {
    unsigned int total_allocated;
    unsigned int total_freed;
    unsigned int current_usage;
    unsigned int peak_usage;
    unsigned int allocation_count;
    unsigned int free_count;
};

// 内存堆起始地址
static unsigned int heap_start = 0x100000; // 1MB
static unsigned int heap_end = 0x100000;
static struct memory_block* head = 0;

// 内存统计信息
static struct memory_stats mem_stats = {0, 0, 0, 0, 0, 0};

// 魔数用于检测内存损坏
#define MEMORY_BLOCK_MAGIC 0xDEADBEEF

// 初始化内存管理
void initialize_memory() {
    // 初始化堆
    head = (struct memory_block*)heap_start;
    head->size = 0;
    head->free = 1;
    head->next = 0;
    head->magic = MEMORY_BLOCK_MAGIC;
    
    // 重置统计信息
    mem_stats.total_allocated = 0;
    mem_stats.total_freed = 0;
    mem_stats.current_usage = 0;
    mem_stats.peak_usage = 0;
    mem_stats.allocation_count = 0;
    mem_stats.free_count = 0;
    
    LOG_INFO("MEMORY", "Memory management initialized");
}

// 分配内存
void* allocate_memory_debug(unsigned int size, const char* file, unsigned int line) {
    // 对齐到4字节边界
    if (size % 4 != 0) {
        size += (4 - (size % 4));
    }
    
    // 遍历空闲块链表
    struct memory_block* current = head;
    while (current) {
        // 检查魔数以检测内存损坏
        if (current->magic != MEMORY_BLOCK_MAGIC) {
            LOG_ERROR("MEMORY", "Memory corruption detected in block %x", (unsigned int)current);
            return 0;
        }
        
        if (current->free && current->size >= size) {
            // 找到合适的空闲块
            current->free = 0;
            current->allocated_file = file;
            current->allocated_line = line;
            
            // 更新统计信息
            mem_stats.total_allocated += size;
            mem_stats.current_usage += size;
            mem_stats.allocation_count++;
            
            if (mem_stats.current_usage > mem_stats.peak_usage) {
                mem_stats.peak_usage = mem_stats.current_usage;
            }
            
            // 如果剩余空间足够，分割内存块
            if (current->size > size + sizeof(struct memory_block)) {
                struct memory_block* new_block = (struct memory_block*)((unsigned int)current + sizeof(struct memory_block) + size);
                new_block->size = current->size - size - sizeof(struct memory_block);
                new_block->free = 1;
                new_block->next = current->next;
                new_block->magic = MEMORY_BLOCK_MAGIC;
                
                current->size = size;
                current->next = new_block;
            }
            
            // 返回内存块数据部分的指针
            return (void*)((unsigned int)current + sizeof(struct memory_block));
        }
        current = current->next;
    }
    
    // 没有找到合适的空闲块，扩展堆
    struct memory_block* new_block = (struct memory_block*)heap_end;
    new_block->size = size;
    new_block->free = 0;
    new_block->next = 0;
    new_block->allocated_file = file;
    new_block->allocated_line = line;
    new_block->magic = MEMORY_BLOCK_MAGIC;
    
    // 更新统计信息
    mem_stats.total_allocated += size;
    mem_stats.current_usage += size;
    mem_stats.allocation_count++;
    
    if (mem_stats.current_usage > mem_stats.peak_usage) {
        mem_stats.peak_usage = mem_stats.current_usage;
    }
    
    // 更新堆结束地址
    heap_end += sizeof(struct memory_block) + size;
    
    // 将新块添加到链表中
    if (head == 0) {
        head = new_block;
    } else {
        current = head;
        while (current->next) {
            // 检查魔数以检测内存损坏
            if (current->magic != MEMORY_BLOCK_MAGIC) {
                LOG_ERROR("MEMORY", "Memory corruption detected in block %x", (unsigned int)current);
                return 0;
            }
            current = current->next;
        }
        current->next = new_block;
    }
    
    // 返回内存块数据部分的指针
    return (void*)((unsigned int)new_block + sizeof(struct memory_block));
}

// 释放内存
void free_memory_debug(void* ptr, const char* file, unsigned int line) {
    // 检查空指针
    if (!ptr) {
        LOG_WARNING("MEMORY", "Attempt to free NULL pointer at %s:%d", file, line);
        return;
    }
    
    // 获取内存块结构指针
    struct memory_block* block = (struct memory_block*)((unsigned int)ptr - sizeof(struct memory_block));
    
    // 检查魔数以检测内存损坏
    if (block->magic != MEMORY_BLOCK_MAGIC) {
        LOG_ERROR("MEMORY", "Memory corruption detected when freeing block at %s:%d", file, line);
        return;
    }
    
    // 检查是否已经释放
    if (block->free) {
        LOG_WARNING("MEMORY", "Attempt to free already freed memory at %s:%d", file, line);
        LOG_WARNING("MEMORY", "Originally allocated at %s:%d", block->allocated_file, block->allocated_line);
        return;
    }
    
    // 更新统计信息
    mem_stats.total_freed += block->size;
    mem_stats.current_usage -= block->size;
    mem_stats.free_count++;
    
    // 标记为空闲
    block->free = 1;
    block->allocated_file = 0;
    block->allocated_line = 0;
    
    // 合并相邻的空闲块
    struct memory_block* current = head;
    while (current) {
        // 检查魔数以检测内存损坏
        if (current->magic != MEMORY_BLOCK_MAGIC) {
            LOG_ERROR("MEMORY", "Memory corruption detected in block %x", (unsigned int)current);
            return;
        }
        
        if (current->free && current->next && current->next->free) {
            // 合并当前块和下一个块
            current->size += sizeof(struct memory_block) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
    
    LOG_DEBUG("MEMORY", "Memory block freed at %s:%d", file, line);
}

// 重新分配内存
void* realloc_memory_debug(void* ptr, unsigned int size, const char* file, unsigned int line) {
    if (!ptr) {
        return allocate_memory_debug(size, file, line);
    }
    
    if (size == 0) {
        free_memory_debug(ptr, file, line);
        return 0;
    }
    
    // 获取当前块的大小
    struct memory_block* block = (struct memory_block*)((unsigned int)ptr - sizeof(struct memory_block));
    
    // 检查魔数以检测内存损坏
    if (block->magic != MEMORY_BLOCK_MAGIC) {
        LOG_ERROR("MEMORY", "Memory corruption detected in realloc at %s:%d", file, line);
        return 0;
    }
    
    unsigned int old_size = block->size;
    
    if (size <= old_size) {
        return ptr;
    }
    
    // 分配新内存
    void* new_ptr = allocate_memory_debug(size, file, line);
    if (!new_ptr) {
        return 0;
    }
    
    // 复制旧数据
    char* src = (char*)ptr;
    char* dst = (char*)new_ptr;
    for (unsigned int i = 0; i < old_size; i++) {
        dst[i] = src[i];
    }
    
    // 释放旧内存
    free_memory_debug(ptr, file, line);
    
    return new_ptr;
}

// 获取内存统计信息
void get_memory_stats(struct memory_stats* stats) {
    if (stats) {
        stats->total_allocated = mem_stats.total_allocated;
        stats->total_freed = mem_stats.total_freed;
        stats->current_usage = mem_stats.current_usage;
        stats->peak_usage = mem_stats.peak_usage;
        stats->allocation_count = mem_stats.allocation_count;
        stats->free_count = mem_stats.free_count;
    }
}

// 打印内存统计信息
void print_memory_stats() {
    LOG_INFO("MEMORY", "=== Memory Statistics ===");
    LOG_INFO("MEMORY", "Total allocated: %d bytes", mem_stats.total_allocated);
    LOG_INFO("MEMORY", "Total freed: %d bytes", mem_stats.total_freed);
    LOG_INFO("MEMORY", "Current usage: %d bytes", mem_stats.current_usage);
    LOG_INFO("MEMORY", "Peak usage: %d bytes", mem_stats.peak_usage);
    LOG_INFO("MEMORY", "Allocation count: %d", mem_stats.allocation_count);
    LOG_INFO("MEMORY", "Free count: %d", mem_stats.free_count);
    LOG_INFO("MEMORY", "========================");
}

// 检查内存完整性
int check_memory_integrity() {
    struct memory_block* current = head;
    int errors = 0;
    
    while (current) {
        if (current->magic != MEMORY_BLOCK_MAGIC) {
            LOG_ERROR("MEMORY", "Memory corruption detected in block %x", (unsigned int)current);
            errors++;
        }
        current = current->next;
    }
    
    if (errors == 0) {
        LOG_INFO("MEMORY", "Memory integrity check passed");
    } else {
        LOG_ERROR("MEMORY", "Memory integrity check failed with %d errors", errors);
    }
    
    return errors == 0 ? 0 : -1;
}