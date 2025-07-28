#!/bin/bash

# QEMU运行脚本，用于在GitHub Actions中运行系统

set -e  # 遇到错误时停止执行

echo "========================================="
echo "Preparing to run LightweightOS in QEMU..."
echo "========================================="

# 检查是否安装了QEMU
echo "Checking for QEMU installation..."
if ! command -v qemu-system-i386 &> /dev/null
then
    echo "ERROR: qemu-system-i386 could not be found"
    echo "Please install QEMU with: sudo apt-get install qemu-system-x86"
    exit 1
else
    echo "QEMU found: $(qemu-system-i386 --version)"
fi

# 检查必要的文件是否存在
echo "Checking for required files..."
if [ ! -f "boot/boot.bin" ]; then
    echo "ERROR: boot/boot.bin not found"
    ls -la boot/
    exit 1
else
    echo "Found boot/boot.bin ($(stat -c%s boot/boot.bin) bytes)"
fi

if [ ! -f "kernel.bin" ]; then
    echo "ERROR: kernel.bin not found"
    ls -la
    exit 1
else
    echo "Found kernel.bin ($(stat -c%s kernel.bin) bytes)"
fi

echo "All required files found. Creating floppy disk image..."

# 创建软盘镜像
echo "Creating 1.44MB floppy disk image..."
dd if=/dev/zero of=floppy.img bs=1K count=1440 2>/dev/null
echo "Formatting floppy disk image..."
mkfs.msdos floppy.img 2>/dev/null

# 将引导扇区和内核写入镜像
echo "Writing bootloader to floppy image..."
dd if=boot/boot.bin of=floppy.img conv=notrunc 2>/dev/null

echo "Floppy image created successfully ($(stat -c%s floppy.img) bytes)"

echo "========================================="
echo "Running LightweightOS in QEMU..."
echo "========================================="
echo "Note: System will run for 30 seconds or until completion"
echo "Press Ctrl+A followed by X to exit QEMU (if running manually)"

# 使用QEMU运行系统，设置超时以避免无限运行
echo "Starting QEMU with timeout..."
timeout 30s qemu-system-i386 -fda floppy.img -boot a -serial stdio -display none || true

echo "QEMU execution finished or timed out after 30 seconds"

# 清理临时文件
echo "Cleaning up..."
rm -f floppy.img

echo "Done."
echo "========================================="