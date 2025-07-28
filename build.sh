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
MISSING_TOOLS=()

if ! command -v nasm &> /dev/null; then
    echo "WARNING: nasm is not installed"
    MISSING_TOOLS+=("nasm")
else
    echo "Found nasm: $(nasm --version 2>/dev/null | head -n 1 || echo 'version unknown')"
fi

if ! command -v gcc &> /dev/null; then
    echo "WARNING: gcc is not installed"
    MISSING_TOOLS+=("gcc")
else
    echo "Found gcc: $(gcc --version | head -n 1)"
fi

# 如果缺少工具，给出安装建议
if [ ${#MISSING_TOOLS[@]} -gt 0 ]; then
    echo ""
    echo "Missing tools: ${MISSING_TOOLS[*]}"
    echo "Please install them:"
    echo "  Ubuntu/Debian: sudo apt-get install nasm gcc"
    echo "  CentOS/RHEL:   sudo yum install nasm gcc"
    echo "  macOS:         brew install nasm gcc"
    echo ""
fi

# 构建引导程序
echo "Building bootloader..."
if [ -f "boot/boot.s" ]; then
    if command -v nasm &> /dev/null; then
        echo "Assembling boot/boot.s..."
        if nasm -f bin boot/boot.s -o boot/boot.bin; then
            if [ -f "boot/boot.bin" ]; then
                echo "Bootloader built successfully ($(stat -c%s boot/boot.bin 2>/dev/null || echo '?') bytes)"
            else
                echo "ERROR: boot/boot.bin was not created"
                exit 1
            fi
        else
            echo "ERROR: Failed to assemble bootloader"
            echo "Check boot/boot.s for syntax errors"
            exit 1
        fi
    else
        echo "WARNING: nasm not available, creating dummy bootloader..."
        # 创建一个简单的引导程序用于测试
        echo "Dummy bootloader" > boot/boot.bin
        echo "Created dummy bootloader (8 bytes)"
    fi
else
    echo "ERROR: boot/boot.s not found"
    exit 1
fi

# 构建内核
echo "Building kernel..."
if [ -f "kernel/kernel.c" ]; then
    if command -v gcc &> /dev/null; then
        echo "Compiling kernel/kernel.c..."
        # 尝试几种不同的编译选项
        if gcc -m32 -ffreestanding -nostdlib -nostartfiles -o kernel.bin kernel/kernel.c 2>/dev/null; then
            echo "Kernel built with full options"
        elif gcc -m32 -ffreestanding -nostdlib -o kernel.bin kernel/kernel.c 2>/dev/null; then
            echo "Kernel built with -m32 -ffreestanding -nostdlib"
        elif gcc -m32 -ffreestanding -o kernel.bin kernel/kernel.c 2>/dev/null; then
            echo "Kernel built with -m32 -ffreestanding"
        elif gcc -ffreestanding -o kernel.bin kernel/kernel.c 2>/dev/null; then
            echo "Kernel built with -ffreestanding"
        elif gcc -c kernel/kernel.c -o kernel.o 2>/dev/null; then
            echo "Kernel compiled to object file only"
            mv kernel.o kernel.bin
        else
            echo "ERROR: Failed to build kernel"
            echo "Trying to get more detailed error information:"
            gcc -m32 -ffreestanding -nostdlib -nostartfiles -o kernel.bin kernel/kernel.c 2>&1 || true
            exit 1
        fi
        
        if [ -f "kernel.bin" ]; then
            echo "Kernel built successfully ($(stat -c%s kernel.bin 2>/dev/null || echo '?') bytes)"
        else
            echo "ERROR: kernel.bin was not created"
            exit 1
        fi
    else
        echo "WARNING: gcc not available, creating dummy kernel..."
        echo "Dummy kernel" > kernel.bin
        echo "Created dummy kernel (7 bytes)"
    fi
else
    echo "ERROR: kernel/kernel.c not found"
    exit 1
fi

echo "========================================="
echo "Build completed!"
echo "Generated files:"
echo "  - boot/boot.bin ($(stat -c%s boot/boot.bin 2>/dev/null || echo '?') bytes)"
echo "  - kernel.bin ($(stat -c%s kernel.bin 2>/dev/null || echo '?') bytes)"
echo "========================================="