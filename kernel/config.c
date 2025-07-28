#include "config.h"
#include "kernel.h"
#include "filesystem.h"
#include "memory.h"
#include "logger.h"

// 系统配置结构
static struct system_config global_config;

// 配置文件路径
#define CONFIG_FILE_PATH "/etc/system.conf"

// 默认配置值
static const struct system_config default_config = {
    .max_processes = 64,
    .max_open_files = 128,
    .max_network_connections = 256,
    .log_level = LOG_LEVEL_INFO,
    .scheduler_time_slice = 10,
    .memory_pool_size = 4 * 1024 * 1024, // 4MB
    .network_buffer_size = 8192,
    .filesystem_cache_size = 1024 * 1024, // 1MB
    .enable_audit = 1,
    .enable_profiling = 1,
    .hostname = "lightweightos",
    .timezone = 0
};

// 初始化配置系统
void config_init() {
    LOG_INFO("CONFIG", "Initializing configuration system");
    
    // 首先设置默认配置
    global_config = default_config;
    
    // 尝试从配置文件加载配置
    if (config_load_from_file(CONFIG_FILE_PATH) < 0) {
        LOG_WARNING("CONFIG", "Failed to load config from file, using defaults");
    }
    
    LOG_INFO("CONFIG", "Configuration system initialized");
}

// 从文件加载配置
int config_load_from_file(const char* filepath) {
    LOG_INFO("CONFIG", "Loading configuration from file");
    
    // 在实际实现中，这里会从文件系统读取配置
    // 简化实现，模拟读取过程
    print_string("Loading configuration from ");
    print_string((char*)filepath);
    print_string("\n");
    
    // 模拟配置文件内容解析
    // 在实际实现中，这里会解析配置文件内容并更新全局配置
    
    return 0; // 成功
}

// 保存配置到文件
int config_save_to_file(const char* filepath) {
    LOG_INFO("CONFIG", "Saving configuration to file");
    
    // 在实际实现中，这里会将配置保存到文件系统
    print_string("Saving configuration to ");
    print_string((char*)filepath);
    print_string("\n");
    
    return 0; // 成功
}

// 获取配置值
struct system_config* config_get() {
    return &global_config;
}

// 设置配置值
int config_set(const struct system_config* new_config) {
    if (!new_config) {
        return -1;
    }
    
    global_config = *new_config;
    LOG_INFO("CONFIG", "Configuration updated");
    return 0;
}

// 获取特定配置项
int config_get_max_processes() {
    return global_config.max_processes;
}

int config_get_max_open_files() {
    return global_config.max_open_files;
}

int config_get_log_level() {
    return global_config.log_level;
}

int config_get_scheduler_time_slice() {
    return global_config.scheduler_time_slice;
}

unsigned int config_get_memory_pool_size() {
    return global_config.memory_pool_size;
}

const char* config_get_hostname() {
    return global_config.hostname;
}

int config_get_timezone() {
    return global_config.timezone;
}

// 设置特定配置项
int config_set_max_processes(int max_processes) {
    if (max_processes <= 0 || max_processes > 1024) {
        LOG_ERROR("CONFIG", "Invalid max_processes value");
        return -1;
    }
    
    global_config.max_processes = max_processes;
    LOG_INFO("CONFIG", "Max processes updated");
    return 0;
}

int config_set_log_level(int level) {
    if (level < LOG_LEVEL_DEBUG || level > LOG_LEVEL_CRITICAL) {
        LOG_ERROR("CONFIG", "Invalid log level");
        return -1;
    }
    
    global_config.log_level = level;
    LOG_INFO("CONFIG", "Log level updated");
    logger_set_level(level);
    return 0;
}

int config_set_hostname(const char* hostname) {
    if (!hostname || strlen(hostname) >= sizeof(global_config.hostname)) {
        LOG_ERROR("CONFIG", "Invalid hostname");
        return -1;
    }
    
    strcpy(global_config.hostname, hostname);
    LOG_INFO("CONFIG", "Hostname updated");
    return 0;
}

// 显示当前配置
void config_print() {
    print_string("=== System Configuration ===\n");
    
    char buffer[32];
    
    print_string("Max Processes: ");
    int_to_string(global_config.max_processes, buffer);
    print_string(buffer);
    print_string("\n");
    
    print_string("Max Open Files: ");
    int_to_string(global_config.max_open_files, buffer);
    print_string(buffer);
    print_string("\n");
    
    print_string("Max Network Connections: ");
    int_to_string(global_config.max_network_connections, buffer);
    print_string(buffer);
    print_string("\n");
    
    print_string("Log Level: ");
    int_to_string(global_config.log_level, buffer);
    print_string(buffer);
    print_string("\n");
    
    print_string("Scheduler Time Slice: ");
    int_to_string(global_config.scheduler_time_slice, buffer);
    print_string(buffer);
    print_string(" ticks\n");
    
    print_string("Memory Pool Size: ");
    int_to_string(global_config.memory_pool_size, buffer);
    print_string(buffer);
    print_string(" bytes\n");
    
    print_string("Network Buffer Size: ");
    int_to_string(global_config.network_buffer_size, buffer);
    print_string(buffer);
    print_string(" bytes\n");
    
    print_string("Filesystem Cache Size: ");
    int_to_string(global_config.filesystem_cache_size, buffer);
    print_string(buffer);
    print_string(" bytes\n");
    
    print_string("Audit Enabled: ");
    print_string(global_config.enable_audit ? "yes" : "no");
    print_string("\n");
    
    print_string("Profiling Enabled: ");
    print_string(global_config.enable_profiling ? "yes" : "no");
    print_string("\n");
    
    print_string("Hostname: ");
    print_string(global_config.hostname);
    print_string("\n");
    
    print_string("Timezone: ");
    int_to_string(global_config.timezone, buffer);
    print_string(buffer);
    print_string("\n");
}

// 重置为默认配置
void config_reset_to_defaults() {
    global_config = default_config;
    LOG_INFO("CONFIG", "Configuration reset to defaults");
    print_string("Configuration reset to defaults\n");
}

// 字符串长度
int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// 字符串复制
char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
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