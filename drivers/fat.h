#ifndef FAT_H
#define FAT_H

// FAT类型
#define FAT12 0
#define FAT16 1
#define FAT32 2

// FAT引导扇区结构
struct fat_boot_sector {
    unsigned char jump[3];                      // 跳转指令
    unsigned char oem_name[8];                  // OEM名称
    unsigned short bytes_per_sector;            // 每扇区字节数
    unsigned char sectors_per_cluster;          // 每簇扇区数
    unsigned short reserved_sectors;            // 保留扇区数
    unsigned char number_of_fats;               // FAT表数量
    unsigned short root_entries;                // 根目录项数
    unsigned short small_sectors;               // 小扇区数
    unsigned char media_descriptor;             // 媒体描述符
    unsigned short sectors_per_fat;             // 每FAT扇区数
    unsigned short sectors_per_track;           // 每磁道扇区数
    unsigned short number_of_heads;             // 磁头数
    unsigned int hidden_sectors;                // 隐藏扇区数
    unsigned int large_sectors;                 // 大扇区数
    
    union {
        // FAT12/FAT16扩展引导扇区
        struct {
            unsigned char drive_number;         // 驱动器号
            unsigned char reserved;             // 保留
            unsigned char extended_boot_signature; // 扩展引导签名
            unsigned int volume_id;             // 卷ID
            unsigned char volume_label[11];     // 卷标
            unsigned char filesystem_type[8];   // 文件系统类型
        } __attribute__((packed)) fat16;
        
        // FAT32扩展引导扇区
        struct {
            unsigned int sectors_per_fat32;     // 每FAT扇区数(FAT32)
            unsigned short extended_flags;      // 扩展标志
            unsigned short filesystem_version;  // 文件系统版本
            unsigned int root_cluster;          // 根目录簇号
            unsigned short fs_info_sector;      // FS信息扇区
            unsigned short backup_boot_sector;  // 备份引导扇区
            unsigned char reserved[12];         // 保留
            unsigned char drive_number;         // 驱动器号
            unsigned char reserved1;            // 保留
            unsigned char extended_boot_signature; // 扩展引导签名
            unsigned int volume_id;             // 卷ID
            unsigned char volume_label[11];     // 卷标
            unsigned char filesystem_type[8];   // 文件系统类型
        } __attribute__((packed)) fat32;
    } ext_boot_signature;
    
    unsigned char bootstrap_code[448];          // 引导代码
    unsigned short signature;                   // 签名(0xAA55)
} __attribute__((packed));

// FAT目录项结构
struct fat_directory_entry {
    unsigned char name[11];                     // 文件名(8.3格式)
    unsigned char attributes;                   // 属性
    unsigned char reserved;                     // 保留
    unsigned char creation_time_tenths;         // 创建时间(十分之一秒)
    unsigned short creation_time;               // 创建时间
    unsigned short creation_date;               // 创建日期
    unsigned short last_access_date;            // 最后访问日期
    unsigned short first_cluster_high;          // 起始簇号高16位(FAT32)
    unsigned short last_write_time;             // 最后写入时间
    unsigned short last_write_date;             // 最后写入日期
    unsigned short first_cluster_low;           // 起始簇号低16位
    unsigned int file_size;                     // 文件大小(字节)
} __attribute__((packed));

// 长文件名目录项结构
struct fat_lfn_entry {
    unsigned char sequence_number;              // 序列号
    unsigned char name1[10];                    // 名称第1部分
    unsigned char attributes;                   // 属性(必须为ATTR_LONG_NAME)
    unsigned char reserved;                     // 保留
    unsigned char checksum;                     // 短文件名校验和
    unsigned char name2[12];                    // 名称第2部分
    unsigned short first_cluster;               // 起始簇号(必须为0)
    unsigned char name3[4];                     // 名称第3部分
} __attribute__((packed));

// FAT文件系统信息结构
struct fat_info {
    unsigned int type;                          // FAT类型(FAT12/FAT16/FAT32)
    unsigned int bytes_per_sector;              // 每扇区字节数
    unsigned int sectors_per_cluster;           // 每簇扇区数
    unsigned int reserved_sectors;              // 保留扇区数
    unsigned int number_of_fats;                // FAT表数量
    unsigned int root_entries;                  // 根目录项数
    unsigned int total_sectors;                 // 总扇区数
    unsigned int fat_size;                      // 每FAT扇区数
    unsigned int root_cluster;                  // 根目录簇号(FAT32)
    unsigned int fat_offset;                    // FAT表偏移
    unsigned int root_offset;                   // 根目录偏移
    unsigned int data_offset;                   // 数据区偏移
    unsigned int cluster_size;                  // 簇大小(字节)
    void* device;                               // 底层设备
};

// FAT属性
#define FAT_ATTR_READ_ONLY      0x01
#define FAT_ATTR_HIDDEN         0x02
#define FAT_ATTR_SYSTEM         0x04
#define FAT_ATTR_VOLUME_ID      0x08
#define FAT_ATTR_DIRECTORY      0x10
#define FAT_ATTR_ARCHIVE        0x20
#define FAT_ATTR_LONG_NAME      0x0F

// 函数声明
int fat_init(struct fat_info* info, unsigned char* data, unsigned int size);
unsigned int fat_read(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int fat_write(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
void fat_open(struct fs_node* node, unsigned char read, unsigned char write);
void fat_close(struct fs_node* node);
struct dirent* fat_readdir(struct fs_node* node, unsigned int index);
struct fs_node* fat_finddir(struct fs_node* node, char* name);
struct fs_node* fat_get_root_node();

#endif