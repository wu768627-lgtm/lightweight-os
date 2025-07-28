#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
静态代码分析工具
用于检查操作系统代码的潜在问题
"""

import os
import re
import sys
from pathlib import Path

# 定义代码问题模式
PATTERNS = {
    "memory_leak": r"(malloc|allocate_memory)\s*\([^)]*\).*?(?!free|sys_free)",
    "uninitialized_variable": r"(int|char|void|\w+_t)\s+(\w+)\s*;",
    "buffer_overflow": r"\w+\[\d+\]\s*;.*?\[\w+\]",
    "null_pointer": r"\w+\s*=\s*NULL\s*;.*?\*\w+",
    "infinite_loop": r"while\s*\(\s*(1|true)\s*\)",
    "unused_variable": r"(int|char|void|\w+_t)\s+(\w+)\s*=\s*[^;]+;.*?(?!\\2)",
}

# 严重性级别
SEVERITY = {
    "memory_leak": "HIGH",
    "uninitialized_variable": "MEDIUM",
    "buffer_overflow": "HIGH",
    "null_pointer": "MEDIUM",
    "infinite_loop": "HIGH",
    "unused_variable": "LOW",
}

def analyze_file(file_path):
    """分析单个文件"""
    issues = []
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            
        # 检查每种模式
        for issue_type, pattern in PATTERNS.items():
            matches = re.finditer(pattern, content, re.IGNORECASE | re.DOTALL)
            for match in matches:
                line_number = content[:match.start()].count('\n') + 1
                issues.append({
                    'file': file_path,
                    'line': line_number,
                    'type': issue_type,
                    'severity': SEVERITY.get(issue_type, "UNKNOWN"),
                    'code': match.group(0)[:50] + "..." if len(match.group(0)) > 50 else match.group(0)
                })
    except Exception as e:
        print(f"Error analyzing {file_path}: {e}")
    
    return issues

def analyze_project(root_dir):
    """分析整个项目"""
    all_issues = []
    c_files = Path(root_dir).glob("**/*.c")
    
    print("开始静态代码分析...")
    print("=" * 50)
    
    for c_file in c_files:
        print(f"正在分析: {c_file}")
        issues = analyze_file(c_file)
        all_issues.extend(issues)
    
    return all_issues

def print_report(issues):
    """打印分析报告"""
    if not issues:
        print("OK 未发现潜在问题")
        return
    
    print(f"\n发现 {len(issues)} 个潜在问题:")
    print("=" * 50)
    
    # 按严重性分组
    high_issues = [i for i in issues if i['severity'] == 'HIGH']
    medium_issues = [i for i in issues if i['severity'] == 'MEDIUM']
    low_issues = [i for i in issues if i['severity'] == 'LOW']
    
    print(f"\n严重问题 (HIGH): {len(high_issues)}")
    for issue in high_issues:
        print(f"  文件: {issue['file']}")
        print(f"  行号: {issue['line']}")
        print(f"  类型: {issue['type']}")
        print(f"  代码: {issue['code']}")
        print()
    
    print(f"中等问题 (MEDIUM): {len(medium_issues)}")
    for issue in medium_issues:
        print(f"  文件: {issue['file']}")
        print(f"  行号: {issue['line']}")
        print(f"  类型: {issue['type']}")
        print(f"  代码: {issue['code']}")
        print()
    
    print(f"轻微问题 (LOW): {len(low_issues)}")
    for issue in low_issues:
        print(f"  文件: {issue['file']}")
        print(f"  行号: {issue['line']}")
        print(f"  类型: {issue['type']}")
        print(f"  代码: {issue['code']}")
        print()

def check_project_structure(root_dir):
    """检查项目结构"""
    print("检查项目结构...")
    print("=" * 50)
    
    required_dirs = ['kernel', 'boot', 'drivers', 'libs', 'userland', 'tools', 'docs']
    required_files = ['Makefile', 'README.md']
    
    missing_dirs = []
    missing_files = []
    
    for dir_name in required_dirs:
        if not os.path.exists(os.path.join(root_dir, dir_name)):
            missing_dirs.append(dir_name)
    
    for file_name in required_files:
        if not os.path.exists(os.path.join(root_dir, file_name)):
            missing_files.append(file_name)
    
    if missing_dirs:
        print(f"缺失目录: {', '.join(missing_dirs)}")
    else:
        print("OK 所有必需目录都存在")
    
    if missing_files:
        print(f"缺失文件: {', '.join(missing_files)}")
    else:
        print("OK 所有必需文件都存在")
    
    print()

def check_kernel_modules(root_dir):
    """检查内核模块"""
    print("检查内核模块...")
    print("=" * 50)
    
    kernel_dir = os.path.join(root_dir, 'kernel')
    if not os.path.exists(kernel_dir):
        print("ERROR 未找到内核目录")
        return
    
    required_modules = [
        'kernel.c', 'memory.c', 'process.c', 'interrupts.c', 
        'syscall.c', 'profiling.c', 'security.c', 'vm.c',
        'scheduler.c', 'logger.c', 'config.c', 'exception.c', 
        'power.c', 'test.c'
    ]
    
    missing_modules = []
    for module in required_modules:
        if not os.path.exists(os.path.join(kernel_dir, module)):
            missing_modules.append(module)
    
    if missing_modules:
        print(f"缺失内核模块: {', '.join(missing_modules)}")
    else:
        print("OK 所有必需内核模块都存在")
    
    print()

def main():
    """主函数"""
    # 修复Windows上的编码问题
    if os.name == 'nt':
        os.system('chcp 65001 > nul')
    project_root = os.path.dirname(os.path.abspath(__file__))
    
    print("LightweightOS 静态代码分析工具")
    print("=" * 50)
    
    # 检查项目结构
    check_project_structure(project_root)
    
    # 检查内核模块
    check_kernel_modules(project_root)
    
    # 执行静态分析
    issues = analyze_project(project_root)
    
    # 打印报告
    print_report(issues)
    
    print("静态代码分析完成。")

if __name__ == "__main__":
    # 修复Windows上的编码问题
    if os.name == 'nt':
        os.system('chcp 65001 > nul')
    main()