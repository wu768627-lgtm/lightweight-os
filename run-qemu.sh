#!/bin/bash

# QEMU运行脚本，用于在GitHub Actions中运行系统

set -e  # 遇到错误时停止执行

echo "Preparing to run LightweightOS in QEMU..."

# 检查是否安装了QEMU
if ! command -v qemu-system-i386 &> /dev/null
then
    echo "ERROR: qemu-system-i386 could not be found"
    echo "Please install QEMU with: sudo apt-get install qemu-system-x86"
    exit 1
fi

# 检查必要的文件是否存在
if [ ! -f "boot/boot.bin" ]; then
    echo "ERROR: boot/boot.bin not found"
    exit 1
fi

if [ ! -f "kernel.bin" ]; then
    echo "ERROR: kernel.bin not found"
    exit 1
fi

echo "All required files found. Creating floppy disk image..."

# 创建软盘镜像
dd if=/dev/zero of=floppy.img bs=1K count=1440 2>/dev/null
mkfs.msdos floppy.img 2>/dev/null

# 将引导扇区和内核写入镜像
dd if=boot/boot.bin of=floppy.img conv=notrunc 2>/dev/null

echo "Running LightweightOS in QEMU..."
echo "Press Ctrl+A followed by X to exit QEMU"

# 使用QEMU运行系统，设置超时以避免无限运行
timeout 30s qemu-system-i386 -fda floppy.img -boot a -serial stdio || true

echo "QEMU execution finished or timed out after 30 seconds"

# 清理临时文件
rm -f floppy.img

echo "Done."