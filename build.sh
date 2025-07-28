#!/bin/bash

# LightweightOS 构建脚本

echo "Building LightweightOS..."

# 创建构建目录
mkdir -p build

# 检查工具链
echo "Checking toolchain..."
if ! command -v gcc &> /dev/null
then
    echo "Error: gcc not found"
    exit 1
fi

if ! command -v make &> /dev/null
then
    echo "Error: make not found"
    exit 1
fi

if ! command -v nasm &> /dev/null
then
    echo "Error: nasm not found"
    exit 1
fi

if ! command -v ld &> /dev/null
then
    echo "Error: ld not found"
    exit 1
fi

if ! command -v objcopy &> /dev/null
then
    echo "Error: objcopy not found"
    exit 1
fi

echo "Toolchain check passed."

# 构建项目
echo "Building project..."
make clean
make all

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"

# 构建用户空间程序
echo "Building userland..."
make userland

if [ $? -ne 0 ]; then
    echo "Userland build failed!"
    exit 1
fi

echo "Userland build successful!"

# 检查生成的文件
echo "Checking generated files..."
if [ -f build/lightweightos.img ]; then
    echo "System image: build/lightweightos.img"
    ls -lh build/lightweightos.img
else
    echo "Error: System image not found!"
    exit 1
fi

echo "Build completed successfully!"
echo "To run the OS, use: make run"