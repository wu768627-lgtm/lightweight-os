#ifndef POWER_H
#define POWER_H

// 电源状态定义
#define POWER_STATE_RUNNING   0
#define POWER_STATE_SLEEP     1
#define POWER_STATE_IDLE      2
#define POWER_STATE_SHUTDOWN  3
#define POWER_STATE_REBOOT    4

// 电源统计结构
struct power_stats {
    unsigned int total_state_changes;
    unsigned int time_in_running;
    unsigned int time_in_sleep;
    unsigned int time_in_idle;
};

// 函数声明
void power_init();
int power_check_acpi_support();
int power_set_state(int state);
int power_get_state();
void power_enter_running();
void power_enter_sleep();
void power_enter_idle();
void power_enter_shutdown();
void power_enter_reboot();
void power_perform_shutdown();
void power_perform_reboot();
void power_cpu_idle();
void power_enable_management();
void power_disable_management();
struct power_stats* power_get_stats();
void power_print_stats();
const char* power_state_to_string(int state);
unsigned int get_current_time();
void int_to_string(int value, char* str);

#endif