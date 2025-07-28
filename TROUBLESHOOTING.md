# 项目故障排除指南

本指南将帮助您解决 LightweightOS 项目可能遇到的各种问题。

## GitHub Actions 构建失败

如果您在 GitHub Actions 中看到构建失败，可能的原因包括：

### 1. 依赖项安装问题

确保所有必要的依赖项都已正确安装：

```yaml
- name: Install dependencies
  run: |
    sudo apt-get update
    sudo apt-get install -y build-essential nasm gcc-multilib qemu-system-x86 dosfstools
```

### 2. 构建脚本权限问题

确保构建脚本具有执行权限：

```bash
chmod +x build.sh
./build.sh
```

### 3. 文件路径问题

确保所有文件路径正确，特别是相对路径：

```bash
# 检查关键文件是否存在
if [ ! -f "boot/boot.bin" ]; then
  echo "ERROR: boot/boot.bin not found"
  exit 1
fi
```

### 4. 构建脚本错误

检查 [build.sh](file:///f%3A/app2/Desktop/kg/lightweight-os/build.sh) 脚本是否正确：

```bash
#!/bin/bash
# 确保脚本在项目根目录运行
cd "$(dirname "$0")"

# 清理之前的构建
rm -f boot/boot.bin kernel.bin

# 构建引导程序
nasm -f bin boot/boot.s -o boot/boot.bin

# 构建内核
gcc -m32 -nostdlib -nostartfiles -o kernel.bin kernel/kernel.c

echo "Build completed"
```

## 本地构建失败

### 1. 缺少必要的工具

确保安装了所有必要的工具：

```bash
# Ubuntu/Debian
sudo apt-get install nasm gcc make qemu-system-x86

# CentOS/RHEL
sudo yum install nasm gcc make qemu-kvm

# macOS
brew install nasm qemu
```

### 2. 架构问题

确保使用正确的编译选项：

```bash
# 对于32位系统
gcc -m32 -nostdlib -nostartfiles -o kernel.bin kernel/kernel.c

# 如果遇到架构不匹配问题，可能需要安装多架构支持
sudo apt-get install gcc-multilib
```

### 3. 权限问题

确保脚本具有执行权限：

```bash
chmod +x build.sh run.sh
```

## QEMU 运行问题

### 1. QEMU 未安装

确保正确安装了 QEMU：

```bash
# Ubuntu/Debian
sudo apt-get install qemu-system-x86

# 检查安装
qemu-system-i386 --version
```

### 2. 镜像文件问题

确保正确创建了镜像文件：

```bash
# 创建软盘镜像
dd if=/dev/zero of=floppy.img bs=1K count=1440
mkfs.msdos floppy.img

# 将引导程序写入镜像
dd if=boot/boot.bin of=floppy.img conv=notrunc
```

## 测试失败

### 1. Python 脚本问题

确保所有测试脚本都能正确运行：

```bash
python3 static-analysis.py
python3 unit-test-sim.py
python3 coverage-analysis.py
python3 integration-test.py
python3 run-tests.py
```

### 2. 编码问题

确保所有 Python 脚本使用 UTF-8 编码，并在文件开头添加：

```python
# -*- coding: utf-8 -*-
```

## 常见错误和解决方案

### 1. "No such file or directory" 错误

检查文件路径是否正确，确保在正确的目录中运行命令。

### 2. "Permission denied" 错误

确保脚本具有执行权限：
```bash
chmod +x *.sh
```

### 3. "Command not found" 错误

确保必要的工具已安装并添加到 PATH 中。

### 4. 构建产物未生成

检查构建脚本是否正确执行，查看是否有错误信息被忽略。

## 检查清单

在提交代码前，请检查以下项目：

1. [ ] 所有源代码文件都已正确添加到 Git
2. [ ] 构建脚本具有执行权限
3. [ ] 所有必要的依赖项都在 GitHub Actions 配置中列出
4. [ ] 测试脚本能够独立运行
5. [ ] README 文件已更新
6. [ ] 文档文件完整

## 调试技巧

### 1. 启用详细输出

在 GitHub Actions 中添加详细输出：

```yaml
- name: Debug information
  run: |
    pwd
    ls -la
    echo "Environment:"
    env
```

### 2. 分步调试

将复杂的步骤分解为多个小步骤，便于定位问题：

```yaml
- name: Check boot directory
  run: ls -la boot/
  
- name: Check kernel directory
  run: ls -la kernel/
```

## 联系支持

如果以上方法都无法解决问题：

1. 在 GitHub Issues 中详细描述问题
2. 提供完整的错误日志
3. 说明您使用的操作系统和环境
4. 提供复现问题的步骤

通过以上方法，您应该能够识别并解决项目中出现的大部分问题。