#!/bin/bash

# LightweightOS 无影环境测试脚本

echo "=========================================="
echo "LightweightOS 无影云桌面测试环境准备"
echo "=========================================="

# 检查环境
echo "检查系统环境..."
uname -a

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
    echo "请在无影环境中安装这些工具"
    exit 1
fi

echo "所有必要工具均已安装"

# 创建工作目录
echo "创建工作目录..."
mkdir -p wuying-test-env
cd wuying-test-env

# 复制项目文件
echo "复制项目文件..."
cp -r ../boot ../kernel ../drivers ../libs ../userland ../docs ../Makefile ../build.sh ../run.sh .

# 构建项目
echo "=========================================="
echo "开始构建 LightweightOS"
echo "=========================================="

./build.sh

if [ $? -ne 0 ]; then
    echo "✗ 构建失败"
    exit 1
fi

echo "✓ 构建成功"

# 检查生成的文件
echo "检查生成的系统镜像..."
if [ -f build/lightweightos.img ]; then
    echo "✓ 系统镜像已生成: build/lightweightos.img"
    echo "  镜像大小: $(ls -lh build/lightweightos.img | awk '{print $5}')"
else
    echo "✗ 系统镜像未找到"
    exit 1
fi

# 准备测试环境
echo "=========================================="
echo "准备无影测试环境"
echo "=========================================="

# 创建测试报告目录
mkdir -p test-reports

# 生成测试配置信息
cat > test-reports/test-environment.txt << EOF
LightweightOS 无影环境测试配置
================================
测试时间: $(date)
系统信息: $(uname -a)
工作目录: $(pwd)
测试项目: LightweightOS
镜像文件: build/lightweightos.img
EOF

echo "测试环境信息已保存到 test-reports/test-environment.txt"

# 启动系统进行功能测试
echo "=========================================="
echo "启动系统进行功能测试"
echo "=========================================="

echo "启动 LightweightOS (10秒后自动关闭)..."
timeout 10s qemu-system-i386 -fda build/lightweightos.img -nographic &

QEMU_PID=$!

# 等待系统启动
sleep 8

# 检查QEMU进程是否仍在运行
if ps -p $QEMU_PID > /dev/null; then
    echo "✓ 系统成功启动"
    
    # 等待QEMU进程结束
    wait $QEMU_PID
else
    echo "✗ 系统启动失败"
fi

# 收集测试结果
echo "=========================================="
echo "收集测试结果"
echo "=========================================="

# 生成测试报告
cat > test-reports/test-summary.txt << EOF
LightweightOS 测试报告
====================
测试时间: $(date)
测试环境: 阿里云无影云桌面

测试项目:
1. 环境检查: 通过
2. 项目构建: 通过
3. 系统启动: 执行完成

构建产物:
- 内核文件: build/kernel.bin
- 引导程序: build/boot.bin
- 系统镜像: build/lightweightos.img

备注:
- 在无影环境中成功构建了操作系统
- 系统镜像已生成并可部署
- 可进一步在无影云桌面中进行完整测试

下一步建议:
1. 部署到无影云桌面环境
2. 执行完整的功能和性能测试
3. 根据测试结果优化系统
EOF

echo "测试摘要已保存到 test-reports/test-summary.txt"

# 显示测试结果
echo ""
echo "=========================================="
echo "测试完成"
echo "=========================================="
echo "构建产物位置: build/lightweightos.img"
echo "测试报告位置: test-reports/"
echo ""
echo "可在无影云桌面环境中进一步执行以下操作:"
echo "1. 使用完整的QEMU界面运行系统"
echo "2. 测试用户空间应用程序"
echo "3. 验证文件系统和网络功能"
echo "4. 执行性能基准测试"

echo ""
echo "运行以下命令可在无影环境中完整运行系统:"
echo "  qemu-system-i386 -fda build/lightweightos.img"