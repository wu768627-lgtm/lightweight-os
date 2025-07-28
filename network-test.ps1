# 网络连接测试脚本

Write-Host "测试网络连接..."
Write-Host "========================"

# 测试基本网络连接
Write-Host "1. 测试DNS解析..."
Resolve-DnsName github.com -ErrorAction SilentlyContinue
if ($?) {
    Write-Host "   ✓ DNS解析成功"
} else {
    Write-Host "   ✗ DNS解析失败"
}

Write-Host "2. 测试HTTP连接..."
try {
    Invoke-WebRequest -Uri "https://github.com" -TimeoutSec 10 -ErrorAction SilentlyContinue
    Write-Host "   ✓ HTTP连接成功"
} catch {
    Write-Host "   ✗ HTTP连接失败: $($_.Exception.Message)"
}

Write-Host "3. 测试HTTPS连接..."
try {
    Invoke-WebRequest -Uri "https://api.github.com" -TimeoutSec 10 -ErrorAction SilentlyContinue
    Write-Host "   ✓ HTTPS连接成功"
} catch {
    Write-Host "   ✗ HTTPS连接失败: $($_.Exception.Message)"
}

Write-Host "4. 测试端口443连接..."
try {
    $tcp = New-Object Net.Sockets.TcpClient
    $tcp.Connect("github.com", 443)
    Write-Host "   ✓ 端口443连接成功"
    $tcp.Close()
} catch {
    Write-Host "   ✗ 端口443连接失败: $($_.Exception.Message)"
}

Write-Host "========================"
Write-Host "网络测试完成"