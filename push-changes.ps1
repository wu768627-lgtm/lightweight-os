# LightweightOS PowerShell 推送脚本
# 方便推送更改到 GitHub

Write-Host "========================================"
Write-Host "LightweightOS PowerShell 推送脚本"
Write-Host "========================================"

# 检查 Git 是否可用
try {
    $gitVersion = git --version 2>$null
    if (-not $gitVersion) {
        Write-Host "错误: 未找到 Git，请确保已安装 Git"
        pause
        exit 1
    }
} catch {
    Write-Host "错误: 无法执行 git 命令，请确保已安装 Git"
    pause
    exit 1
}

# 检查是否有未提交的更改
try {
    $gitStatus = git status --porcelain 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "错误: 无法获取 Git 状态"
        pause
        exit 1
    }
} catch {
    Write-Host "错误: 无法获取 Git 状态"
    pause
    exit 1
}

# 检查是否有未提交的更改
if ($gitStatus) {
    Write-Host "检测到未提交的更改:"
    git status --short
    
    Write-Host ""
    Write-Host "请选择操作:"
    Write-Host "1. 添加所有更改并提交"
    Write-Host "2. 仅推送已提交的更改"
    Write-Host "3. 取消"
    
    $choice = Read-Host "请选择操作 (1/2/3)"
    
    switch ($choice) {
        "3" {
            Write-Host "操作已取消"
            exit 0
        }
        
        "2" {
            Write-Host "推送已提交的更改..."
            # 继续到推送部分
        }
        
        "1" {
            $commitMessage = Read-Host "请输入提交信息"
            if (-not $commitMessage) {
                $commitMessage = "Update files"
            }
            
            Write-Host "添加所有更改..."
            git add .
            
            Write-Host "提交更改..."
            git commit -m "$commitMessage"
        }
        
        default {
            Write-Host "无效选择，操作已取消"
            exit 0
        }
    }
} else {
    Write-Host "没有未提交的更改"
}

# 推送更改
Write-Host "推送更改到 GitHub..."
git push

if ($LASTEXITCODE -eq 0) {
    Write-Host "推送成功完成!"
} else {
    Write-Host "推送失败，可能是网络问题"
    Write-Host "请检查网络连接后重试"
}

Write-Host ""
Write-Host "脚本执行完成"
Write-Host "按任意键退出..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")