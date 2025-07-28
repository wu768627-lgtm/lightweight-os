#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../kernel/kernel.h"

// 键盘端口
#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_COMMAND_PORT   0x64

// 键盘IRQ
#define KEYBOARD_IRQ            1

// 键盘缓冲区大小
#define KEYBOARD_BUFFER_SIZE    256

// 键盘状态位
#define KEYBOARD_STATUS_CAPS_LOCK    0x01
#define KEYBOARD_STATUS_NUM_LOCK     0x02
#define KEYBOARD_STATUS_SCROLL_LOCK  0x04

// 键盘IO控制命令
#define KEYBOARD_IOCTL_RESET         0x01
#define KEYBOARD_IOCTL_SET_LEDS      0x02
#define KEYBOARD_IOCTL_GET_STATUS    0x03

// 特殊扫描码
#define SCANCODE_SHIFT_LEFT     0x2A
#define SCANCODE_SHIFT_RIGHT    0x36
#define SCANCODE_CTRL           0x1D
#define SCANCODE_ALT            0x38
#define SCANCODE_CAPS_LOCK      0x3A
#define SCANCODE_NUM_LOCK       0x45
#define SCANCODE_SCROLL_LOCK    0x46

// 设备结构声明
struct device;

// 函数声明
int keyboard_init(struct device* dev);
int keyboard_read(struct device* dev, unsigned int offset, void* buffer, unsigned int count);
int keyboard_ioctl(struct device* dev, unsigned int command, void* args);
int keyboard_reset(struct device* dev);
int keyboard_shutdown(struct device* dev);
void keyboard_interrupt_handler(unsigned int irq, void* data);
int keyboard_getchar();
int keyboard_has_input();
void keyboard_register_device();

// 端口操作函数声明
unsigned char inb(unsigned short port);

#endif