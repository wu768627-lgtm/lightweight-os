#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Bug测试脚本
用于检测操作系统代码中的潜在bug
"""

import os
import re
import sys
from pathlib import Path

class BugTester:
    """Bug测试类"""
    
    def __init__(self, project_root):
        self.project_root = project_root
        self.bugs_found = []
    
    def run_all_tests(self):
        """运行所有bug测试"""
        print("LightweightOS Bug测试")
        print("=" * 50)
        
        # 运行各项测试
        self.test_memory_leaks()
        self.test_buffer_overflows()
        self.test_null_pointer_dereferences()
        self.test_uninitialized_variables()
        self.test_division_by_zero()
        self.test_infinite_loops()
        self.test_race_conditions()
        self.test_deadlocks()
        self.test_resource_leaks()
        
        # 显示结果
        self.print_report()
    
    def test_memory_leaks(self):
        """测试内存泄漏"""
        print("检查内存泄漏...")
        
        # 查找可能的内存泄漏模式
        patterns = [
            (r"malloc\s*\([^)]*\).*?(?!free)", "可能的内存泄漏 - 分配后未释放"),
            (r"allocate_memory\s*\([^)]*\).*?(?!free|deallocate)", "可能的内存泄漏 - 分配后未释放"),
        ]
        
        self.search_patterns(patterns, "内存泄漏")
    
    def test_buffer_overflows(self):
        """测试缓冲区溢出"""
        print("检查缓冲区溢出...")
        
        # 查找可能导致缓冲区溢出的模式
        patterns = [
            (r"\w+\[\d+\][^;]*\[[^;\]]+\]", "可能的缓冲区溢出 - 数组访问"),
            (r"strcpy\s*\([^)]*\)", "可能的缓冲区溢出 - strcpy使用"),
            (r"strcat\s*\([^)]*\)", "可能的缓冲区溢出 - strcat使用"),
            (r"gets\s*\([^)]*\)", "可能的缓冲区溢出 - gets使用"),
            (r"sprintf\s*\([^)]*\)", "可能的缓冲区溢出 - sprintf使用"),
        ]
        
        self.search_patterns(patterns, "缓冲区溢出")
    
    def test_null_pointer_dereferences(self):
        """测试空指针解引用"""
        print("检查空指针解引用...")
        
        # 查找可能导致空指针解引用的模式
        patterns = [
            (r"\w+\s*=\s*NULL\s*;.*?\*\w+", "可能的空指针解引用"),
            (r"(\w+)\s*=\s*malloc\s*\([^)]*\)\s*;.*?(\1)\s*->", "可能的空指针解引用"),
        ]
        
        self.search_patterns(patterns, "空指针解引用")
    
    def test_uninitialized_variables(self):
        """测试未初始化变量"""
        print("检查未初始化变量...")
        
        # 查找可能未初始化的变量
        patterns = [
            (r"(int|char|void|\w+_t)\s+(\w+)\s*;", "可能的未初始化变量"),
        ]
        
        self.search_patterns(patterns, "未初始化变量")
    
    def test_division_by_zero(self):
        """测试除零错误"""
        print("检查除零错误...")
        
        # 查找可能导致除零的模式
        patterns = [
            (r"/\s*0\s*", "可能的除零错误"),
            (r"%\s*0\s*", "可能的模零错误"),
        ]
        
        self.search_patterns(patterns, "除零错误")
    
    def test_infinite_loops(self):
        """测试无限循环"""
        print("检查无限循环...")
        
        # 查找可能的无限循环
        patterns = [
            (r"while\s*\(\s*(1|true)\s*\)", "可能的无限循环"),
            (r"for\s*\([^;]*;\s*;\s*[^)]*\)", "可能的无限循环"),
        ]
        
        self.search_patterns(patterns, "无限循环")
    
    def test_race_conditions(self):
        """测试竞态条件"""
        print("检查竞态条件...")
        
        # 查找可能的竞态条件
        patterns = [
            (r"shared_\w+", "可能的竞态条件 - 共享变量访问"),
            (r"global_\w+", "可能的竞态条件 - 全局变量访问"),
        ]
        
        self.search_patterns(patterns, "竞态条件")
    
    def test_deadlocks(self):
        """测试死锁"""
        print("检查死锁...")
        
        # 查找可能导致死锁的模式
        patterns = [
            (r"lock\s*\([^)]*\).*?lock\s*\([^)]*\)", "可能的死锁 - 多重锁定"),
        ]
        
        self.search_patterns(patterns, "死锁")
    
    def test_resource_leaks(self):
        """测试资源泄漏"""
        print("检查资源泄漏...")
        
        # 查找可能的资源泄漏
        patterns = [
            (r"fopen\s*\([^)]*\).*?(?!fclose)", "可能的文件句柄泄漏"),
            (r"open\s*\([^)]*\).*?(?!close)", "可能的文件描述符泄漏"),
        ]
        
        self.search_patterns(patterns, "资源泄漏")
    
    def search_patterns(self, patterns, bug_type):
        """在代码中搜索特定模式"""
        c_files = Path(self.project_root).glob("**/*.c")
        
        for c_file in c_files:
            try:
                with open(c_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                for pattern, description in patterns:
                    matches = re.finditer(pattern, content, re.IGNORECASE | re.DOTALL)
                    for match in matches:
                        line_number = content[:match.start()].count('\n') + 1
                        self.bugs_found.append({
                            'file': str(c_file),
                            'line': line_number,
                            'type': bug_type,
                            'description': description,
                            'code': match.group(0)[:50] + "..." if len(match.group(0)) > 50 else match.group(0)
                        })
            except Exception as e:
                print(f"  读取文件 {c_file} 时出错: {e}")
    
    def print_report(self):
        """打印bug报告"""
        print("\n" + "=" * 50)
        print("Bug测试报告")
        print("=" * 50)
        
        if not self.bugs_found:
            print("✓ 未发现明显bug")
            return
        
        print(f"发现 {len(self.bugs_found)} 个潜在bug:")
        
        # 按类型分组显示
        bug_types = {}
        for bug in self.bugs_found:
            if bug['type'] not in bug_types:
                bug_types[bug['type']] = []
            bug_types[bug['type']].append(bug)
        
        for bug_type, bugs in bug_types.items():
            print(f"\n{bug_type}:")
            print("-" * 30)
            for bug in bugs:
                print(f"  文件: {bug['file']}")
                print(f"  行号: {bug['line']}")
                print(f"  描述: {bug['description']}")
                print(f"  代码: {bug['code']}")
                print()
        
        print("=" * 50)
        print("建议:")
        print("1. 仔细检查以上标记的代码")
        print("2. 添加适当的错误处理")
        print("3. 增加边界检查")
        print("4. 使用静态分析工具进行更深入的分析")

def main():
    """主函数"""
    # 修复Windows上的编码问题
    if os.name == 'nt':
        os.system('chcp 65001 > nul')
    
    project_root = os.path.dirname(os.path.abspath(__file__))
    
    tester = BugTester(project_root)
    tester.run_all_tests()

if __name__ == "__main__":
    main()