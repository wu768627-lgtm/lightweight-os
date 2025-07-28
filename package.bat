@echo off
REM LightweightOS Windows 打包脚本
REM 创建一个包含所有必要文件的发布包

echo =========================================
echo LightweightOS Windows 打包脚本
echo =========================================

REM 设置版本信息
set VERSION=1.0.0
set PACKAGE_NAME=lightweight-os-v%VERSION%
set PACKAGE_DIR=%TEMP%\%PACKAGE_NAME%

echo 创建版本: %VERSION%
echo 包名: %PACKAGE_NAME%

REM 清理之前的打包目录
echo 清理之前的打包目录...
if exist "%PACKAGE_DIR%" rmdir /s /q "%PACKAGE_DIR%"
if exist "%PACKAGE_NAME%.zip" del "%PACKAGE_NAME%.zip"

REM 创建打包目录结构
echo 创建打包目录结构...
mkdir "%PACKAGE_DIR%"

REM 复制必要文件和目录
echo 复制文件和目录...

REM 核心代码目录
xcopy boot "%PACKAGE_DIR%\boot\" /E /I /H /Y
xcopy kernel "%PACKAGE_DIR%\kernel\" /E /I /H /Y
xcopy drivers "%PACKAGE_DIR%\drivers\" /E /I /H /Y
xcopy libs "%PACKAGE_DIR%\libs\" /E /I /H /Y
xcopy userland "%PACKAGE_DIR%\userland\" /E /I /H /Y
xcopy tools "%PACKAGE_DIR%\tools\" /E /I /H /Y

REM 构建和运行脚本
copy build.sh "%PACKAGE_DIR%\" >nul
copy run.sh "%PACKAGE_DIR%\" >nul
copy run-qemu.sh "%PACKAGE_DIR%\" >nul
copy push-to-github.sh "%PACKAGE_DIR%\" >nul
copy push-to-github.bat "%PACKAGE_DIR%\" >nul
copy diagnose.sh "%PACKAGE_DIR%\" >nul
copy package.sh "%PACKAGE_DIR%\" >nul
copy package.bat "%PACKAGE_DIR%\" >nul

REM 配置文件
copy Makefile "%PACKAGE_DIR%\" >nul
copy wuying-test-config.json "%PACKAGE_DIR%\" >nul

REM 文档
xcopy docs "%PACKAGE_DIR%\docs\" /E /I /H /Y
copy README.md "%PACKAGE_DIR%\" >nul
if exist LICENSE copy LICENSE "%PACKAGE_DIR%\" >nul

REM 测试文件
copy static-analysis.py "%PACKAGE_DIR%\" >nul
copy unit-test-sim.py "%PACKAGE_DIR%\" >nul
copy coverage-analysis.py "%PACKAGE_DIR%\" >nul
copy integration-test.py "%PACKAGE_DIR%\" >nul
copy run-tests.py "%PACKAGE_DIR%\" >nul
copy bug-test.py "%PACKAGE_DIR%\" >nul
copy bug-test-improved.py "%PACKAGE_DIR%\" >nul

REM GitHub Actions 工作流
xcopy .github "%PACKAGE_DIR%\.github\" /E /I /H /Y

REM 创建 ZIP 包
echo 创建 ZIP 包...
cd /d %TEMP%
if exist "%PACKAGE_NAME%.zip" del "%PACKAGE_NAME%.zip"
powershell -Command "Add-Type -AssemblyName System.IO.Compression.FileSystem; [System.IO.Compression.ZipFile]::CreateFromDirectory('%PACKAGE_DIR%', '%cd%\%PACKAGE_NAME%.zip')"
cd /d "%cd%"

REM 显示包信息
echo =========================================
echo 打包完成!
echo 包信息:
echo   ZIP 包: %PACKAGE_NAME%.zip
for %%A in ("%PACKAGE_NAME%.zip") do echo   文件大小: %%~zA 字节
dir "%PACKAGE_NAME%".zip
echo =========================================

REM 清理临时目录
echo 清理临时目录...
rmdir /s /q "%PACKAGE_DIR%"

echo 打包脚本执行完成!