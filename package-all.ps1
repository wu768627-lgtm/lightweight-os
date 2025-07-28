# LightweightOS 多格式打包脚本
# 创建多种格式的发布包 (ZIP, TAR.GZ)

[CmdletBinding()]
param(
    [string]$VERSION = "1.0.0",
    [switch]$CleanOnly
)

Write-Host "========================================="
Write-Host "LightweightOS 多格式打包脚本"
Write-Host "========================================="

# 如果只执行清理操作，处理完直接退出
if ($CleanOnly) {
    $TEMP_DIR = "$env:TEMP\lightweight-os-packaging"
    Write-Host "清理临时目录..."
    if (Test-Path $TEMP_DIR) {
        Remove-Item $TEMP_DIR -Recurse -Force
    }
    Write-Host "多格式打包脚本执行完成!"
    exit
}

# 设置版本信息
$VERSION = "1.0.0"
$PACKAGE_NAME = "lightweight-os-v$VERSION"
$TEMP_DIR = "$env:TEMP\lightweight-os-packaging"

Write-Host "创建版本: $VERSION"
Write-Host "包名: $PACKAGE_NAME"

# 清理之前的临时目录
Write-Host "清理临时目录..."
if (Test-Path $TEMP_DIR) {
    Remove-Item $TEMP_DIR -Recurse -Force
}

# 创建临时目录结构
Write-Host "创建临时目录结构..."
New-Item -ItemType Directory -Path $TEMP_DIR | Out-Null
New-Item -ItemType Directory -Path "$TEMP_DIR\$PACKAGE_NAME" | Out-Null

# 获取当前目录
$SOURCE_DIR = Get-Location
$TARGET_DIR = "$TEMP_DIR\$PACKAGE_NAME"

# 复制必要文件和目录
Write-Host "复制文件和目录..."

# 核心代码目录
Copy-Item -Path "$SOURCE_DIR\boot" -Destination $TARGET_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\kernel" -Destination $TARGET_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\drivers" -Destination $TARGET_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\libs" -Destination $TARGET_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\userland" -Destination $TARGET_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\tools" -Destination $TARGET_DIR -Recurse -Force

# 构建和运行脚本
Copy-Item -Path "$SOURCE_DIR\build.sh" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\run.sh" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\run-qemu.sh" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\push-to-github.sh" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\push-to-github.bat" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\diagnose.sh" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\package.sh" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\package.bat" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\package.ps1" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\package-all.ps1" -Destination $TARGET_DIR -Force

# 配置文件
Copy-Item -Path "$SOURCE_DIR\Makefile" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\wuying-test-config.json" -Destination $TARGET_DIR -Force

# 文档
Copy-Item -Path "$SOURCE_DIR\docs" -Destination $TARGET_DIR -Recurse -Force
Copy-Item -Path "$SOURCE_DIR\README.md" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\RELEASE.md" -Destination $TARGET_DIR -Force
if (Test-Path "$SOURCE_DIR\LICENSE") {
    Copy-Item -Path "$SOURCE_DIR\LICENSE" -Destination $TARGET_DIR -Force
}

# 测试文件
Copy-Item -Path "$SOURCE_DIR\static-analysis.py" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\unit-test-sim.py" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\coverage-analysis.py" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\integration-test.py" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\run-tests.py" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\bug-test.py" -Destination $TARGET_DIR -Force
Copy-Item -Path "$SOURCE_DIR\bug-test-improved.py" -Destination $TARGET_DIR -Force

# GitHub Actions 工作流
Copy-Item -Path "$SOURCE_DIR\.github" -Destination $TARGET_DIR -Recurse -Force

# 创建版本文件
Write-Host "创建版本信息文件..."
$VERSION_CONTENT = @"
LightweightOS 版本信息
==================

版本: $VERSION
打包日期: $(Get-Date)
"@
Set-Content -Path "$TARGET_DIR\VERSION" -Value $VERSION_CONTENT

# 创建 ZIP 包
Write-Host "创建 ZIP 包..."
try {
    Compress-Archive -Path "$TEMP_DIR\$PACKAGE_NAME\*" -DestinationPath "$PACKAGE_NAME.zip" -Force -ErrorAction Stop
} catch {
    Write-Error "ZIP 打包失败: $_"
    # 清理临时目录
    Write-Host "清理临时目录..."
    if (Test-Path $TEMP_DIR) {
        Remove-Item $TEMP_DIR -Recurse -Force
    }
    exit 1
}

# 检查是否有 tar 命令可用（通过 WSL 或 Git Bash）
$tarAvailable = $false
try {
    tar --version > $null 2>&1
    $tarAvailable = $true
} catch {
    $tarAvailable = $false
}

if ($tarAvailable) {
    Write-Host "创建 TAR.GZ 包..."
    # 创建 TAR.GZ 包
    try {
        Push-Location $TEMP_DIR
        tar -czf "$SOURCE_DIR\$PACKAGE_NAME.tar.gz" $PACKAGE_NAME 2>&1 | Out-Null
        if ($LASTEXITCODE -ne 0) { throw "tar 命令执行失败" }
    } catch {
        Write-Error "TAR.GZ 打包失败: $_"
    } finally {
        Pop-Location
    }
} else {
    Write-Host "注意: 系统中未找到 tar 命令，跳过 TAR.GZ 包创建"
    Write-Host "请安装 WSL 或 Git Bash 以创建 TAR.GZ 格式的包"
}

# 显示包信息
Write-Host "========================================="
Write-Host "打包完成!"
Write-Host "包信息:"

if (Test-Path "$PACKAGE_NAME.zip") {
    $zipFile = Get-Item "$PACKAGE_NAME.zip"
    Write-Host "  ZIP 包: $PACKAGE_NAME.zip ($($zipFile.Length) 字节)"
}

if (Test-Path "$PACKAGE_NAME.tar.gz") {
    $tarFile = Get-Item "$PACKAGE_NAME.tar.gz"
    Write-Host "  TAR.GZ 包: $PACKAGE_NAME.tar.gz ($($tarFile.Length) 字节)"
}

Write-Host ""
Write-Host "文件列表:"
Get-ChildItem "$PACKAGE_NAME.*"
Write-Host "========================================="

# 清理临时目录
Write-Host "清理临时目录..."
if (Test-Path $TEMP_DIR) {
    try {
        Remove-Item $TEMP_DIR -Recurse -Force -ErrorAction Stop
    } catch {
        Write-Warning "临时目录清理失败: $_"
    }
}

Write-Host "多格式打包脚本执行完成!"