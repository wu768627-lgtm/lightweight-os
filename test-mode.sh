#!/bin/bash

# LightweightOS 测试模式启动脚本

echo "=========================================="
echo "LightweightOS 测试模式"
echo "=========================================="

# 检查必要工具
echo "检查必要工具..."
TOOLS=("gcc" "make" "nasm" "ld" "objcopy" "qemu-system-i386")
MISSING_TOOLS=()

for tool in "${TOOLS[@]}"; do
    if ! command -v $tool &> /dev/null; then
        MISSING_TOOLS+=($tool)
    else
        echo "✓ $tool: 已安装"
    fi
done

if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
    echo "✗ 缺少以下工具: ${MISSING_TOOLS[*]}"
    echo "请安装这些工具后再运行测试"
    exit 1
fi

echo "所有必要工具均已安装"

# 构建项目
echo "=========================================="
echo "构建 LightweightOS"
echo "=========================================="

make clean
make all

if [ $? -ne 0 ]; then
    echo "✗ 内核构建失败"
    exit 1
fi

echo "✓ 内核构建成功"

make userland

if [ $? -ne 0 ]; then
    echo "✗ 用户空间程序构建失败"
    exit 1
fi

echo "✓ 用户空间程序构建成功"

# 创建测试配置
echo "=========================================="
echo "创建测试配置"
echo "=========================================="

# 在实际实现中，这里会修改内核配置以启用测试模式
echo "创建测试模式配置..."
echo "TEST_MODE=1" > build/test_config

echo "测试配置创建完成"

# 启动QEMU进行测试
echo "=========================================="
echo "启动系统进行测试"
echo "=========================================="

echo "启动 LightweightOS (测试模式)..."
echo "系统将在测试完成后自动关闭"

timeout 30s qemu-system-i386 -fda build/lightweightos.img

if [ $? -eq 124 ]; then
    echo "测试超时"
else
    echo "测试完成"
fi

echo "=========================================="
echo "测试结果分析"
echo "=========================================="

echo "请查看QEMU窗口中的测试输出"
echo "完整的测试报告将在系统运行时生成"

# 保存测试日志
echo "保存测试日志..."
echo "测试完成时间: $(date)" > test_results.log
echo "测试环境: $(uname -a)" >> test_results.log
echo "构建状态: 成功" >> test_results.log

echo "测试日志已保存到 test_results.log"

echo ""
echo "如需再次运行测试，请执行: ./test-mode.sh"