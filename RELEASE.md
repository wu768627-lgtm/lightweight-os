# LightweightOS v1.0.0 发布说明

我们很高兴地宣布 LightweightOS v1.0.0 正式发布！这是一个教育性质的操作系统项目，旨在展示操作系统的核心概念和实现技术。

## 发布亮点

- 完整的引导程序实现
- 基本内核功能（内存管理、进程调度、系统调用等）
- 设备驱动框架
- 文件系统支持
- 用户空间程序示例
- 完整的测试套件
- GitHub Actions 自动化构建和测试

## 包含内容

此发布包包含以下组件：

- 引导程序源代码 (`boot/`)
- 内核核心代码 (`kernel/`)
- 设备驱动程序 (`drivers/`)
- 系统库函数 (`libs/`)
- 用户空间程序 (`userland/`)
- 系统工具 (`tools/`)
- 文档 (`docs/`)
- 测试工具和脚本
- 构建和运行脚本

## 系统要求

### 开发环境
- NASM (汇编器)
- GCC (C 编译器)
- Make (构建工具)
- QEMU (可选，用于测试运行)

### 运行环境
- x86 架构计算机
- BIOS 启动支持

## 安装和使用

### 构建项目

1. 确保安装了必要的工具：
   ```bash
   # Ubuntu/Debian
   sudo apt-get install nasm gcc make qemu-system-x86
   
   # CentOS/RHEL
   sudo yum install nasm gcc make qemu-kvm
   
   # macOS
   brew install nasm gcc qemu
   ```

2. 解压发布包：
   ```bash
   unzip lightweight-os-v1.0.0.zip
   cd lightweight-os-v1.0.0
   ```

3. 运行构建脚本：
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

### 运行系统

1. 使用内置运行脚本：
   ```bash
   chmod +x run.sh
   ./run.sh
   ```

2. 或者使用 QEMU 直接运行：
   ```bash
   qemu-system-i386 -fda floppy.img
   ```

### 在 Windows 上使用

在 Windows 上，您可以使用 WSL (Windows Subsystem for Linux) 来构建和运行系统：

1. 安装 WSL2 和 Ubuntu
2. 在 Ubuntu 中安装必要的工具：
   ```bash
   sudo apt-get update
   sudo apt-get install nasm gcc make qemu-system-x86
   ```
3. 按照上述构建和运行步骤操作

## 目录结构

```
lightweight-os-v1.0.0/
├── boot/          # 启动引导程序
├── kernel/        # 内核核心代码
│   ├── kernel.c   # 内核主入口
│   ├── memory.c   # 内存管理
│   ├── process.c  # 进程管理
│   ├── interrupts.c # 中断处理
│   ├── syscall.c  # 系统调用
│   └── ...        # 其他内核组件
├── drivers/       # 设备驱动程序
├── libs/          # 系统库函数
├── userland/      # 用户空间程序
├── tools/         # 系统工具
├── docs/          # 文档
├── tests/         # 测试代码
├── .github/       # GitHub Actions 配置
├── build.sh       # 构建脚本
├── run.sh         # 运行脚本
└── Makefile       # 构建配置
```

## 文档

详细的文档可以在 `docs/` 目录中找到：

- [设计文档](docs/design.md) - 系统整体设计
- [安全文档](docs/security.md) - 安全特性和考虑
- [性能文档](docs/performance.md) - 性能分析和优化
- [测试文档](docs/testing.md) - 测试策略和方法
- [云平台测试](docs/cloud-testing.md) - 在云平台上测试系统的指南

## 贡献

我们欢迎任何形式的贡献！如果您想为项目做出贡献，请：

1. Fork 项目
2. 创建功能分支
3. 提交更改
4. 推送分支
5. 创建 Pull Request

## 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 联系方式

如有任何问题或建议，请创建 Issue 或联系项目维护者：
邮箱: 1810200151@qq.com

感谢您对 LightweightOS 的关注和支持！