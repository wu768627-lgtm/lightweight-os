#ifndef VM_H
#define VM_H

// 页大小
#define PAGE_SIZE 4096

// 物理内存大小 (假设有128MB物理内存)
#define PHYSICAL_MEMORY_SIZE (128 * 1024 * 1024)
#define TOTAL_PHYSICAL_PAGES (PHYSICAL_MEMORY_SIZE / PAGE_SIZE)

// 页帧位图大小 (以字节为单位)
#define PAGE_FRAME_BITMAP_SIZE (TOTAL_PHYSICAL_PAGES / 8)

// 最大页表数
#define MAX_PAGE_TABLES 1024

// 页目录项结构
typedef struct {
    unsigned int present        : 1;   // 页存在位
    unsigned int rw             : 1;   // 读写权限位
    unsigned int user           : 1;   // 用户/超级用户权限位
    unsigned int write_through  : 1;   // 写透标志
    unsigned int cache_disabled : 1;   // 缓存禁用标志
    unsigned int accessed       : 1;   // 访问标志
    unsigned int reserved       : 1;   // 保留位
    unsigned int page_size      : 1;   // 页大小标志
    unsigned int global         : 1;   // 全局页标志
    unsigned int available      : 3;   // 可用位
    unsigned int frame          : 20;  // 页帧地址(高20位)
} page_directory_entry_t;

// 页表项结构
typedef struct {
    unsigned int present        : 1;   // 页存在位
    unsigned int rw             : 1;   // 读写权限位
    unsigned int user           : 1;   // 用户/超级用户权限位
    unsigned int write_through  : 1;   // 写透标志
    unsigned int cache_disabled : 1;   // 缓存禁用标志
    unsigned int accessed       : 1;   // 访问标志
    unsigned int dirty          : 1;   // 脏页标志
    unsigned int reserved       : 1;   // 保留位
    unsigned int global         : 1;   // 全局页标志
    unsigned int available      : 3;   // 可用位
    unsigned int frame          : 20;  // 页帧地址(高20位)
} page_table_entry_t;

// 页目录结构
typedef struct {
    page_directory_entry_t entries[1024];
} page_directory_t;

// 页表结构
typedef struct {
    page_table_entry_t entries[1024];
} page_table_t;

// 虚拟内存统计结构
struct vm_stats {
    unsigned int total_pages;   // 总页数
    unsigned int used_pages;    // 已使用页数
    unsigned int free_pages;    // 空闲页数
    unsigned int page_faults;   // 页错误次数
};

// 函数声明
void vm_init();
unsigned int vm_allocate_frame();
void vm_free_frame(unsigned int frame);
page_table_t* vm_create_page_table();
int vm_map_page(page_directory_t* page_dir, unsigned int virtual_addr, unsigned int physical_addr, int user, int rw);
void vm_handle_page_fault(unsigned int faulting_address, unsigned int error_code);
struct vm_stats* vm_get_stats();
void vm_print_stats();

// 内部函数
void load_page_directory(page_directory_t* page_dir);
void enable_paging();
void flush_tlb_entry(unsigned int virtual_addr);

// 辅助函数
void hex_to_string(unsigned int value, char* str);
void int_to_string(int value, char* str);

#endif