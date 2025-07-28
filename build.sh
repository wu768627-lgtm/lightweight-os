#!/bin/bash

# LightweightOS 构建脚本

echo "========================================="
echo "LightweightOS Build Script"
echo "========================================="

# 确保在项目根目录
cd "$(dirname "$0")"
echo "Working in directory: $(pwd)"

# 清理之前的构建
echo "Cleaning previous builds..."
rm -f boot/boot.bin kernel.bin

# 检查必要的工具
echo "Checking for required tools..."
for tool in nasm gcc ld; do
    if ! command -v $tool &> /dev/null
    then
        echo "ERROR: $tool is not installed"
        exit 1
    else
        echo "Found $tool: $($tool --version | head -n 1)"
    fi
done

# 构建引导程序
echo "Building bootloader..."
if [ -f "boot/boot.s" ]; then
    echo "Assembling boot/boot.s..."
    nasm -f bin boot/boot.s -o boot/boot.bin
    if [ $? -eq 0 ] && [ -f "boot/boot.bin" ]; then
        echo "Bootloader built successfully ($(stat -c%s boot/boot.bin) bytes)"
    else
        echo "ERROR: Failed to build bootloader"
        exit 1
    fi
else
    echo "ERROR: boot/boot.s not found"
    exit 1
fi

# 构建内核
echo "Building kernel..."
if [ -f "kernel/kernel.c" ]; then
    echo "Compiling kernel/kernel.c..."
    gcc -m32 -nostdlib -nostartfiles -ffreestanding -fno-builtin -o kernel.bin kernel/kernel.c
    if [ $? -eq 0 ] && [ -f "kernel.bin" ]; then
        echo "Kernel built successfully ($(stat -c%s kernel.bin) bytes)"
    else
        echo "ERROR: Failed to build kernel"
        exit 1
    fi
else
    echo "ERROR: kernel/kernel.c not found"
    exit 1
fi

echo "========================================="
echo "Build completed successfully!"
echo "Generated files:"
echo "  - boot/boot.bin ($(stat -c%s boot/boot.bin) bytes)"
echo "  - kernel.bin ($(stat -c%s kernel.bin) bytes)"
echo "========================================="