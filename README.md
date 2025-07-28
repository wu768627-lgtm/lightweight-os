# LightweightOS

一个轻量级、安全且高性能的操作系统，专为现代计算需求设计。

## 简介

LightweightOS 是一个基于微内核架构的操作系统，旨在提供卓越的性能、强大的安全性和极小的资源占用。它专为成为"性能小钢炮"而设计，适合资源受限的环境，同时不牺牲功能性和安全性。项目支持本地开发和云平台测试，方便开发者在不同环境中进行构建和验证。

## 特性

### 轻量级设计
- 微内核架构，内核体积小
- 最小化内存占用
- 快速启动时间
- 模块化设计，按需加载功能

### 安全性
- 安全启动机制
- 内存保护和进程隔离
- 强制访问控制
- 加密文件系统支持
- 详细的[安全特性](docs/security.md)

### 高性能
- 优化的进程调度算法
- 高效的内存管理
- 快速系统调用接口
- 优化的I/O子系统
- 详细的[性能优化方案](docs/performance.md)

## 系统架构

```
+-----------------------+
|     用户应用程序      |
|  - Shell              |
|  - 文本编辑器         |
|  - 系统监控工具       |
|  - 服务管理器         |
|  - 系统工具集         |
|  - 测试工具           |
+-----------------------+
|     C标准库/系统库    |
+-----------------------+
|   系统服务 (用户态)   |
|  - 文件系统服务       |
|  - 网络服务           |
|  - 设备驱动服务       |
|  - 系统服务守护进程   |
+-----------------------+
|    微内核 (核心态)    |
|  - 进程调度           |
|  - 内存管理           |
|  - 虚拟内存管理       |
|  - 进程间通信         |
|  - 系统调用接口       |
|  - 安全子系统         |
|  - 性能分析模块       |
|  - 日志系统           |
|  - 配置管理           |
|  - 异常处理           |
|  - 电源管理           |
|  - 测试框架           |
+-----------------------+
|      硬件平台         |
+-----------------------+
```

## 项目结构

- [boot/](boot/) - 系统引导程序
- [kernel/](kernel/) - 内核源代码
- [drivers/](drivers/) - 设备驱动程序
- [libs/](libs/) - 系统库
- [userland/](userland/) - 用户空间程序
- [tools/](tools/) - 构建和开发工具
- [docs/](docs/) - 文档
  - [设计文档](docs/design.md)
  - [安全特性](docs/security.md)
  - [性能优化](docs/performance.md)
  - [测试计划](docs/testing.md)

## 构建说明

### 环境要求
- GCC 编译器
- GNU Make
- NASM 汇编器
- QEMU (用于测试)

### 构建流程
所有构建操作都可通过 Makefile 完成：
```bash
# 构建完整系统
make

# 构建用户空间程序
make userland

# 清理构建产物
make clean
```

### 构建步骤

在 Unix/Linux/macOS 系统上:
```bash
# 克隆项目 (如果使用版本控制)
# git clone <repository-url>
# cd lightweight-os

# 构建系统
./build.sh

# 或者手动构建
make

# 构建用户空间程序
make userland
```

在 Windows 系统上:
```cmd
# 使用 Makefile 构建
make

# 构建用户空间程序
make userland
```

## 运行系统

### 使用脚本运行
```bash
./run.sh
```

### 手动运行
```bash
# 运行系统 (需要 QEMU)
make run

# 或者直接使用 QEMU
qemu-system-i386 -fda build/lightweightos.img
```


## 测试内容

1. **单元测试** - 测试各个模块的基本功能
2. **集成测试** - 测试模块间的交互
3. **系统测试** - 测试整个系统的功能
4. **性能测试** - 测试系统的性能指标
5. **压力测试** - 测试系统在极限条件下的表现
6. **安全测试** - 测试系统的安全机制

## 开发计划

1. [x] 基本内核框架
2. [x] 内存管理模块
3. [x] 进程管理模块
4. [x] 中断处理系统
5. [x] 系统调用接口
6. [x] 文件系统实现
7. [x] 网络协议栈
8. [x] 设备驱动框架
9. [x] 用户空间库
10. [x] 图形用户界面
11. [x] 安全模块增强
12. [x] 性能优化调优
13. [x] 虚拟内存管理
14. [x] 高级进程调度
15. [x] 系统日志功能
16. [x] 扩展系统调用接口
17. [x] 用户空间工具和应用程序
18. [x] 系统服务管理
19. [x] 配置管理模块
20. [x] 异常处理模块
21. [x] 电源管理模块
22. [x] 系统工具集
23. [x] 测试框架和工具
24. [x] 云平台测试集成
25. [ ] 实际硬件测试
26. [ ] 更多用户应用程序
27. [ ] 图形界面应用
## 云平台测试

本项目集成了持续集成(CI)系统，支持在云平台上进行自动化构建和测试。详细信息请参阅 [云平台测试指南](docs/cloud-testing.md)。

### 使用 GitHub Actions

1. Fork 本项目到你的 GitHub 账户
2. 推送代码到你的仓库
3. GitHub Actions 会自动运行构建和测试工作流

你也可以手动触发构建：
1. 在 GitHub 仓库页面，点击 "Actions" 标签
2. 选择 "Build and Test OS" 工作流
3. 点击 "Run workflow" 按钮

## 推送到 GitHub

如果您想要将此项目推送到您自己的 GitHub 仓库，请按照以下步骤操作：
  
  详细信息请参阅 [GitHub 推送指南](GITHUB_PUSH_GUIDE.md)。

1. 在 GitHub 上创建一个新的仓库（例如 `lightweight-os`）
2. 使用我们提供的脚本推送代码：
   - 在 Linux/Mac 上：
     ```bash
     chmod +x push-to-github.sh
     ./push-to-github.sh your-github-username your-repo-name
     ```
   - 在 Windows 上：
     ```cmd
     push-to-github.bat your-github-username your-repo-name
     ```
   - 例如：
     ```bash
     ./push-to-github.sh wu768627-lgtm lightweight-os
     ```

3. 如果推送失败，请确保：
   - 您的网络连接正常
   - 您已在 GitHub 上创建了同名仓库
   - 您有权限推送到该仓库

## 贡献

欢迎提交 Issue 和 Pull Request 来帮助改进 LightweightOS。

## 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 联系方式

如有任何问题或建议，请通过以下方式联系：
- 提交 Issue
- 发送邮件至 [your-email@example.com]