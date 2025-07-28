#!/bin/bash

# 推送到 GitHub 的脚本
# 使用方法: ./push-to-github.sh <your-github-username> <repository-name>

if [ "$#" -ne 2 ]; then
    echo "使用方法: $0 <your-github-username> <repository-name>"
    echo "例如: $0 wu768627-lgtm lightweight-os"
    exit 1
fi

GITHUB_USERNAME=$1
REPO_NAME=$2

echo "设置远程仓库..."
git remote add origin https://github.com/$GITHUB_USERNAME/$REPO_NAME.git

echo "设置分支名称..."
git branch -M main

echo "推送代码到 GitHub..."
git push -u origin main

echo "如果推送失败，请尝试以下步骤："
echo "1. 检查网络连接"
echo "2. 确保在 GitHub 上创建了同名仓库"
echo "3. 如果仍然失败，可以尝试使用 SSH 方式："
echo "   git remote remove origin"
echo "   git remote add origin git@github.com:$GITHUB_USERNAME/$REPO_NAME.git"
echo "   git push -u origin main"