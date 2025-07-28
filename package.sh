#!/bin/bash

# LightweightOS 打包脚本
# 创建一个包含所有必要文件的发布包

echo "========================================="
echo "LightweightOS 打包脚本"
echo "========================================="

# 确保在项目根目录
cd "$(dirname "$0")"
echo "工作目录: $(pwd)"

# 设置版本信息
VERSION="1.0.0"
PACKAGE_NAME="lightweight-os-v${VERSION}"
PACKAGE_DIR="/tmp/${PACKAGE_NAME}"

echo "创建版本: ${VERSION}"
echo "包名: ${PACKAGE_NAME}"

# 清理之前的打包目录
echo "清理之前的打包目录..."
rm -rf "${PACKAGE_DIR}"
rm -f "${PACKAGE_NAME}.tar.gz"
rm -f "${PACKAGE_NAME}.zip"

# 创建打包目录结构
echo "创建打包目录结构..."
mkdir -p "${PACKAGE_DIR}"

# 复制必要文件和目录
echo "复制文件和目录..."

# 核心代码目录
cp -r boot "${PACKAGE_DIR}/"
cp -r kernel "${PACKAGE_DIR}/"
cp -r drivers "${PACKAGE_DIR}/"
cp -r libs "${PACKAGE_DIR}/"
cp -r userland "${PACKAGE_DIR}/"
cp -r tools "${PACKAGE_DIR}/"

# 构建和运行脚本
cp build.sh "${PACKAGE_DIR}/"
cp run.sh "${PACKAGE_DIR}/"
cp run-qemu.sh "${PACKAGE_DIR}/"
cp push-to-github.sh "${PACKAGE_DIR}/"
cp push-to-github.bat "${PACKAGE_DIR}/"
cp diagnose.sh "${PACKAGE_DIR}/"

# 配置文件
cp Makefile "${PACKAGE_DIR}/"
cp wuying-test-config.json "${PACKAGE_DIR}/"

# 文档
cp -r docs "${PACKAGE_DIR}/"
cp README.md "${PACKAGE_DIR}/"
cp LICENSE "${PACKAGE_DIR}/" 2>/dev/null || echo "注意: LICENSE 文件不存在"

# 测试文件
cp static-analysis.py "${PACKAGE_DIR}/"
cp unit-test-sim.py "${PACKAGE_DIR}/"
cp coverage-analysis.py "${PACKAGE_DIR}/"
cp integration-test.py "${PACKAGE_DIR}/"
cp run-tests.py "${PACKAGE_DIR}/"
cp bug-test.py "${PACKAGE_DIR}/"
cp bug-test-improved.py "${PACKAGE_DIR}/"

# GitHub Actions 工作流
cp -r .github "${PACKAGE_DIR}/"

# 设置执行权限
echo "设置执行权限..."
find "${PACKAGE_DIR}" -name "*.sh" -exec chmod +x {} \;

# 创建版本文件
echo "创建版本信息文件..."
cat > "${PACKAGE_DIR}/VERSION" << EOF
LightweightOS 版本信息
==================

版本: ${VERSION}
打包日期: $(date)
EOF

# 创建打包说明
cat > "${PACKAGE_DIR}/PACKAGING.md" << EOF
# LightweightOS 打包说明

这个包包含了构建和运行 LightweightOS 操作系统所需的所有文件。

## 包含内容

- \`boot/\` - 引导程序源代码
- \`kernel/\` - 内核源代码
- \`drivers/\` - 设备驱动程序
- \`libs/\` - 系统库函数
- \`userland/\` - 用户空间程序
- \`tools/\` - 系统工具
- \`docs/\` - 文档
- 构建和运行脚本
- 测试工具
- GitHub Actions 配置

## 构建说明

1. 确保安装了必要的工具：
   - \`nasm\` (汇编器)
   - \`gcc\` (C 编译器)
   - \`make\` (构建工具)

2. 运行构建脚本：
   \`\`\`bash
   chmod +x build.sh
   ./build.sh
   \`\`\`

3. 运行系统：
   \`\`\`bash
   chmod +x run.sh
   ./run.sh
   \`\`\`

## 目录结构

\`\`\`
${PACKAGE_NAME}/
├── boot/          # 启动引导程序
├── kernel/        # 内核核心代码
├── drivers/       # 设备驱动程序
├── libs/          # 系统库函数
├── userland/      # 用户空间程序
├── tools/         # 系统工具
├── docs/          # 文档
├── tests/         # 测试代码
└── Makefile       # 构建配置
\`\`\`

版本: ${VERSION}
打包日期: $(date)
EOF

# 创建 tar.gz 包
echo "创建 tar.gz 包..."
tar -czf "${PACKAGE_NAME}.tar.gz" -C /tmp "${PACKAGE_NAME}"

# 创建 zip 包
echo "创建 zip 包..."
cd /tmp
zip -r "${PACKAGE_NAME}.zip" "${PACKAGE_NAME}" > /dev/null
mv "${PACKAGE_NAME}.zip" "$(pwd)/../${PACKAGE_NAME}.zip"
cd -

# 显示包信息
echo "========================================="
echo "打包完成!"
echo "包信息:"
echo "  TAR.GZ 包: ${PACKAGE_NAME}.tar.gz ($(stat -c%s "${PACKAGE_NAME}.tar.gz" 2>/dev/null || echo '未知') 字节)"
echo "  ZIP 包:    ${PACKAGE_NAME}.zip ($(stat -c%s "${PACKAGE_NAME}.zip" 2>/dev/null || echo '未知') 字节)"
echo ""
echo "文件列表:"
ls -lh "${PACKAGE_NAME}".*
echo "========================================="

# 清理临时目录
echo "清理临时目录..."
rm -rf "${PACKAGE_DIR}"

echo "打包脚本执行完成!"