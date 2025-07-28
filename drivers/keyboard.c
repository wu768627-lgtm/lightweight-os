#include "keyboard.h"
#include "device.h"
#include "../kernel/kernel.h"
#include "../kernel/interrupts.h"

// 键盘状态
static unsigned char keyboard_status = 0;

// 键盘缓冲区
static unsigned char key_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_head = 0;
static int buffer_tail = 0;

// SHIFT、CTRL、ALT键状态
static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;

// 扫描码到ASCII码的映射表(无SHIFT)
static unsigned char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

// 扫描码到ASCII码的映射表(SHIFT按下)
static unsigned char scancode_to_ascii_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

// 键盘设备结构
static struct device keyboard_device = {
    .name = "ps2_keyboard",
    .type = DEVICE_TYPE_INPUT,
    .status = DEVICE_STATUS_READY,
    .flags = 0,
    .vendor_id = 0,
    .device_id = 0,
    .device_data = 0,
    .next = 0,
    .init = keyboard_init,
    .read = keyboard_read,
    .write = 0,
    .ioctl = keyboard_ioctl,
    .reset = keyboard_reset,
    .shutdown = keyboard_shutdown
};

// 键盘初始化
int keyboard_init(struct device* dev) {
    if (!dev) {
        return -1;
    }
    
    // 初始化键盘状态
    keyboard_status = 0;
    buffer_head = 0;
    buffer_tail = 0;
    shift_pressed = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;
    
    // 启用键盘中断
    irq_register(KEYBOARD_IRQ, keyboard_interrupt_handler, 0);
    irq_enable(KEYBOARD_IRQ);
    
    print_string("PS/2 Keyboard initialized\n");
    return 0;
}

// 键盘读取函数
int keyboard_read(struct device* dev, unsigned int offset, void* buffer, unsigned int count) {
    if (!dev || !buffer || count == 0) {
        return -1;
    }
    
    unsigned char* buf = (unsigned char*)buffer;
    int bytes_read = 0;
    
    // 从缓冲区读取数据
    while (bytes_read < (int)count && buffer_head != buffer_tail) {
        buf[bytes_read++] = key_buffer[buffer_head];
        buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
    }
    
    return bytes_read;
}

// 键盘控制函数
int keyboard_ioctl(struct device* dev, unsigned int command, void* args) {
    if (!dev) {
        return -1;
    }
    
    switch (command) {
        case KEYBOARD_IOCTL_RESET:
            return keyboard_reset(dev);
            
        case KEYBOARD_IOCTL_SET_LEDS:
            // 在实际实现中，这里会设置键盘LED状态
            return 0;
            
        case KEYBOARD_IOCTL_GET_STATUS:
            if (args) {
                *((unsigned char*)args) = keyboard_status;
                return 0;
            }
            return -1;
            
        default:
            return -1;
    }
}

// 键盘重置函数
int keyboard_reset(struct device* dev) {
    if (!dev) {
        return -1;
    }
    
    // 重置键盘状态
    buffer_head = 0;
    buffer_tail = 0;
    shift_pressed = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;
    
    print_string("Keyboard reset\n");
    return 0;
}

// 键盘关闭函数
int keyboard_shutdown(struct device* dev) {
    if (!dev) {
        return -1;
    }
    
    // 禁用键盘中断
    irq_disable(KEYBOARD_IRQ);
    
    print_string("Keyboard shutdown\n");
    return 0;
}

// 键盘中断处理函数
void keyboard_interrupt_handler(unsigned int irq, void* data) {
    // 读取扫描码
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    
    // 处理按键释放
    if (scancode & 0x80) {
        scancode &= 0x7F;
        
        // 更新修饰键状态
        switch (scancode) {
            case SCANCODE_SHIFT_LEFT:
            case SCANCODE_SHIFT_RIGHT:
                shift_pressed = 0;
                break;
                
            case SCANCODE_CTRL:
                ctrl_pressed = 0;
                break;
                
            case SCANCODE_ALT:
                alt_pressed = 0;
                break;
        }
        
        return;
    }
    
    // 处理按键按下
    switch (scancode) {
        case SCANCODE_SHIFT_LEFT:
        case SCANCODE_SHIFT_RIGHT:
            shift_pressed = 1;
            return;
            
        case SCANCODE_CTRL:
            ctrl_pressed = 1;
            return;
            
        case SCANCODE_ALT:
            alt_pressed = 1;
            return;
            
        case SCANCODE_CAPS_LOCK:
            // 切换大写锁定状态
            keyboard_status ^= KEYBOARD_STATUS_CAPS_LOCK;
            return;
            
        case SCANCODE_NUM_LOCK:
            // 切换数字锁定状态
            keyboard_status ^= KEYBOARD_STATUS_NUM_LOCK;
            return;
            
        case SCANCODE_SCROLL_LOCK:
            // 切换滚动锁定状态
            keyboard_status ^= KEYBOARD_STATUS_SCROLL_LOCK;
            return;
    }
    
    // 转换扫描码为ASCII码
    unsigned char ascii = 0;
    if (shift_pressed) {
        if (scancode < sizeof(scancode_to_ascii_shift)) {
            ascii = scancode_to_ascii_shift[scancode];
        }
    } else {
        if (scancode < sizeof(scancode_to_ascii)) {
            ascii = scancode_to_ascii[scancode];
        }
    }
    
    // 处理大写锁定
    if (keyboard_status & KEYBOARD_STATUS_CAPS_LOCK) {
        if (ascii >= 'a' && ascii <= 'z') {
            ascii = ascii - 'a' + 'A';
        } else if (ascii >= 'A' && ascii <= 'Z') {
            ascii = ascii - 'A' + 'a';
        }
    }
    
    // 将ASCII码放入缓冲区
    if (ascii != 0) {
        int next_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
        if (next_tail != buffer_head) { // 缓冲区未满
            key_buffer[buffer_tail] = ascii;
            buffer_tail = next_tail;
        }
    }
}

// 获取按键字符
int keyboard_getchar() {
    // 检查缓冲区是否为空
    if (buffer_head == buffer_tail) {
        return -1; // 无按键数据
    }
    
    // 从缓冲区读取字符
    unsigned char ch = key_buffer[buffer_head];
    buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
    
    return ch;
}

// 检查是否有按键数据
int keyboard_has_input() {
    return (buffer_head != buffer_tail);
}

// 注册键盘设备
void keyboard_register_device() {
    device_register(&keyboard_device);
}

// 从端口读取字节
unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}