@echo off
echo LightweightOS 推送脚本
echo ======================

echo 切换到项目目录...
cd /d "f:\app2\Desktop\kg\lightweight-os"

echo 检查Git状态...
git status

echo 尝试推送所有更改...
git push

echo 推送完成。
pause