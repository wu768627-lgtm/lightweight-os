#ifndef FILESYSTEM_H
#define FILESYSTEM_H

// 文件类型
#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x04
#define FS_BLOCKDEVICE  0x08
#define FS_PIPE         0x10
#define FS_SYMLINK      0x20
#define FS_MOUNTPOINT   0x40

// 文件打开标志
#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_CREAT         0x0200
#define O_TRUNC         0x0400
#define O_APPEND        0x0800

// 文件系统节点结构
struct fs_node {
    char name[128];                     // 文件名
    unsigned int flags;                 // 文件类型和属性
    unsigned int inode;                 // inode号
    unsigned int size;                  // 文件大小
    unsigned int impl;                  // 实现定义的数字
    struct fs_node* ptr;                // 指向其他数据的指针
    struct fs_node* parent;             // 父目录
    struct fs_node* children;           // 子节点链表
    struct fs_node* next;               // 链表中的下一个节点
    struct fs_node* prev;               // 链表中的前一个节点
    
    // 函数指针
    unsigned int (*read)(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
    unsigned int (*write)(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
    void (*open)(struct fs_node* node, unsigned char read, unsigned char write);
    void (*close)(struct fs_node* node);
    struct dirent* (*readdir)(struct fs_node* node, unsigned int index);
    struct fs_node* (*finddir)(struct fs_node* node, char* name);
};

// 目录项结构
struct dirent {
    char name[128];     // 文件名
    unsigned int ino;   // inode号
};

// 文件描述符结构
struct file_descriptor {
    struct fs_node* node;
    unsigned int offset;
    unsigned int flags;
};

// 函数声明
void fs_init();
struct fs_node* fs_root;  // 根文件系统节点
struct fs_node* fs_mount(struct fs_node* node, const char* mountpoint);
struct fs_node* fs_open(const char* path, unsigned int flags);
void fs_close(struct fs_node* node);
unsigned int fs_read(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int fs_write(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
struct dirent* fs_readdir(struct fs_node* node, unsigned int index);
struct fs_node* fs_finddir(struct fs_node* node, char* name);
int fs_mkdir(const char* path);
int fs_create_file(const char* path);

#endif