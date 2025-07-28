# LightweightOS v1.0.0 发布说明

## 简介

LightweightOS v1.0.0 是一个轻量级操作系统内核项目的首个正式发布版本。该项目旨在提供一个简单、可理解的操作系统实现，展示了操作系统的核心概念和实现技术。

## 新增功能

- 完整的引导程序实现
- 基本内核功能（内存管理、进程调度、系统调用等）
- 设备驱动框架（文件系统、网络、图形、键盘等）
- 用户空间程序支持
- 完整的构建和测试工具链
- 详细的文档说明

## 系统要求

### 开发环境
- NASM (汇编器)
- GCC (C 编译器)
- QEMU (用于测试运行)
- Make (构建工具)

### 运行环境
- x86 架构计算机
- 至少 4MB RAM
- BIOS 兼容启动支持

## 安装说明

### 从源代码构建

1. 克隆或下载发布包:
   ```bash
   git clone https://github.com/yourusername/lightweight-os.git
   # 或者下载并解压 lightweight-os-v1.0.0.zip
   ```

2. 安装必要的工具:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install nasm gcc make qemu-system-x86
   
   # CentOS/RHEL
   sudo yum install nasm gcc make qemu-kvm
   
   # macOS
   brew install nasm qemu
   ```

3. 构建系统:
   ```bash
   cd lightweight-os
   chmod +x build.sh
   ./build.sh
   ```

4. 运行系统:
   ```bash
   chmod +x run.sh
   ./run.sh
   ```

### 使用预构建镜像

发布包中包含了预构建的系统镜像，可以直接使用 QEMU 运行:

```bash
qemu-system-i386 -fda floppy.img
```

## 包含内容

发布包中包含以下内容:

```
lightweight-os-v1.0.0/
├── boot/          # 启动引导程序
├── kernel/        # 内核核心代码
├── drivers/       # 设备驱动程序
├── libs/          # 系统库函数
├── userland/      # 用户空间程序
├── tools/         # 系统工具
├── docs/          # 文档
├── tests/         # 测试代码
├── build.sh       # 构建脚本
├── run.sh         # 运行脚本
└── Makefile       # 构建配置
```

## 使用说明

### 构建系统

运行 `build.sh` 脚本将会:

1. 汇编引导程序
2. 编译内核
3. 创建可启动的软盘镜像

### 运行系统

运行 `run.sh` 脚本将会:

1. 启动 QEMU 虚拟机
2. 从创建的软盘镜像启动系统
3. 运行 LightweightOS

### 测试系统

项目包含多种测试工具:

- `static-analysis.py` - 静态代码分析
- `unit-test-sim.py` - 单元测试模拟
- `coverage-analysis.py` - 代码覆盖率分析
- `integration-test.py` - 集成测试
- `run-tests.py` - 运行所有测试

## 开发指南

### 目录结构

- `boot/` - 引导程序源代码
- `kernel/` - 内核核心代码
- `drivers/` - 设备驱动程序
- `libs/` - 系统库函数
- `userland/` - 用户空间程序
- `tools/` - 系统工具
- `docs/` - 项目文档

### 贡献代码

欢迎对项目进行贡献！请遵循以下步骤:

1. Fork 项目
2. 创建功能分支
3. 提交更改
4. 推送分支
5. 创建 Pull Request

## 已知问题

- 目前仅支持在 x86 架构上运行
- 网络功能仍在开发中
- 文件系统功能有限

## 更新日志

### v1.0.0 (2025-07-28)

- 首个正式发布版本
- 实现基本的内核功能
- 完整的设备驱动框架
- 用户空间程序支持
- 完善的文档和测试工具

## 许可证

本项目采用 MIT 许可证 - 查看 LICENSE 文件了解详情。

## 联系方式

如有任何问题或建议，请创建 Issue 或联系项目维护者：
邮箱: 1810200151@qq.com