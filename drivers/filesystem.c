#include "filesystem.h"
#include "../kernel/memory.h"
#include "../kernel/logger.h"

// 文件系统根节点
static struct fs_node* fs_root = 0;

// 初始化文件系统
void fs_init() {
    // 创建根节点
    fs_root = (struct fs_node*)allocate_memory(sizeof(struct fs_node));
    if (!fs_root) {
        LOG_ERROR("FS", "Failed to allocate memory for root node");
        return;
    }
    
    // 初始化根节点
    fs_root->type = FS_DIRECTORY;
    fs_root->name[0] = '/';
    fs_root->name[1] = '\0';
    fs_root->inode = 0;
    fs_root->permissions = 0755;
    fs_root->size = 0;
    fs_root->open = 0;
    fs_root->close = 0;
    fs_root->read = 0;
    fs_root->write = 0;
    fs_root->next = 0;
    fs_root->child = 0;
    
    LOG_INFO("FS", "Filesystem initialized");
}

// 获取根节点
struct fs_node* fs_get_root() {
    return fs_root;
}

// 打开文件
int fs_open(const char* path, unsigned int flags) {
    // 参数验证
    if (!path) {
        LOG_ERROR("FS", "Invalid path for fs_open");
        return -1;
    }
    
    // 检查路径长度
    int path_len = 0;
    while (path[path_len] && path_len < 256) {
        path_len++;
    }
    
    if (path_len >= 256) {
        LOG_ERROR("FS", "Path too long for fs_open");
        return -1;
    }
    
    // 查找文件节点
    struct fs_node* node = fs_find_node(path);
    if (!node) {
        // 如果是创建模式且文件不存在，则创建新文件
        if (flags & FS_CREATE) {
            node = fs_create_node(path, FS_FILE);
            if (!node) {
                LOG_ERROR("FS", "Failed to create node for path: %s", path);
                return -1;
            }
        } else {
            LOG_ERROR("FS", "File not found: %s", path);
            return -1;
        }
    }
    
    // 检查权限
    if ((flags & FS_WRITE) && !(node->permissions & 0200)) {
        LOG_ERROR("FS", "Permission denied for writing to: %s", path);
        return -1;
    }
    
    if ((flags & FS_READ) && !(node->permissions & 0400)) {
        LOG_ERROR("FS", "Permission denied for reading from: %s", path);
        return -1;
    }
    
    // 调用节点特定的打开函数
    if (node->open) {
        return node->open(node, flags);
    }
    
    LOG_DEBUG("FS", "File opened: %s", path);
    return (int)node; // 简化实现，直接返回节点指针
}

// 读取文件
int fs_read(int fd, void* buffer, unsigned int size) {
    // 参数验证
    if (!buffer || size == 0) {
        LOG_ERROR("FS", "Invalid buffer or size for fs_read");
        return -1;
    }
    
    // 限制最大读取大小
    if (size > 65536) { // 限制为64KB
        LOG_WARNING("FS", "Read size too large, limiting to 64KB");
        size = 65536;
    }
    
    struct fs_node* node = (struct fs_node*)fd;
    if (!node) {
        LOG_ERROR("FS", "Invalid file descriptor for fs_read");
        return -1;
    }
    
    // 调用节点特定的读取函数
    if (node->read) {
        return node->read(node, buffer, size);
    }
    
    LOG_DEBUG("FS", "Read from file: %d bytes", size);
    return 0; // 简化实现
}

// 写入文件
int fs_write(int fd, void* buffer, unsigned int size) {
    // 参数验证
    if (!buffer || size == 0) {
        LOG_ERROR("FS", "Invalid buffer or size for fs_write");
        return -1;
    }
    
    // 限制最大写入大小
    if (size > 65536) { // 限制为64KB
        LOG_WARNING("FS", "Write size too large, limiting to 64KB");
        size = 65536;
    }
    
    struct fs_node* node = (struct fs_node*)fd;
    if (!node) {
        LOG_ERROR("FS", "Invalid file descriptor for fs_write");
        return -1;
    }
    
    // 调用节点特定的写入函数
    if (node->write) {
        return node->write(node, buffer, size);
    }
    
    LOG_DEBUG("FS", "Write to file: %d bytes", size);
    return 0; // 简化实现
}

// 关闭文件
int fs_close(int fd) {
    struct fs_node* node = (struct fs_node*)fd;
    if (!node) {
        LOG_ERROR("FS", "Invalid file descriptor for fs_close");
        return -1;
    }
    
    // 调用节点特定的关闭函数
    if (node->close) {
        int result = node->close(node);
        if (result < 0) {
            LOG_ERROR("FS", "Error closing file");
            return result;
        }
    }
    
    LOG_DEBUG("FS", "File closed");
    return 0;
}

// 查找文件节点
struct fs_node* fs_find_node(const char* path) {
    if (!path) {
        return 0;
    }
    
    // 检查路径长度
    int path_len = 0;
    while (path[path_len] && path_len < 256) {
        path_len++;
    }
    
    if (path_len >= 256) {
        LOG_ERROR("FS", "Path too long for fs_find_node");
        return 0;
    }
    
    // 简化实现，仅处理根目录
    if (path[0] == '/' && (path[1] == '\0' || path[1] == '\0')) {
        return fs_root;
    }
    
    // 在实际实现中，这里会遍历目录结构查找节点
    LOG_DEBUG("FS", "Find node: %s", path);
    return 0; // 简化实现
}

// 创建文件节点
struct fs_node* fs_create_node(const char* path, enum fs_node_type type) {
    if (!path) {
        LOG_ERROR("FS", "fs_create_node called with NULL path");
        return 0;
    }
    
    // 检查路径长度
    int path_len = 0;
    while (path[path_len] && path_len < 256) {
        path_len++;
    }
    
    if (path_len >= 256) {
        LOG_ERROR("FS", "Path too long for fs_create_node");
        return 0;
    }
    
    // 分配新节点内存
    struct fs_node* node = (struct fs_node*)allocate_memory(sizeof(struct fs_node));
    if (!node) {
        LOG_ERROR("FS", "Failed to allocate memory for new node");
        return 0;
    }
    
    // 初始化节点
    node->type = type;
    // 简化实现，仅复制路径的最后一部分作为名称
    int len = 0;
    const char* p = path;
    while (*p) {
        len++;
        p++;
    }
    
    int name_start = 0;
    for (int i = len - 1; i >= 0; i--) {
        if (path[i] == '/') {
            name_start = i + 1;
            break;
        }
    }
    
    int name_len = 0;
    for (int i = name_start; path[i] && path[i] != '/' && name_len < FS_NAME_MAX - 1; i++) {
        node->name[name_len] = path[i];
        name_len++;
    }
    node->name[name_len] = '\0';
    
    node->inode = 0; // 简化实现
    node->permissions = 0644;
    node->size = 0;
    node->open = 0;
    node->close = 0;
    node->read = 0;
    node->write = 0;
    node->next = 0;
    node->child = 0;
    
    // 将节点添加到目录结构中
    // 简化实现，直接添加到根目录下
    if (fs_root->child) {
        struct fs_node* current = fs_root->child;
        while (current->next) {
            current = current->next;
        }
        current->next = node;
    } else {
        fs_root->child = node;
    }
    
    LOG_DEBUG("FS", "Node created: %s", node->name);
    return node;
}