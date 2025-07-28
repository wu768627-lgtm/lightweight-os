# LightweightOS Makefile

# 编译器和工具
CC = gcc
AS = as
LD = ld
OBJCOPY = objcopy

# 编译选项
CFLAGS = -m32 -nostdlib -fno-builtin -Wall -Wextra -O2 -I./libs -I./kernel -I./drivers
ASFLAGS = --32
LDFLAGS = -m elf_i386

# 目录
KERNEL_DIR = kernel
BOOT_DIR = boot
DRIVERS_DIR = drivers
LIBS_DIR = libs
USERLAND_DIR = userland
TOOLS_DIR = tools
BUILD_DIR = build

# 内核源文件
KERNEL_SOURCES = $(KERNEL_DIR)/kernel.c $(KERNEL_DIR)/memory.c $(KERNEL_DIR)/process.c $(KERNEL_DIR)/interrupts.c $(KERNEL_DIR)/syscall.c $(KERNEL_DIR)/profiling.c $(KERNEL_DIR)/security.c $(KERNEL_DIR)/vm.c $(KERNEL_DIR)/scheduler.c $(KERNEL_DIR)/logger.c $(KERNEL_DIR)/config.c $(KERNEL_DIR)/exception.c $(KERNEL_DIR)/power.c $(KERNEL_DIR)/test.c
KERNEL_OBJECTS = $(KERNEL_SOURCES:.c=.o)

# 驱动源文件
DRIVERS_SOURCES = $(DRIVERS_DIR)/filesystem.c $(DRIVERS_DIR)/network.c $(DRIVERS_DIR)/device.c $(DRIVERS_DIR)/graphics.c $(DRIVERS_DIR)/fat.c $(DRIVERS_DIR)/tcp.c $(DRIVERS_DIR)/keyboard.c
DRIVERS_OBJECTS = $(DRIVERS_SOURCES:.c=.o)

# 库源文件
LIBS_SOURCES = $(LIBS_DIR)/stdlib.c $(LIBS_DIR)/string.c
LIBS_OBJECTS = $(LIBS_SOURCES:.c=.o)

# 用户空间源文件
USERLAND_SOURCES = $(USERLAND_DIR)/init.c $(USERLAND_DIR)/editor.c $(USERLAND_DIR)/shell.c $(USERLAND_DIR)/svc.c $(USERLAND_DIR)/test.c
USERLAND_OBJECTS = $(USERLAND_SOURCES:.c=.o)

# 工具源文件
TOOLS_SOURCES = $(TOOLS_DIR)/sysmon.c $(TOOLS_DIR)/stress.c $(TOOLS_DIR)/integrity.c
TOOLS_OBJECTS = $(TOOLS_SOURCES:.c=.o)

# 引导程序源文件
BOOT_SOURCE = $(BOOT_DIR)/boot.s
BOOT_OBJECT = $(BOOT_DIR)/boot.o

# 目标文件
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
BOOT_BIN = $(BUILD_DIR)/boot.bin
OS_IMAGE = $(BUILD_DIR)/lightweightos.img

# 默认目标
all: directories $(OS_IMAGE)

# 创建构建目录
directories:
	mkdir -p $(BUILD_DIR)

# 编译内核
$(KERNEL_BIN): $(KERNEL_SOURCES) $(DRIVERS_SOURCES) $(LIBS_SOURCES)
	$(CC) $(CFLAGS) -c $(KERNEL_SOURCES) $(DRIVERS_SOURCES) $(LIBS_SOURCES)
	$(LD) $(LDFLAGS) -Ttext 0x10000 -o $(KERNEL_BIN) $(KERNEL_OBJECTS) $(DRIVERS_OBJECTS) $(LIBS_OBJECTS) --oformat binary

# 编译引导程序
$(BOOT_BIN): $(BOOT_SOURCE)
	$(AS) $(ASFLAGS) -o $(BOOT_OBJECT) $(BOOT_SOURCE)
	$(LD) $(LDFLAGS) -Ttext 0x7C00 -o $(BUILD_DIR)/boot.elf $(BOOT_OBJECT)
	$(OBJCOPY) -O binary $(BUILD_DIR)/boot.elf $(BOOT_BIN)

# 创建系统镜像
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	dd if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880 2>/dev/null
	dd if=$(BOOT_BIN) of=$(OS_IMAGE) bs=512 count=1 conv=notrunc 2>/dev/null

# 编译用户空间程序
userland: $(USERLAND_SOURCES) $(LIBS_SOURCES) $(TOOLS_SOURCES)
	$(CC) $(CFLAGS) -c $(USERLAND_SOURCES) $(LIBS_SOURCES) $(TOOLS_SOURCES)
	$(LD) $(LDFLAGS) -Ttext 0x400000 -o $(BUILD_DIR)/init.bin $(USERLAND_DIR)/init.o $(LIBS_OBJECTS) --oformat binary
	$(LD) $(LDFLAGS) -Ttext 0x410000 -o $(BUILD_DIR)/editor.bin $(USERLAND_DIR)/editor.o $(LIBS_OBJECTS) --oformat binary
	$(LD) $(LDFLAGS) -Ttext 0x420000 -o $(BUILD_DIR)/shell.bin $(USERLAND_DIR)/shell.o $(LIBS_OBJECTS) --oformat binary
	$(LD) $(LDFLAGS) -Ttext 0x430000 -o $(BUILD_DIR)/svc.bin $(USERLAND_DIR)/svc.o $(LIBS_OBJECTS) --oformat binary
	$(LD) $(LDFLAGS) -Ttext 0x440000 -o $(BUILD_DIR)/test.bin $(USERLAND_DIR)/test.o $(LIBS_OBJECTS) --oformat binary
	$(LD) $(LDFLAGS) -Ttext 0x450000 -o $(BUILD_DIR)/sysmon.bin $(TOOLS_DIR)/sysmon.o $(LIBS_OBJECTS) --oformat binary
	$(LD) $(LDFLAGS) -Ttext 0x460000 -o $(BUILD_DIR)/stress.bin $(TOOLS_DIR)/stress.o $(LIBS_OBJECTS) --oformat binary
	$(LD) $(LDFLAGS) -Ttext 0x470000 -o $(BUILD_DIR)/integrity.bin $(TOOLS_DIR)/integrity.o $(LIBS_OBJECTS) --oformat binary

# 清理构建文件
clean:
	rm -f $(KERNEL_OBJECTS) $(DRIVERS_OBJECTS) $(LIBS_OBJECTS) $(USERLAND_OBJECTS) $(TOOLS_OBJECTS) $(BOOT_OBJECT) $(BUILD_DIR)/*.elf $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.img
	rmdir $(BUILD_DIR) 2>/dev/null || true

# 运行系统 (需要安装QEMU)
run: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE)

# 调试系统
debug: $(OS_IMAGE)
	qemu-system-i386 -s -S -fda $(OS_IMAGE) &

.PHONY: all clean run debug directories userland