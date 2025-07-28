#include "logger.h"
#include "kernel.h"
#include "memory.h"

// 日志条目缓冲区
static struct log_entry log_buffer[LOG_BUFFER_SIZE];
static int log_head = 0;
static int log_tail = 0;
static int log_count = 0;

// 日志级别
static int current_log_level = LOG_LEVEL_INFO;

// 日志统计
static struct log_stats log_statistics;

// 初始化日志系统
void logger_init() {
    // 初始化缓冲区
    log_head = 0;
    log_tail = 0;
    log_count = 0;
    
    // 初始化统计信息
    log_statistics.total_entries = 0;
    log_statistics.debug_entries = 0;
    log_statistics.info_entries = 0;
    log_statistics.warning_entries = 0;
    log_statistics.error_entries = 0;
    log_statistics.critical_entries = 0;
    log_statistics.dropped_entries = 0;
    
    print_string("System logger initialized\n");
}

// 设置日志级别
void logger_set_level(int level) {
    if (level >= LOG_LEVEL_DEBUG && level <= LOG_LEVEL_CRITICAL) {
        current_log_level = level;
        print_string("Log level set to ");
        char level_str[12];
        int_to_string(level, level_str);
        print_string(level_str);
        print_string("\n");
    }
}

// 记录日志条目
void logger_log(int level, const char* module, const char* message) {
    // 检查日志级别
    if (level < current_log_level) {
        return;
    }
    
    // 检查缓冲区是否已满
    if (log_count >= LOG_BUFFER_SIZE) {
        // 丢弃最旧的日志条目
        log_head = (log_head + 1) % LOG_BUFFER_SIZE;
        log_count--;
        log_statistics.dropped_entries++;
    }
    
    // 获取当前时间戳
    unsigned long long timestamp = get_timestamp();
    
    // 创建日志条目
    struct log_entry* entry = &log_buffer[log_tail];
    entry->timestamp = timestamp;
    entry->level = level;
    entry->module[MODULE_NAME_MAX - 1] = '\0';
    entry->message[MESSAGE_MAX - 1] = '\0';
    
    // 复制模块名和消息（限制长度）
    int i;
    for (i = 0; i < MODULE_NAME_MAX - 1 && module[i]; i++) {
        entry->module[i] = module[i];
    }
    entry->module[i] = '\0';
    
    for (i = 0; i < MESSAGE_MAX - 1 && message[i]; i++) {
        entry->message[i] = message[i];
    }
    entry->message[i] = '\0';
    
    // 更新缓冲区指针
    log_tail = (log_tail + 1) % LOG_BUFFER_SIZE;
    log_count++;
    
    // 更新统计信息
    log_statistics.total_entries++;
    switch (level) {
        case LOG_LEVEL_DEBUG:
            log_statistics.debug_entries++;
            break;
        case LOG_LEVEL_INFO:
            log_statistics.info_entries++;
            break;
        case LOG_LEVEL_WARNING:
            log_statistics.warning_entries++;
            break;
        case LOG_LEVEL_ERROR:
            log_statistics.error_entries++;
            break;
        case LOG_LEVEL_CRITICAL:
            log_statistics.critical_entries++;
            break;
    }
    
    // 如果是错误或关键级别，立即输出到控制台
    if (level >= LOG_LEVEL_ERROR) {
        print_string("[");
        switch (level) {
            case LOG_LEVEL_DEBUG:
                print_string("DEBUG");
                break;
            case LOG_LEVEL_INFO:
                print_string("INFO");
                break;
            case LOG_LEVEL_WARNING:
                print_string("WARNING");
                break;
            case LOG_LEVEL_ERROR:
                print_string("ERROR");
                break;
            case LOG_LEVEL_CRITICAL:
                print_string("CRITICAL");
                break;
        }
        print_string("] ");
        print_string(module);
        print_string(": ");
        print_string((char*)message);
        print_string("\n");
    }
}

// 获取日志统计信息
struct log_stats* logger_get_stats() {
    return &log_statistics;
}

// 显示日志统计信息
void logger_print_stats() {
    print_string("=== Logger Statistics ===\n");
    
    char stat_str[16];
    
    print_string("Total entries: ");
    int_to_string(log_statistics.total_entries, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Debug entries: ");
    int_to_string(log_statistics.debug_entries, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Info entries: ");
    int_to_string(log_statistics.info_entries, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Warning entries: ");
    int_to_string(log_statistics.warning_entries, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Error entries: ");
    int_to_string(log_statistics.error_entries, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Critical entries: ");
    int_to_string(log_statistics.critical_entries, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Dropped entries: ");
    int_to_string(log_statistics.dropped_entries, stat_str);
    print_string(stat_str);
    print_string("\n");
    
    print_string("Current buffer usage: ");
    int usage = (log_count * 100) / LOG_BUFFER_SIZE;
    int_to_string(usage, stat_str);
    print_string(stat_str);
    print_string("%\n");
}

// 导出日志到缓冲区
int logger_export(char* buffer, int buffer_size) {
    if (!buffer || buffer_size <= 0) {
        return -1;
    }
    
    int pos = 0;
    int entries_exported = 0;
    
    // 添加头部信息
    const char* header = "=== System Log Export ===\n";
    int header_len = 0;
    while (header[header_len] && pos < buffer_size - 1) {
        buffer[pos++] = header[header_len++];
    }
    
    // 导出日志条目
    int index = log_head;
    for (int i = 0; i < log_count && pos < buffer_size - 128; i++) {
        struct log_entry* entry = &log_buffer[index];
        
        // 时间戳
        buffer[pos++] = '[';
        long_long_to_string(entry->timestamp, &buffer[pos]);
        while (buffer[pos]) pos++;
        buffer[pos++] = ']';
        buffer[pos++] = ' ';
        
        // 日志级别
        switch (entry->level) {
            case LOG_LEVEL_DEBUG:
                buffer[pos++] = 'D';
                break;
            case LOG_LEVEL_INFO:
                buffer[pos++] = 'I';
                break;
            case LOG_LEVEL_WARNING:
                buffer[pos++] = 'W';
                break;
            case LOG_LEVEL_ERROR:
                buffer[pos++] = 'E';
                break;
            case LOG_LEVEL_CRITICAL:
                buffer[pos++] = 'C';
                break;
        }
        buffer[pos++] = ' ';
        
        // 模块名
        int j = 0;
        while (entry->module[j] && pos < buffer_size - 1) {
            buffer[pos++] = entry->module[j++];
        }
        buffer[pos++] = ':';
        buffer[pos++] = ' ';
        
        // 消息
        j = 0;
        while (entry->message[j] && pos < buffer_size - 1) {
            buffer[pos++] = entry->message[j++];
        }
        buffer[pos++] = '\n';
        
        index = (index + 1) % LOG_BUFFER_SIZE;
        entries_exported++;
    }
    
    // 添加尾部信息
    const char* footer = "=== End of Log ===\n";
    int footer_len = 0;
    while (footer[footer_len] && pos < buffer_size - 1) {
        buffer[pos++] = footer[footer_len++];
    }
    
    buffer[pos] = '\0';
    return entries_exported;
}

// 清空日志缓冲区
void logger_clear() {
    log_head = 0;
    log_tail = 0;
    log_count = 0;
    
    print_string("Log buffer cleared\n");
}

// 获取时间戳
unsigned long long get_timestamp() {
    // 在实际实现中，这会从高精度时钟获取时间戳
    static unsigned long long timestamp = 0;
    return timestamp++;
}

// 长整数转字符串
void long_long_to_string(unsigned long long value, char* str) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    char temp[32];
    int i = 0;
    
    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    int j = 0;
    for (int k = i - 1; k >= 0; k--) {
        str[j++] = temp[k];
    }
    str[j] = '\0';
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