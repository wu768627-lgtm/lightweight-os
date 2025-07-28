@echo off
REM LightweightOS 推送脚本
REM 方便推送更改到 GitHub

echo ========================================
echo LightweightOS 推送脚本
echo ========================================

REM 检查是否有未提交的更改
echo 检查 Git 状态...
git status --porcelain >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 无法执行 git 命令，请确保已安装 Git
    pause
    exit /b 1
)

REM 检查是否有未提交的更改
git diff-index --quiet HEAD -- >nul 2>&1
if %errorlevel% equ 1 (
    echo 检测到未提交的更改
    echo 请选择操作:
    echo 1. 添加所有更改并提交
    echo 2. 仅推送已提交的更改
    echo 3. 取消
    choice /c 123 /m "请选择操作"
    
    if errorlevel 3 (
        echo 操作已取消
        exit /b 0
    )
    
    if errorlevel 2 (
        echo 推送已提交的更改...
        goto push
    )
    
    if errorlevel 1 (
        echo 请输入提交信息:
        set /p commit_message="> "
        if "%commit_message%"=="" (
            set commit_message="Update files"
        )
        echo 添加所有更改...
        git add .
        echo 提交更改...
        git commit -m "%commit_message%"
    )
) else (
    echo 没有未提交的更改
)

:push
echo 推送更改到 GitHub...
git push

if %errorlevel% equ 0 (
    echo 推送成功完成!
) else (
    echo 推送失败，可能是网络问题
    echo 请检查网络连接后重试
)

echo.
echo 脚本执行完成
pause