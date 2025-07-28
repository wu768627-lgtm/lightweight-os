#!/bin/bash

# LightweightOS 诊断脚本
# 用于识别可能导致构建失败的问题

echo "========================================="
echo "LightweightOS 诊断脚本"
echo "========================================="

# 确保在项目根目录
cd "$(dirname "$0")"
echo "工作目录: $(pwd)"
echo ""

# 检查目录结构
echo "1. 检查目录结构..."
echo "根目录内容:"
ls -la
echo ""

if [ ! -d "boot" ]; then
    echo "错误: 找不到 boot 目录"
    exit 1
fi

if [ ! -d "kernel" ]; then
    echo "错误: 找不到 kernel 目录"
    exit 1
fi

echo "boot 目录内容:"
ls -la boot/
echo ""

echo "kernel 目录内容:"
ls -la kernel/
echo ""

# 检查必要文件
echo "2. 检查必要文件..."

REQUIRED_FILES=(
    "boot/boot.s"
    "kernel/kernel.c"
    "kernel/memory.c"
    "kernel/process.c"
    "kernel/interrupts.c"
    "kernel/syscall.c"
)

MISSING_FILES=()

for file in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo "错误: 找不到文件 $file"
        MISSING_FILES+=("$file")
    else
        echo "找到: $file ($(stat -c%s "$file") 字节)"
    fi
done

if [ ${#MISSING_FILES[@]} -gt 0 ]; then
    echo "缺少 ${#MISSING_FILES[@]} 个必要文件"
    exit 1
fi

echo ""

# 检查工具
echo "3. 检查必要工具..."

TOOLS=("nasm" "gcc" "ld" "make")
MISSING_TOOLS=()

for tool in "${TOOLS[@]}"; do
    if ! command -v "$tool" &> /dev/null; then
        echo "错误: 找不到工具 $tool"
        MISSING_TOOLS+=("$tool")
    else
        echo "找到: $tool ($($tool --version | head -n 1))"
    fi
done

if [ ${#MISSING_TOOLS[@]} -gt 0 ]; then
    echo "缺少 ${#MISSING_TOOLS[@]} 个必要工具"
    exit 1
fi

echo ""

# 检查引导程序
echo "4. 检查引导程序..."

if [ -f "boot/boot.s" ]; then
    echo "boot.s 文件内容预览:"
    head -n 20 boot/boot.s
    echo ""
    
    echo "尝试汇编引导程序..."
    nasm -f bin boot/boot.s -o boot/boot.bin
    if [ $? -eq 0 ] && [ -f "boot/boot.bin" ]; then
        echo "引导程序汇编成功 ($(stat -c%s boot/boot.bin) 字节)"
    else
        echo "引导程序汇编失败"
        exit 1
    fi
else
    echo "错误: 找不到 boot/boot.s"
    exit 1
fi

echo ""

# 检查内核
echo "5. 检查内核..."

if [ -f "kernel/kernel.c" ]; then
    echo "kernel.c 文件内容预览:"
    head -n 30 kernel/kernel.c
    echo ""
    
    echo "尝试编译内核(仅编译成对象文件)..."
    gcc -c kernel/kernel.c -o kernel.o 2>kernel_compile.log
    if [ $? -eq 0 ] && [ -f "kernel.o" ]; then
        echo "内核编译成对象文件成功 ($(stat -c%s kernel.o) 字节)"
        rm kernel.o
    else
        echo "内核编译成对象文件失败"
        echo "错误日志:"
        cat kernel_compile.log
        exit 1
    fi
    
    echo ""
    echo "尝试完整编译内核..."
    gcc -m32 -ffreestanding -nostdlib -nostartfiles -o kernel.bin kernel/kernel.c 2>kernel_link.log
    if [ $? -eq 0 ] && [ -f "kernel.bin" ]; then
        echo "内核完整编译成功 ($(stat -c%s kernel.bin) 字节)"
    else
        echo "内核完整编译失败"
        echo "错误日志:"
        cat kernel_link.log
        exit 1
    fi
else
    echo "错误: 找不到 kernel/kernel.c"
    exit 1
fi

echo ""

# 检查文件权限
echo "6. 检查文件权限..."

SCRIPTS=("build.sh" "run.sh" "run-qemu.sh")
for script in "${SCRIPTS[@]}"; do
    if [ -f "$script" ]; then
        if [ -x "$script" ]; then
            echo "$script: 可执行"
        else
            echo "$script: 不可执行 (chmod +x $script 可修复)"
        fi
    else
        echo "$script: 未找到"
    fi
done

echo ""

# 检查编码问题
echo "7. 检查文件编码..."

FILES_TO_CHECK=("boot/boot.s" "kernel/kernel.c" "build.sh")
for file in "${FILES_TO_CHECK[@]}"; do
    if [ -f "$file" ]; then
        encoding=$(file -b --mime-encoding "$file")
        echo "$file: $encoding"
    fi
done

echo ""

echo "========================================="
echo "诊断完成 - 未发现明显问题"
echo "========================================="