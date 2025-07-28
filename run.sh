#!/bin/bash

# LightweightOS 运行脚本

echo "Running LightweightOS..."

# 检查QEMU
echo "Checking for QEMU..."
if ! command -v qemu-system-i386 &> /dev/null
then
    echo "Error: qemu-system-i386 not found"
    echo "Please install QEMU to run the OS:"
    echo "  Ubuntu/Debian: sudo apt-get install qemu-system-x86"
    echo "  CentOS/RHEL: sudo yum install qemu-system-x86"
    echo "  macOS: brew install qemu"
    echo "  Windows: Download from https://www.qemu.org/download/"
    exit 1
fi

echo "QEMU found."

# 检查系统镜像
if [ ! -f build/lightweightos.img ]; then
    echo "Error: System image not found!"
    echo "Please build the OS first using: ./build.sh"
    exit 1
fi

echo "System image found."

# 运行系统
echo "Starting LightweightOS in QEMU..."
echo "Press Ctrl+A then X to exit QEMU"

qemu-system-i386 -fda build/lightweightos.img

echo "LightweightOS session ended."