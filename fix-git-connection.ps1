# Git连接问题修复脚本

Write-Host "尝试修复Git连接问题..."
Write-Host "========================"

# 检查当前Git配置
Write-Host "1. 检查当前Git配置..."
cd "f:\app2\Desktop\kg\lightweight-os"
git config --local -l

Write-Host "2. 尝试配置代理设置..."
# 尝试禁用代理
git config --local --unset http.proxy
git config --local --unset https.proxy

# 尝试使用不同的传输协议
git config --local http.sslBackend schannel

# 尝试增加超时时间
git config --local http.timeout 120

Write-Host "3. 尝试使用SSH而不是HTTPS..."
# 查看当前远程URL
$remoteUrl = git remote get-url origin
Write-Host "   当前远程URL: $remoteUrl"

# 如果是HTTPS URL，尝试改为SSH URL
if ($remoteUrl -like "https://*") {
    Write-Host "   检测到HTTPS URL，尝试改为SSH URL..."
    # 注意：这需要您在GitHub上配置了SSH密钥
    # git remote set-url origin git@github.com:wu768627-lgtm/lightweight-os.git
    Write-Host "   注意：要使用SSH，您需要先在GitHub上配置SSH密钥"
}

Write-Host "4. 尝试使用不同的Git传输协议..."
git config --local http.version HTTP/1.1

Write-Host "========================"
Write-Host "Git配置更新完成"
Write-Host ""
Write-Host "现在再次尝试推送："
Write-Host "cd `"f:\app2\Desktop\kg\lightweight-os`""
Write-Host "git push"