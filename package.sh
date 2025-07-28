#!/bin/bash

# LightweightOS 多平台打包脚本
# 支持创建 ZIP 和 tar.gz 格式的发布包

echo "========================================="
echo "LightweightOS 多平台打包脚本"
echo "========================================="

# 确保在项目根目录
cd "$(dirname "$0")"
echo "工作目录: $(pwd)"

# 检查必要工具
echo "检查必要工具..."
MISSING_TOOLS=()

if ! command -v zip &> /dev/null; then
    echo "警告: zip 工具未安装"
    MISSING_TOOLS+=("zip")
fi

if ! command -v tar &> /dev/null; then
    echo "警告: tar 工具未安装"
    MISSING_TOOLS+=("tar")
fi

if [ ${#MISSING_TOOLS[@]} -gt 0 ]; then
    echo "缺少工具: ${MISSING_TOOLS[*]}"
    echo "请安装这些工具以创建完整的发布包"
fi

# 设置版本信息
VERSION="1.0.0"
PACKAGE_NAME="lightweight-os-v${VERSION}"

echo "创建版本: ${VERSION}"
echo "包名: ${PACKAGE_NAME}"

# 清理之前的打包文件
echo "清理之前的打包文件..."
rm -f "${PACKAGE_NAME}.tar.gz"
rm -f "${PACKAGE_NAME}.zip"

# 创建临时目录
TEMP_DIR="/tmp/${PACKAGE_NAME}"
echo "创建临时目录: ${TEMP_DIR}"
rm -rf "${TEMP_DIR}"
mkdir -p "${TEMP_DIR}"

# 复制必要文件和目录
echo "复制文件和目录..."

# 核心代码目录
cp -r boot "${TEMP_DIR}/"
cp -r kernel "${TEMP_DIR}/"
cp -r drivers "${TEMP_DIR}/"
cp -r libs "${TEMP_DIR}/"
cp -r userland "${TEMP_DIR}/"
cp -r tools "${TEMP_DIR}/"

# 构建和运行脚本
cp build.sh "${TEMP_DIR}/"
cp run.sh "${TEMP_DIR}/"
cp run-qemu.sh "${TEMP_DIR}/"
cp push-to-github.sh "${TEMP_DIR}/"
cp push-to-github.bat "${TEMP_DIR}/"
cp diagnose.sh "${TEMP_DIR}/"
cp package.sh "${TEMP_DIR}/"
cp package.bat "${TEMP_DIR}/"
cp package.ps1 "${TEMP_DIR}/"

# 配置文件
cp Makefile "${TEMP_DIR}/"
cp wuying-test-config.json "${TEMP_DIR}/"

# 文档
cp -r docs "${TEMP_DIR}/"
cp README.md "${TEMP_DIR}/"
cp RELEASE.md "${TEMP_DIR}/"
cp LICENSE "${TEMP_DIR}/" 2>/dev/null || echo "注意: LICENSE 文件不存在"

# 测试文件
cp static-analysis.py "${TEMP_DIR}/"
cp unit-test-sim.py "${TEMP_DIR}/"
cp coverage-analysis.py "${TEMP_DIR}/"
cp integration-test.py "${TEMP_DIR}/"
cp run-tests.py "${TEMP_DIR}/"
cp bug-test.py "${TEMP_DIR}/"
cp bug-test-improved.py "${TEMP_DIR}/"

# GitHub Actions 工作流
cp -r .github "${TEMP_DIR}/"

# 创建版本文件
echo "创建版本信息文件..."
cat > "${TEMP_DIR}/VERSION" << EOF
LightweightOS 版本信息
==================

版本: ${VERSION}
打包日期: $(date)
EOF

# 设置执行权限
echo "设置执行权限..."
find "${TEMP_DIR}" -name "*.sh" -exec chmod +x {} \;
find "${TEMP_DIR}" -name "*.py" -exec chmod +x {} \;

# 创建 tar.gz 包
if command -v tar &> /dev/null; then
    echo "创建 tar.gz 包..."
    tar -czf "${PACKAGE_NAME}.tar.gz" -C /tmp "${PACKAGE_NAME}"
    echo "tar.gz 包创建完成: ${PACKAGE_NAME}.tar.gz ($(stat -c%s "${PACKAGE_NAME}.tar.gz" 2>/dev/null || echo '未知') 字节)"
else
    echo "跳过 tar.gz 包创建 (缺少 tar 工具)"
fi

# 创建 zip 包
if command -v zip &> /dev/null; then
    echo "创建 zip 包..."
    cd /tmp
    zip -r "${PACKAGE_NAME}.zip" "${PACKAGE_NAME}" > /dev/null
    mv "${PACKAGE_NAME}.zip" "$(pwd)/../${PACKAGE_NAME}.zip"
    cd -
    echo "zip 包创建完成: ${PACKAGE_NAME}.zip ($(stat -c%s "${PACKAGE_NAME}.zip" 2>/dev/null || echo '未知') 字节)"
else
    echo "跳过 zip 包创建 (缺少 zip 工具)"
fi

# 显示包信息
echo "========================================="
echo "打包完成!"
echo "生成的包:"
if [ -f "${PACKAGE_NAME}.tar.gz" ]; then
    echo "  TAR.GZ 包: ${PACKAGE_NAME}.tar.gz ($(stat -c%s "${PACKAGE_NAME}.tar.gz" 2>/dev/null || echo '未知') 字节)"
fi
if [ -f "${PACKAGE_NAME}.zip" ]; then
    echo "  ZIP 包: ${PACKAGE_NAME}.zip ($(stat -c%s "${PACKAGE_NAME}.zip" 2>/dev/null || echo '未知') 字节)"
fi
echo ""
echo "文件列表:"
ls -lh "${PACKAGE_NAME}".* 2>/dev/null || echo "没有生成包文件"
echo "========================================="

# 清理临时目录
echo "清理临时目录..."
rm -rf "${TEMP_DIR}"

echo "打包脚本执行完成!"