# 推送代码到 GitHub 的详细指南

本指南将帮助您将 LightweightOS 项目推送到 GitHub 仓库。

## 前提条件

1. 确保您已经在 GitHub 上创建了一个仓库
2. 确保您的网络连接正常
3. 如果使用公司或学校的网络，可能需要配置代理

## 方法一：使用 HTTPS（推荐）

### 1. 确认远程仓库设置

```bash
cd "f:\app2\Desktop\kg\lightweight-os"
git remote -v
```

如果已经设置了错误的远程仓库，先移除它：
```bash
git remote remove origin
```

然后添加正确的远程仓库：
```bash
git remote add origin https://github.com/wu768627-lgtm/lightweight-os.git
```

### 2. 确认分支名称

GitHub 现在默认使用 [main](file:///f%3A/app2/Desktop/kg/lightweight-os/docs/main) 作为默认分支名称：
```bash
git branch -M main
```

### 3. 推送代码

```bash
git push -u origin main
```

如果提示需要用户名和密码，需要使用 GitHub Personal Access Token 而不是密码：
1. 访问 GitHub 设置 -> Developer settings -> Personal access tokens
2. 点击 "Generate new token"
3. 选择适当的权限（repo 权限即可）
4. 生成 token 后复制它
5. 当提示输入密码时，粘贴这个 token

## 方法二：使用 SSH（更安全且方便）

### 1. 生成 SSH 密钥（如果还没有）

在终端中运行：
```bash
ssh-keygen -t ed25519 -C "your_email@example.com"
```

按提示操作，默认设置即可。

### 2. 添加 SSH 密钥到 ssh-agent

```bash
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519
```

### 3. 添加 SSH 密钥到 GitHub 账户

1. 复制公钥内容：
   ```bash
   cat ~/.ssh/id_ed25519.pub
   ```
2. 在 GitHub 上，进入 Settings -> SSH and GPG keys
3. 点击 "New SSH key"
4. 粘贴公钥内容并保存

### 4. 更改远程仓库 URL 为 SSH

```bash
git remote remove origin
git remote add origin git@github.com:wu768627-lgtm/lightweight-os.git
```

### 5. 推送代码

```bash
git push -u origin main
```

## 网络问题解决方案

### 1. 使用代理（如果在公司或学校网络）

如果需要使用代理，可以配置 Git：
```bash
git config --global http.proxy http://proxy.server:port
git config --global https.proxy https://proxy.server:port
```

取消代理设置：
```bash
git config --global --unset http.proxy
git config --global --unset https.proxy
```

### 2. 使用 HTTPS 代理

```bash
git config --global http.proxy http://username:password@proxy.server:port
```

### 3. 尝试不同的 Git 协议

```bash
git config --global url."https://".insteadOf git://
```

## 故障排除

### 1. 如果推送过程中断

Git 有断点续传功能，重新执行推送命令即可：
```bash
git push -u origin main
```

### 2. 如果遇到权限问题

确保您有仓库的写入权限，并且使用正确的认证方式。

### 3. 如果遇到 SSL 证书问题

可以临时禁用 SSL 验证（不推荐，仅用于测试）：
```bash
git config --global http.sslVerify false
```

恢复 SSL 验证：
```bash
git config --global http.sslVerify true
```

## 验证推送结果

推送完成后，您可以访问 GitHub 仓库页面确认文件是否已成功上传。

## 后续更新推送

推送初始代码后，后续的更新可以使用简单的命令推送：
```bash
git add .
git commit -m "您的提交信息"
git push
```

## 需要帮助？

如果以上方法都无法解决问题，请提供具体的错误信息，以便进一步诊断问题。