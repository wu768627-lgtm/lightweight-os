# LightweightOS PowerShell 打包脚本
# 创建一个包含所有必要文件的发布包

Write-Host "========================================="
Write-Host "LightweightOS PowerShell 打包脚本"
Write-Host "========================================="

# 设置版本信息
$VERSION = "1.0.0"
$PACKAGE_NAME = "lightweight-os-v$VERSION"
$PACKAGE_DIR = "$env:TEMP\$PACKAGE_NAME"

Write-Host "创建版本: $VERSION"
Write-Host "包名: $PACKAGE_NAME"

# 清理之前的打包目录
Write-Host "清理之前的打包目录..."
if (Test-Path $PACKAGE_DIR) {
    Remove-Item $PACKAGE_DIR -Recurse -Force
}
if (Test-Path "$PACKAGE_NAME.zip") {
    Remove-Item "$PACKAGE_NAME.zip" -Force
}

# 创建打包目录结构
Write-Host "创建打包目录结构..."
New-Item -ItemType Directory -Path $PACKAGE_DIR | Out-Null

# 获取当前目录
$SOURCE_DIR = Get-Location

# 复制必要文件和目录
Write-Host "复制文件和目录..."

# 核心代码目录
Copy-Item -Path "$SOURCE_DIR\boot" -Destination $PACKAGE_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\kernel" -Destination $PACKAGE_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\drivers" -Destination $PACKAGE_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\libs" -Destination $PACKAGE_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\userland" -Destination $PACKAGE_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\tools" -Destination $PACKAGE_DIR -Recurse -Force

# 构建和运行脚本
Copy-Item -Path "$SOURCE_DIR\build.sh" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\run.sh" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\run-qemu.sh" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\push-to-github.sh" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\push-to-github.bat" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\diagnose.sh" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\package.sh" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\package.bat" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\package.ps1" -Destination $PACKAGE_DIR -Force

# 配置文件
Copy-Item -Path "$SOURCE_DIR\Makefile" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\wuying-test-config.json" -Destination $PACKAGE_DIR -Force

# 文档
Copy-Item -Path "$SOURCE_DIR\docs" -Destination $PACKAGE_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\README.md" -Destination $PACKAGE_DIR -Force
if (Test-Path "$SOURCE_DIR\LICENSE") {
    Copy-Item -Path "$SOURCE_DIR\LICENSE" -Destination $PACKAGE_DIR -Force
}

# 测试文件
Copy-Item -Path "$SOURCE_DIR\static-analysis.py" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\unit-test-sim.py" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\coverage-analysis.py" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\integration-test.py" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\run-tests.py" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\bug-test.py" -Destination $PACKAGE_DIR -Force
Copy-Item -Path "$SOURCE_DIR\bug-test-improved.py" -Destination $PACKAGE_DIR -Force

# GitHub Actions 工作流
Copy-Item -Path "$SOURCE_DIR\.github" -Destination $PACKAGE_DIR -Recurse -Force

# 创建版本文件
Write-Host "创建版本信息文件..."
$VERSION_CONTENT = @"
LightweightOS 版本信息
==================

版本: $VERSION
打包日期: $(Get-Date)
"@
Set-Content -Path "$PACKAGE_DIR\VERSION" -Value $VERSION_CONTENT

# 创建 ZIP 包
Write-Host "创建 ZIP 包..."
Compress-Archive -Path $PACKAGE_DIR -DestinationPath "$PACKAGE_NAME.zip" -Force

# 显示包信息
Write-Host "========================================="
Write-Host "打包完成!"
Write-Host "包信息:"
Write-Host "  ZIP 包: $PACKAGE_NAME.zip"
$zipFile = Get-Item "$PACKAGE_NAME.zip"
Write-Host "  文件大小: $($zipFile.Length) 字节"
Write-Host ""
Write-Host "文件列表:"
Get-ChildItem "$PACKAGE_NAME.zip"
Write-Host "========================================="

# 清理临时目录
Write-Host "清理临时目录..."
if (Test-Path $PACKAGE_DIR) {
    Remove-Item $PACKAGE_DIR -Recurse -Force
}

Write-Host "打包脚本执行完成!"