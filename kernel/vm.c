#include "vm.h"
#include "kernel.h"
#include "memory.h"
#include "process.h"

// 页目录和页表
static page_directory_t* kernel_page_directory = 0;
static page_table_t page_tables[MAX_PAGE_TABLES];

// 页帧位图
static unsigned char page_frame_bitmap[PAGE_FRAME_BITMAP_SIZE];

// 虚拟内存统计
static struct vm_stats vm_statistics;

// 初始化虚拟内存管理
void vm_init() {
    // 初始化页帧位图
    for (int i = 0; i < PAGE_FRAME_BITMAP_SIZE; i++) {
        page_frame_bitmap[i] = 0;
    }
    
    // 标记前4MB为已使用(内核空间)
    for (int i = 0; i < 1024; i++) {
        page_frame_bitmap[i / 8] |= (1 << (i % 8));
    }
    
    // 初始化虚拟内存统计
    vm_statistics.total_pages = TOTAL_PHYSICAL_PAGES;
    vm_statistics.used_pages = 1024; // 内核占用的页
    vm_statistics.free_pages = TOTAL_PHYSICAL_PAGES - 1024;
    vm_statistics.page_faults = 0;
    
    // 创建内核页目录
    kernel_page_directory = (page_directory_t*)allocate_memory(sizeof(page_directory_t));
    if (!kernel_page_directory) {
        print_string("Error: Failed to allocate kernel page directory\n");
        return;
    }
    
    // 初始化页目录项
    for (int i = 0; i < 1024; i++) {
        kernel_page_directory->entries[i].present = 0;
        kernel_page_directory->entries[i].rw = 1;
        kernel_page_directory->entries[i].user = 0;
        kernel_page_directory->entries[i].write_through = 0;
        kernel_page_directory->entries[i].cache_disabled = 0;
        kernel_page_directory->entries[i].accessed = 0;
        kernel_page_directory->entries[i].reserved = 0;
        kernel_page_directory->entries[i].page_size = 0;
        kernel_page_directory->entries[i].global = 0;
        kernel_page_directory->entries[i].available = 0;
        kernel_page_directory->entries[i].frame = 0;
    }
    
    // 设置内核页表(映射前4MB)
    for (int i = 0; i < 4; i++) {
        // 创建页表
        page_table_t* table = &page_tables[i];
        
        // 初始化页表项
        for (int j = 0; j < 1024; j++) {
            table->entries[j].present = 1;
            table->entries[j].rw = 1;
            table->entries[j].user = 0;
            table->entries[j].write_through = 0;
            table->entries[j].cache_disabled = 0;
            table->entries[j].accessed = 0;
            table->entries[j].dirty = 0;
            table->entries[j].reserved = 0;
            table->entries[j].global = 1;
            table->entries[j].available = 0;
            table->entries[j].frame = (i * 1024 * 4096 + j * 4096) >> 12;
        }
        
        // 设置页目录项
        kernel_page_directory->entries[i].present = 1;
        kernel_page_directory->entries[i].rw = 1;
        kernel_page_directory->entries[i].user = 0;
        kernel_page_directory->entries[i].frame = ((unsigned int)table) >> 12;
    }
    
    // 加载页目录到CR3寄存器
    load_page_directory(kernel_page_directory);
    
    // 启用分页
    enable_paging();
    
    print_string("Virtual memory management initialized\n");
    print_string("Total pages: ");
    char pages_str[16];
    int_to_string(vm_statistics.total_pages, pages_str);
    print_string(pages_str);
    print_string("\n");
}

// 分配页帧
unsigned int vm_allocate_frame() {
    // 查找空闲页帧
    for (int i = 0; i < PAGE_FRAME_BITMAP_SIZE; i++) {
        if (page_frame_bitmap[i] != 0xFF) {
            for (int j = 0; j < 8; j++) {
                if (!(page_frame_bitmap[i] & (1 << j))) {
                    // 标记为已使用
                    page_frame_bitmap[i] |= (1 << j);
                    
                    // 更新统计信息
                    vm_statistics.used_pages++;
                    vm_statistics.free_pages--;
                    
                    // 返回页帧号
                    return (i * 8 + j);
                }
            }
        }
    }
    
    // 没有空闲页帧
    return 0;
}

// 释放页帧
void vm_free_frame(unsigned int frame) {
    if (frame >= TOTAL_PHYSICAL_PAGES) {
        return;
    }
    
    // 标记为未使用
    page_frame_bitmap[frame / 8] &= ~(1 << (frame % 8));
    
    // 更新统计信息
    vm_statistics.used_pages--;
    vm_statistics.free_pages++;
}

// 创建页表
page_table_t* vm_create_page_table() {
    // 在实际实现中，这里会分配物理内存给页表
    static int table_index = 4; // 前4个页表已用于内核
    
    if (table_index >= MAX_PAGE_TABLES) {
        return 0; // 没有可用的页表
    }
    
    page_table_t* table = &page_tables[table_index++];
    
    // 初始化页表项
    for (int i = 0; i < 1024; i++) {
        table->entries[i].present = 0;
        table->entries[i].rw = 0;
        table->entries[i].user = 0;
        table->entries[i].write_through = 0;
        table->entries[i].cache_disabled = 0;
        table->entries[i].accessed = 0;
        table->entries[i].dirty = 0;
        table->entries[i].reserved = 0;
        table->entries[i].global = 0;
        table->entries[i].available = 0;
        table->entries[i].frame = 0;
    }
    
    return table;
}

// 映射虚拟地址到物理地址
int vm_map_page(page_directory_t* page_dir, unsigned int virtual_addr, unsigned int physical_addr, int user, int rw) {
    if (!page_dir) {
        return -1;
    }
    
    // 计算页目录索引和页表索引
    unsigned int page_dir_index = virtual_addr >> 22;
    unsigned int page_table_index = (virtual_addr >> 12) & 0x3FF;
    
    // 如果页目录项不存在，则创建页表
    if (!page_dir->entries[page_dir_index].present) {
        page_table_t* new_table = vm_create_page_table();
        if (!new_table) {
            return -1;
        }
        
        // 设置页目录项
        page_dir->entries[page_dir_index].present = 1;
        page_dir->entries[page_dir_index].rw = rw;
        page_dir->entries[page_dir_index].user = user;
        page_dir->entries[page_dir_index].frame = ((unsigned int)new_table) >> 12;
    }
    
    // 获取页表
    page_table_t* table = (page_table_t*)(page_dir->entries[page_dir_index].frame << 12);
    
    // 设置页表项
    table->entries[page_table_index].present = 1;
    table->entries[page_table_index].rw = rw;
    table->entries[page_table_index].user = user;
    table->entries[page_table_index].frame = physical_addr >> 12;
    
    // 刷新TLB
    flush_tlb_entry(virtual_addr);
    
    return 0;
}

// 处理页错误
void vm_handle_page_fault(unsigned int faulting_address, unsigned int error_code) {
    // 更新统计信息
    vm_statistics.page_faults++;
    
    print_string("Page fault at address: 0x");
    char addr_str[16];
    hex_to_string(faulting_address, addr_str);
    print_string(addr_str);
    print_string("\n");
    
    // 分析错误代码
    if (error_code & 0x1) {
        print_string("Page protection violation\n");
    } else {
        print_string("Page not present\n");
    }
    
    if (error_code & 0x2) {
        print_string("Write operation\n");
    } else {
        print_string("Read operation\n");
    }
    
    if (error_code & 0x4) {
        print_string("User mode access\n");
    } else {
        print_string("Supervisor mode access\n");
    }
    
    // 在实际实现中，这里会处理页错误
    // 可能的操作包括：
    // 1. 分配物理页帧
    // 2. 从磁盘加载页面
    // 3. 终止进程（如果访问非法）
    
    // 简化实现，只记录错误
}

// 获取虚拟内存统计信息
struct vm_stats* vm_get_stats() {
    return &vm_statistics;
}

// 显示虚拟内存统计信息
void vm_print_stats() {
    print_string("=== Virtual Memory Statistics ===\n");
    
    char stat_str[16];
    
    print_string("Total pages: ");
    int_to_string(vm_statistics.total_pages, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Used pages: ");
    int_to_string(vm_statistics.used_pages, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Free pages: ");
    int_to_string(vm_statistics.free_pages, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Page faults: ");
    int_to_string(vm_statistics.page_faults, stat_str);
    print_string(stat_str);
    print_string("\n");
}

// 加载页目录到CR3寄存器
void load_page_directory(page_directory_t* page_dir) {
    __asm__ volatile (
        "mov %0, %%cr3"
        :
        : "r"(page_dir)
        : "memory"
    );
}

// 启用分页
void enable_paging() {
    __asm__ volatile (
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0"
        :
        :
        : "eax", "memory"
    );
}

// 刷新TLB条目
void flush_tlb_entry(unsigned int virtual_addr) {
    __asm__ volatile (
        "invlpg (%0)"
        :
        : "r"(virtual_addr)
        : "memory"
    );
}

// 整数转十六进制字符串
void hex_to_string(unsigned int value, char* str) {
    const char hex_chars[] = "0123456789ABCDEF";
    int pos = 0;
    
    // 处理特殊情况
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    // 转换为十六进制
    char temp[16];
    int i = 0;
    while (value > 0) {
        temp[i++] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    // 反转字符串
    for (int j = i - 1; j >= 0; j--) {
        str[pos++] = temp[j];
    }
    str[pos] = '\0';
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