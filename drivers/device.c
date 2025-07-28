#include "device.h"
#include "../kernel/kernel.h"
#include "../libs/string.h"

// 设备链表
static struct device* devices = 0;

// 驱动程序链表
static struct device_driver* drivers = 0;

// 初始化设备管理子系统
void device_init() {
    print_string("Initializing device management subsystem...\n");
    
    // 初始化PCI子系统
    pci_init();
    
    print_string("Device management subsystem initialized.\n");
}

// 注册设备
int device_register(struct device* dev) {
    if (!dev) {
        return -1;
    }
    
    // 检查设备是否已存在
    struct device* current = devices;
    while (current) {
        if (strcmp(current->name, dev->name) == 0) {
            print_string("Error: Device ");
            print_string(dev->name);
            print_string(" already exists.\n");
            return -1;
        }
        current = current->next;
    }
    
    // 尝试匹配驱动程序
    struct device_driver* driver = driver_match(dev);
    if (driver) {
        print_string("Found driver ");
        print_string(driver->name);
        print_string(" for device ");
        print_string(dev->name);
        print_string(".\n");
        
        // 调用驱动程序的attach函数
        if (driver->attach && driver->attach(dev) != 0) {
            print_string("Error: Failed to attach driver to device ");
            print_string(dev->name);
            print_string(".\n");
            return -1;
        }
    } else {
        print_string("No driver found for device ");
        print_string(dev->name);
        print_string(".\n");
    }
    
    // 添加到设备链表
    dev->next = devices;
    devices = dev;
    
    print_string("Device ");
    print_string(dev->name);
    print_string(" registered.\n");
    
    return 0;
}

// 注销设备
int device_unregister(const char* name) {
    if (!name || !devices) {
        return -1;
    }
    
    // 如果是第一个设备
    if (strcmp(devices->name, name) == 0) {
        struct device* temp = devices;
        devices = devices->next;
        // 在实际实现中，可能需要释放资源
        return 0;
    }
    
    // 查找设备
    struct device* current = devices;
    while (current->next) {
        if (strcmp(current->next->name, name) == 0) {
            struct device* temp = current->next;
            current->next = temp->next;
            // 在实际实现中，可能需要释放资源
            return 0;
        }
        current = current->next;
    }
    
    print_string("Error: Device ");
    print_string((char*)name);
    print_string(" not found.\n");
    return -1;
}

// 根据名称获取设备
struct device* device_get(const char* name) {
    struct device* current = devices;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return 0;
}

// 根据类型获取设备
struct device* device_get_by_type(unsigned int type) {
    struct device* current = devices;
    while (current) {
        if (current->type == type) {
            return current;
        }
        current = current->next;
    }
    return 0;
}

// 列出所有设备
void device_list() {
    print_string("Devices:\n");
    struct device* current = devices;
    while (current) {
        print_string("  ");
        print_string(current->name);
        print_string(" (Type: ");
        char type_str[12];
        int_to_string(current->type, type_str);
        print_string(type_str);
        print_string(")\n");
        current = current->next;
    }
}

// 读取设备数据
int device_read(struct device* dev, unsigned int offset, void* buffer, unsigned int count) {
    if (!dev || !buffer) {
        return -1;
    }
    
    if (dev->read) {
        return dev->read(dev, offset, buffer, count);
    }
    
    print_string("Device ");
    print_string(dev->name);
    print_string(" does not support read operation.\n");
    return -1;
}

// 写入设备数据
int device_write(struct device* dev, unsigned int offset, const void* buffer, unsigned int count) {
    if (!dev || !buffer) {
        return -1;
    }
    
    if (dev->write) {
        return dev->write(dev, offset, buffer, count);
    }
    
    print_string("Device ");
    print_string(dev->name);
    print_string(" does not support write operation.\n");
    return -1;
}

// 设备控制操作
int device_ioctl(struct device* dev, unsigned int command, void* args) {
    if (!dev) {
        return -1;
    }
    
    if (dev->ioctl) {
        return dev->ioctl(dev, command, args);
    }
    
    print_string("Device ");
    print_string(dev->name);
    print_string(" does not support ioctl operation.\n");
    return -1;
}

// 重置设备
int device_reset(struct device* dev) {
    if (!dev) {
        return -1;
    }
    
    if (dev->reset) {
        return dev->reset(dev);
    }
    
    print_string("Device ");
    print_string(dev->name);
    print_string(" does not support reset operation.\n");
    return -1;
}

// 关闭设备
int device_shutdown(struct device* dev) {
    if (!dev) {
        return -1;
    }
    
    if (dev->shutdown) {
        return dev->shutdown(dev);
    }
    
    print_string("Device ");
    print_string(dev->name);
    print_string(" does not support shutdown operation.\n");
    return -1;
}

// 注册驱动程序
int driver_register(struct device_driver* driver) {
    if (!driver) {
        return -1;
    }
    
    // 检查驱动程序是否已存在
    struct device_driver* current = drivers;
    while (current) {
        if (strcmp(current->name, driver->name) == 0) {
            print_string("Error: Driver ");
            print_string(driver->name);
            print_string(" already exists.\n");
            return -1;
        }
        current = current->next;
    }
    
    // 添加到驱动程序链表
    driver->next = drivers;
    drivers = driver;
    
    print_string("Driver ");
    print_string(driver->name);
    print_string(" registered.\n");
    
    return 0;
}

// 注销驱动程序
int driver_unregister(const char* name) {
    if (!name || !drivers) {
        return -1;
    }
    
    // 如果是第一个驱动程序
    if (strcmp(drivers->name, name) == 0) {
        struct device_driver* temp = drivers;
        drivers = drivers->next;
        // 在实际实现中，可能需要释放资源
        return 0;
    }
    
    // 查找驱动程序
    struct device_driver* current = drivers;
    while (current->next) {
        if (strcmp(current->next->name, name) == 0) {
            struct device_driver* temp = current->next;
            current->next = temp->next;
            // 在实际实现中，可能需要释放资源
            return 0;
        }
        current = current->next;
    }
    
    print_string("Error: Driver ");
    print_string((char*)name);
    print_string(" not found.\n");
    return -1;
}

// 根据名称获取驱动程序
struct device_driver* driver_get(const char* name) {
    struct device_driver* current = drivers;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return 0;
}

// 匹配设备和驱动程序
struct device_driver* driver_match(struct device* dev) {
    struct device_driver* current = drivers;
    while (current) {
        // 检查设备类型是否匹配
        if (current->type == dev->type) {
            // 如果驱动程序指定了特定的厂商ID和设备ID，则进行匹配
            if ((current->vendor_id == 0 || current->vendor_id == dev->vendor_id) &&
                (current->device_id == 0 || current->device_id == dev->device_id)) {
                return current;
            }
        }
        current = current->next;
    }
    return 0;
}

// 初始化PCI子系统
void pci_init() {
    print_string("Initializing PCI subsystem...\n");
    
    // 在实际实现中，这里会枚举所有PCI设备
    pci_enumerate_devices();
    
    print_string("PCI subsystem initialized.\n");
}

// 查找PCI设备
struct pci_device* pci_find_device(unsigned short vendor_id, unsigned short device_id) {
    // 在实际实现中，这里会查找指定的PCI设备
    return 0;
}

// 根据类别查找PCI设备
struct pci_device* pci_find_class(unsigned char class_code, unsigned char subclass) {
    // 在实际实现中，这里会查找指定类别的PCI设备
    return 0;
}

// 读取PCI配置空间字节
void pci_read_config_byte(struct pci_device* dev, unsigned char offset, unsigned char* value) {
    // 在实际实现中，这里会读取PCI配置空间
}

// 读取PCI配置空间字
void pci_read_config_word(struct pci_device* dev, unsigned char offset, unsigned short* value) {
    // 在实际实现中，这里会读取PCI配置空间
}

// 读取PCI配置空间双字
void pci_read_config_dword(struct pci_device* dev, unsigned char offset, unsigned int* value) {
    // 在实际实现中，这里会读取PCI配置空间
}

// 写入PCI配置空间字节
void pci_write_config_byte(struct pci_device* dev, unsigned char offset, unsigned char value) {
    // 在实际实现中，这里会写入PCI配置空间
}

// 写入PCI配置空间字
void pci_write_config_word(struct pci_device* dev, unsigned char offset, unsigned short value) {
    // 在实际实现中，这里会写入PCI配置空间
}

// 写入PCI配置空间双字
void pci_write_config_dword(struct pci_device* dev, unsigned char offset, unsigned int value) {
    // 在实际实现中，这里会写入PCI配置空间
}

// 枚举PCI设备
void pci_enumerate_devices() {
    print_string("Enumerating PCI devices...\n");
    // 在实际实现中，这里会枚举所有PCI设备
}

// 注册中断处理程序
void irq_register(unsigned int irq, void (*handler)(unsigned int irq, void* data), void* data) {
    print_string("Registering IRQ ");
    char irq_str[12];
    int_to_string(irq, irq_str);
    print_string(irq_str);
    print_string(" handler.\n");
    // 在实际实现中，这里会注册中断处理程序
}

// 注销中断处理程序
void irq_unregister(unsigned int irq) {
    print_string("Unregistering IRQ ");
    char irq_str[12];
    int_to_string(irq, irq_str);
    print_string(irq_str);
    print_string(" handler.\n");
    // 在实际实现中，这里会注销中断处理程序
}

// 启用中断
void irq_enable(unsigned int irq) {
    // 在实际实现中，这里会启用指定的中断线
}

// 禁用中断
void irq_disable(unsigned int irq) {
    // 在实际实现中，这里会禁用指定的中断线
}

// 字符串比较函数（简化实现）
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// 整数转字符串辅助函数
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