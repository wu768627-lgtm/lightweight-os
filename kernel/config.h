#ifndef CONFIG_H
#define CONFIG_H

// 系统配置结构
struct system_config {
    int max_processes;              // 最大进程数
    int max_open_files;             // 最大打开文件数
    int max_network_connections;    // 最大网络连接数
    int log_level;                  // 日志级别
    int scheduler_time_slice;       // 调度器时间片
    unsigned int memory_pool_size;  // 内存池大小
    unsigned int network_buffer_size; // 网络缓冲区大小
    unsigned int filesystem_cache_size; // 文件系统缓存大小
    int enable_audit;               // 是否启用审计
    int enable_profiling;           // 是否启用性能分析
    char hostname[64];              // 主机名
    int timezone;                   // 时区（相对于UTC的小时数）
};

// 函数声明
void config_init();
int config_load_from_file(const char* filepath);
int config_save_to_file(const char* filepath);
struct system_config* config_get();
int config_set(const struct system_config* new_config);

// 获取特定配置项
int config_get_max_processes();
int config_get_max_open_files();
int config_get_log_level();
int config_get_scheduler_time_slice();
unsigned int config_get_memory_pool_size();
const char* config_get_hostname();
int config_get_timezone();

// 设置特定配置项
int config_set_max_processes(int max_processes);
int config_set_log_level(int level);
int config_set_hostname(const char* hostname);

// 其他配置函数
void config_print();
void config_reset_to_defaults();

// 辅助函数
int strlen(const char* str);
char* strcpy(char* dest, const char* src);
void int_to_string(int value, char* str);

#endif