# 云平台测试指南

本文档介绍了如何使用免费的云平台来托管、编译和测试 LightweightOS 操作系统项目。

## GitHub Actions 云编译和测试

[GitHub Actions](https://github.com/features/actions) 是 GitHub 提供的持续集成和持续部署 (CI/CD) 平台，可以自动化构建、测试和部署项目。对于 LightweightOS 这样的操作系统项目，我们可以使用 GitHub Actions 来自动编译和测试我们的代码。

### 1. 创建 GitHub 仓库

1. 访问 [GitHub](https://github.com) 并登录
2. 点击右上角的 "+" 号，选择 "New repository"
3. 输入仓库名称，例如 "lightweight-os"
4. 选择公开 (Public) 仓库（私有仓库可能有使用限制）
5. 不要初始化 README、.gitignore 或许可证文件
6. 点击 "Create repository"

### 2. 推送代码到 GitHub

在本地项目目录中执行以下命令：

```bash
git init
git add .
git commit -m "Initial commit"
git branch -M main
git remote add origin https://github.com/你的用户名/lightweight-os.git
git push -u origin main
```

### 3. 配置 GitHub Actions

项目中已经包含了 [.github/workflows/build-and-test.yml](file:///f%3A/app2/Desktop/kg/lightweight-os/.github/workflows/build-and-test.yml) 文件，它定义了以下自动化任务：

1. 检出代码
2. 安装编译依赖项（build-essential, nasm, gcc-multilib, qemu-system-x86）
3. 编译操作系统
4. 运行所有测试套件
5. 创建操作系统镜像
6. 上传构建产物

### 4. 触发构建

每次推送到 [main](file:///f%3A/app2/Desktop/kg/lightweight-os/docs/main) 或 [master](file:///f%3A/app2/Desktop/kg/lightweight-os/docs/master) 分支，或者创建 Pull Request 时，GitHub Actions 都会自动运行工作流。

你也可以手动触发构建：
1. 在 GitHub 仓库页面，点击 "Actions" 标签
2. 选择 "Build and Test OS" 工作流
3. 点击 "Run workflow" 按钮

### 5. 查看构建结果

1. 在 GitHub 仓库页面，点击 "Actions" 标签
2. 选择一个工作流运行查看详细信息
3. 在 "Jobs" 部分查看每个步骤的执行结果
4. 可以下载构建产物（Build Artifacts）进行进一步测试

## 其他免费云平台选项

### 1. GitLab CI/CD

GitLab 也提供了类似 GitHub Actions 的 CI/CD 功能：

1. 在 [GitLab](https://gitlab.com) 创建项目
2. 推送代码到 GitLab
3. 创建 `.gitlab-ci.yml` 配置文件：

```yaml
image: ubuntu:latest

before_script:
  - apt-get update
  - apt-get install -y build-essential nasm gcc-multilib qemu-system-x86

stages:
  - build
  - test

build_os:
  stage: build
  script:
    - chmod +x build.sh
    - ./build.sh
    - ls -la boot/boot.bin kernel.bin
  artifacts:
    paths:
      - boot/boot.bin
      - kernel.bin

test_os:
  stage: test
  script:
    - python3 static-analysis.py
    - python3 unit-test-sim.py
    - python3 coverage-analysis.py
    - python3 integration-test.py
    - python3 run-tests.py
```

### 2. 免费云服务器

一些云服务提供商提供免费套餐：

#### Oracle Cloud Free Tier
- 官方网站: [https://www.oracle.com/cloud/free/](https://www.oracle.com/cloud/free/)
- 提供永久免费的虚拟机实例
- 2 个 AMD 计算实例（1 核 CPU，1GB 内存）
- 4 个 ARM 计算实例（2 核 CPU，12GB 内存）
- 限制：需要提供信用卡信息

#### Google Cloud Free Tier
- 提供 300 美元的免费额度（12 个月）
- 每月 1GB 云端存储免费

#### Microsoft Azure Free Tier
- 提供 200 美元的免费额度（30 天）
- 12 个月的免费服务

#### Amazon Web Services (AWS) Free Tier
- 提供 12 个月的免费服务
- t2.micro 实例每月 750 小时免费

## 在云环境中运行操作系统

### 1. 使用 QEMU 模拟器

在云服务器上，可以使用 QEMU 来运行和测试操作系统：

```bash
# 安装 QEMU
sudo apt-get install qemu-system-x86

# 运行操作系统
qemu-system-i386 -fda os-image.img
```

### 2. 使用 NoMachine 进行图形访问

在云服务器上安装 NoMachine 可以获得图形界面访问：

```bash
# 下载并安装 NoMachine
wget https://download.nomachine.com/download/7.7/Linux/nomachine_7.7.4_1_amd64.deb
sudo dpkg -i nomachine_7.7.4_1_amd64.deb

# 启动 NoMachine 服务
sudo systemctl start nxserver
```

### 3. 使用 VNC 进行远程访问

```bash
# 安装 VNC 服务器
sudo apt-get install tightvncserver

# 启动 VNC 服务器
vncserver :1

# 通过 VNC 客户端连接
```

## 最佳实践

### 1. 自动化测试

确保所有的测试脚本都能在云环境中正常运行：

1. 使用相对路径而不是绝对路径
2. 确保所有依赖项都能通过包管理器安装
3. 避免使用特定于本地环境的配置

### 2. 构建产物管理

1. 上传重要的构建产物作为 GitHub Actions 的 artifacts
2. 使用版本标签管理重要的发布版本
3. 保持构建脚本的简洁和可重复性

### 3. 安全注意事项

1. 不要在配置文件中存储敏感信息
2. 使用 GitHub Secrets 存储敏感数据
3. 定期更新依赖项和工具链

## 故障排除

### 1. 构建失败

检查以下几点：
1. 所有依赖项是否正确安装
2. 构建脚本是否有权限问题
3. 代码中是否有平台相关的假设

### 2. 测试失败

检查以下几点：
1. 测试脚本是否兼容云环境
2. 是否有足够的资源（内存、磁盘空间）
3. 是否需要特殊的环境变量或配置

### 3. QEMU 运行问题

检查以下几点：
1. 是否正确安装了 QEMU
2. 镜像文件是否正确创建
3. 是否有足够的权限运行虚拟机

通过使用这些云平台和工具，你可以免费地编译、测试和运行 LightweightOS 操作系统，而无需在本地配置复杂的开发环境。