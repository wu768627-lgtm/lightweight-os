#include "fat.h"
#include "filesystem.h"
#include "../kernel/kernel.h"
#include "../libs/string.h"

// FAT文件系统信息
static struct fat_info fat_fs_info;

// FAT文件系统根目录
static struct fs_node fat_root_node;

// 初始化FAT文件系统
int fat_init(struct fat_info* info, unsigned char* data, unsigned int size) {
    if (!info || !data) {
        return -1;
    }
    
    // 初始化FAT信息
    fat_fs_info = *info;
    
    // 解析FAT引导扇区
    struct fat_boot_sector* boot = (struct fat_boot_sector*)data;
    
    // 检查签名
    if (boot->signature != 0xAA55) {
        print_string("FAT: Invalid boot sector signature\n");
        return -1;
    }
    
    // 提取FAT文件系统参数
    fat_fs_info.bytes_per_sector = boot->bytes_per_sector;
    fat_fs_info.sectors_per_cluster = boot->sectors_per_cluster;
    fat_fs_info.reserved_sectors = boot->reserved_sectors;
    fat_fs_info.number_of_fats = boot->number_of_fats;
    fat_fs_info.root_entries = boot->root_entries;
    fat_fs_info.total_sectors = boot->small_sectors ? boot->small_sectors : boot->large_sectors;
    fat_fs_info.fat_size = boot->sectors_per_fat ? boot->sectors_per_fat : boot->ext_boot_signature.sectors_per_fat32;
    fat_fs_info.root_cluster = boot->ext_boot_signature.root_cluster;
    
    // 计算各种偏移量
    fat_fs_info.fat_offset = fat_fs_info.reserved_sectors * fat_fs_info.bytes_per_sector;
    fat_fs_info.root_offset = fat_fs_info.fat_offset + (fat_fs_info.number_of_fats * fat_fs_info.fat_size * fat_fs_info.bytes_per_sector);
    
    if (fat_fs_info.type == FAT32) {
        fat_fs_info.data_offset = fat_fs_info.root_offset;
        fat_fs_info.root_offset = 0; // FAT32没有固定的根目录区域
    } else {
        unsigned int root_size = fat_fs_info.root_entries * sizeof(struct fat_directory_entry);
        fat_fs_info.data_offset = fat_fs_info.root_offset + root_size;
    }
    
    fat_fs_info.cluster_size = fat_fs_info.sectors_per_cluster * fat_fs_info.bytes_per_sector;
    
    print_string("FAT filesystem initialized:\n");
    print_string("  Type: FAT");
    char type_str[12];
    if (fat_fs_info.type == FAT12) {
        strcpy(type_str, "12");
    } else if (fat_fs_info.type == FAT16) {
        strcpy(type_str, "16");
    } else {
        strcpy(type_str, "32");
    }
    print_string(type_str);
    print_string("\n");
    print_string("  Cluster size: ");
    int_to_string(fat_fs_info.cluster_size, type_str);
    print_string(type_str);
    print_string(" bytes\n");
    
    // 初始化根节点
    fat_root_node.flags = FS_DIRECTORY;
    strcpy(fat_root_node.name, "fat_root");
    fat_root_node.inode = 0;
    
    // 设置函数指针
    fat_root_node.read = fat_read;
    fat_root_node.write = fat_write;
    fat_root_node.open = fat_open;
    fat_root_node.close = fat_close;
    fat_root_node.readdir = fat_readdir;
    fat_root_node.finddir = fat_finddir;
    
    return 0;
}

// 读取FAT数据
unsigned int fat_read(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
    if (!node || !buffer) {
        return 0;
    }
    
    print_string("FAT: Reading from file ");
    print_string(node->name);
    print_string("\n");
    
    // 在实际实现中，这里会从存储设备读取文件数据
    // 简化实现，只返回0
    return 0;
}

// 写入FAT数据
unsigned int fat_write(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer) {
    if (!node || !buffer) {
        return 0;
    }
    
    print_string("FAT: Writing to file ");
    print_string(node->name);
    print_string("\n");
    
    // 在实际实现中，这里会将数据写入存储设备
    // 简化实现，只返回写入大小
    return size;
}

// 打开FAT文件
void fat_open(struct fs_node* node, unsigned char read, unsigned char write) {
    if (!node) {
        return;
    }
    
    print_string("FAT: Opening file ");
    print_string(node->name);
    print_string("\n");
    
    // 在实际实现中，这里会执行打开文件的操作
}

// 关闭FAT文件
void fat_close(struct fs_node* node) {
    if (!node) {
        return;
    }
    
    print_string("FAT: Closing file ");
    print_string(node->name);
    print_string("\n");
    
    // 在实际实现中，这里会执行关闭文件的操作
}

// 读取FAT目录项
struct dirent* fat_readdir(struct fs_node* node, unsigned int index) {
    if (!node) {
        return 0;
    }
    
    static struct dirent entry;
    
    print_string("FAT: Reading directory entry ");
    char index_str[12];
    int_to_string(index, index_str);
    print_string(index_str);
    print_string("\n");
    
    // 在实际实现中，这里会从目录中读取指定索引的项
    // 简化实现，只在特定索引返回示例数据
    if (index == 0) {
        strcpy(entry.name, "example.txt");
        entry.ino = 2;
        return &entry;
    } else if (index == 1) {
        strcpy(entry.name, "readme.md");
        entry.ino = 3;
        return &entry;
    }
    
    return 0; // 没有更多目录项
}

// 查找FAT目录项
struct fs_node* fat_finddir(struct fs_node* node, char* name) {
    if (!node || !name) {
        return 0;
    }
    
    print_string("FAT: Finding directory entry ");
    print_string(name);
    print_string("\n");
    
    static struct fs_node found_node;
    
    // 在实际实现中，这里会在目录中查找指定名称的项
    // 简化实现，只匹配特定名称
    if (strcmp(name, "example.txt") == 0) {
        strcpy(found_node.name, "example.txt");
        found_node.flags = FS_FILE;
        found_node.inode = 2;
        found_node.size = 1024;
        found_node.read = fat_read;
        found_node.write = fat_write;
        found_node.open = fat_open;
        found_node.close = fat_close;
        return &found_node;
    }
    
    return 0; // 未找到
}

// 获取FAT根节点
struct fs_node* fat_get_root_node() {
    return &fat_root_node;
}

// 字符串比较函数
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// 字符串复制函数
char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
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