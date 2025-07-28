#ifndef DEVICE_H
#define DEVICE_H

// 设备类型
#define DEVICE_TYPE_BLOCK       1
#define DEVICE_TYPE_CHARACTER   2
#define DEVICE_TYPE_NETWORK     3
#define DEVICE_TYPE_INPUT       4
#define DEVICE_TYPE_DISPLAY     5
#define DEVICE_TYPE_STORAGE     6

// 设备状态
#define DEVICE_STATUS_UNINITIALIZED 0
#define DEVICE_STATUS_READY         1
#define DEVICE_STATUS_BUSY          2
#define DEVICE_STATUS_ERROR         3
#define DEVICE_STATUS_OFFLINE       4

// 设备标志
#define DEVICE_FLAG_REMOVABLE   0x01
#define DEVICE_FLAG_READONLY    0x02
#define DEVICE_FLAG_HOTPLUG     0x04

// 设备结构
struct device {
    char name[32];              // 设备名称
    unsigned int type;          // 设备类型
    unsigned int status;        // 设备状态
    unsigned int flags;         // 设备标志
    unsigned int vendor_id;     // 厂商ID
    unsigned int device_id;     // 设备ID
    void* device_data;          // 设备特定数据
    struct device* next;        // 链表中的下一个设备
    
    // 设备操作函数指针
    int (*init)(struct device* dev);
    int (*read)(struct device* dev, unsigned int offset, void* buffer, unsigned int count);
    int (*write)(struct device* dev, unsigned int offset, const void* buffer, unsigned int count);
    int (*ioctl)(struct device* dev, unsigned int command, void* args);
    int (*reset)(struct device* dev);
    int (*shutdown)(struct device* dev);
};

// 设备驱动程序结构
struct device_driver {
    char name[32];                      // 驱动程序名称
    unsigned int type;                  // 支持的设备类型
    unsigned int vendor_id;             // 支持的厂商ID (0表示通配)
    unsigned int device_id;             // 支持的设备ID (0表示通配)
    struct device_driver* next;         // 链表中的下一个驱动
    
    // 驱动程序函数指针
    int (*probe)(struct device* dev);
    int (*attach)(struct device* dev);
    int (*detach)(struct device* dev);
    int (*suspend)(struct device* dev);
    int (*resume)(struct device* dev);
};

// PCI设备结构
struct pci_device {
    unsigned char bus;          // 总线号
    unsigned char device;       // 设备号
    unsigned char function;     // 功能号
    unsigned short vendor_id;   // 厂商ID
    unsigned short device_id;   // 设备ID
    unsigned short command;     // 命令寄存器
    unsigned short status;      // 状态寄存器
    unsigned char revision_id;  // 修订ID
    unsigned char prog_if;      // 编程接口
    unsigned char subclass;     // 子类
    unsigned char class_code;   // 类代码
    unsigned char cache_line_size;
    unsigned char latency_timer;
    unsigned char header_type;
    unsigned char bist;         // 内置自检测试
    unsigned int bars[6];       // 基地址寄存器
    unsigned int cardbus_cis_ptr;
    unsigned short subsystem_vendor_id;
    unsigned short subsystem_id;
    unsigned int expansion_rom_base_addr;
    unsigned char capabilities_ptr;
    unsigned char interrupt_line;
    unsigned char interrupt_pin;
    unsigned char min_grant;
    unsigned char max_latency;
};

// 函数声明
void device_init();
int device_register(struct device* dev);
int device_unregister(const char* name);
struct device* device_get(const char* name);
struct device* device_get_by_type(unsigned int type);
void device_list();
int device_read(struct device* dev, unsigned int offset, void* buffer, unsigned int count);
int device_write(struct device* dev, unsigned int offset, const void* buffer, unsigned int count);
int device_ioctl(struct device* dev, unsigned int command, void* args);
int device_reset(struct device* dev);
int device_shutdown(struct device* dev);

// 驱动程序管理
int driver_register(struct device_driver* driver);
int driver_unregister(const char* name);
struct device_driver* driver_get(const char* name);
struct device_driver* driver_match(struct device* dev);

// PCI相关函数
void pci_init();
struct pci_device* pci_find_device(unsigned short vendor_id, unsigned short device_id);
struct pci_device* pci_find_class(unsigned char class_code, unsigned char subclass);
void pci_read_config_byte(struct pci_device* dev, unsigned char offset, unsigned char* value);
void pci_read_config_word(struct pci_device* dev, unsigned char offset, unsigned short* value);
void pci_read_config_dword(struct pci_device* dev, unsigned char offset, unsigned int* value);
void pci_write_config_byte(struct pci_device* dev, unsigned char offset, unsigned char value);
void pci_write_config_word(struct pci_device* dev, unsigned char offset, unsigned short value);
void pci_write_config_dword(struct pci_device* dev, unsigned char offset, unsigned int value);
void pci_enumerate_devices();

// 中断处理
void irq_register(unsigned int irq, void (*handler)(unsigned int irq, void* data), void* data);
void irq_unregister(unsigned int irq);
void irq_enable(unsigned int irq);
void irq_disable(unsigned int irq);

// 输入设备
struct input_event {
    unsigned int type;      // 事件类型
    unsigned int code;      // 事件代码
    int value;              // 事件值
};

// 显示设备
struct display_info {
    unsigned int width;     // 宽度（像素）
    unsigned int height;    // 高度（像素）
    unsigned int bpp;       // 每像素位数
    unsigned int pitch;     // 每行字节数
    void* framebuffer;      // 帧缓冲区地址
};

// 存储设备
struct storage_info {
    unsigned long long capacity;    // 容量（字节）
    unsigned int block_size;        // 块大小（字节）
    unsigned int flags;             // 标志
};

#endif