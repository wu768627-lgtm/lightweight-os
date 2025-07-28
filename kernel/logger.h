#ifndef LOGGER_H
#define LOGGER_H

// 日志级别
#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_INFO      1
#define LOG_LEVEL_WARNING   2
#define LOG_LEVEL_ERROR     3
#define LOG_LEVEL_CRITICAL  4

// 日志缓冲区大小
#define LOG_BUFFER_SIZE     1024

// 模块名最大长度
#define MODULE_NAME_MAX     32

// 消息最大长度
#define MESSAGE_MAX         128

// 日志条目结构
struct log_entry {
    unsigned long long timestamp;           // 时间戳
    int level;                              // 日志级别
    char module[MODULE_NAME_MAX];           // 模块名
    char message[MESSAGE_MAX];              // 日志消息
};

// 日志统计结构
struct log_stats {
    unsigned int total_entries;             // 总条目数
    unsigned int debug_entries;             // 调试条目数
    unsigned int info_entries;              // 信息条目数
    unsigned int warning_entries;           // 警告条目数
    unsigned int error_entries;             // 错误条目数
    unsigned int critical_entries;          // 关键条目数
    unsigned int dropped_entries;           // 丢弃条目数
};

// 函数声明
void logger_init();
void logger_set_level(int level);
void logger_log(int level, const char* module, const char* message);
struct log_stats* logger_get_stats();
void logger_print_stats();
int logger_export(char* buffer, int buffer_size);
void logger_clear();

// 辅助函数
unsigned long long get_timestamp();
void long_long_to_string(unsigned long long value, char* str);
void int_to_string(int value, char* str);

// 便捷宏定义
#define LOG_DEBUG(module, message) logger_log(LOG_LEVEL_DEBUG, module, message)
#define LOG_INFO(module, message) logger_log(LOG_LEVEL_INFO, module, message)
#define LOG_WARNING(module, message) logger_log(LOG_LEVEL_WARNING, module, message)
#define LOG_ERROR(module, message) logger_log(LOG_LEVEL_ERROR, module, message)
#define LOG_CRITICAL(module, message) logger_log(LOG_LEVEL_CRITICAL, module, message)

#endif