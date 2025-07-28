# 最终推送脚本

Write-Host "LightweightOS 最终推送脚本"
Write-Host "=============================="

Write-Host "检查当前Git状态..."
cd "f:\app2\Desktop\kg\lightweight-os"
$status = git status --porcelain
$branch = git branch --show-current
$remote = git remote get-url origin

Write-Host "当前分支: $branch"
Write-Host "远程仓库: $remote"

# 检查是否有未推送的提交
$unpushed = git log --oneline origin/$branch..$branch
if ($unpushed) {
    Write-Host "发现未推送的提交:"
    Write-Host $unpushed
    Write-Host ""
    
    Write-Host "尝试推送所有更改到GitHub..."
    try {
        git push -v
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✓ 推送成功完成!"
        } else {
            Write-Host "✗ 推送失败，错误代码: $LASTEXITCODE"
            Write-Host "可能的原因:"
            Write-Host "1. 网络连接问题"
            Write-Host "2. 防火墙阻止了连接"
            Write-Host "3. GitHub服务器暂时不可用"
            Write-Host ""
            Write-Host "解决建议:"
            Write-Host "- 检查网络连接"
            Write-Host "- 稍后再试"
            Write-Host "- 联系网络管理员（如果在公司/学校网络中）"
        }
    } catch {
        Write-Host ("✗ 推送过程中发生异常: " + $_.Exception.Message)
    }
} else {
    Write-Host "没有未推送的更改，所有内容都已同步到GitHub。"
}

Write-Host ""
Write-Host "脚本执行完成。"
Write-Host "按任意键退出..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")