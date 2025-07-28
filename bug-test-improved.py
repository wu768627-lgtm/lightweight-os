#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
改进的Bug测试脚本
用于检测操作系统代码中的潜在bug，减少误报
"""

import os
import re
import sys
from pathlib import Path

class ImprovedBugTester:
    """改进的Bug测试类"""
    
    def __init__(self, project_root):
        self.project_root = project_root
        self.bugs_found = []
        # 定义需要忽略的文件模式
        self.ignore_patterns = [
            "test.c",  # 测试文件
            "stress.c",  # 压力测试文件
        ]
    
    def run_all_tests(self):
        """运行所有bug测试"""
        print("LightweightOS 改进版Bug测试")
        print("=" * 50)
        
        # 运行各项测试
        self.test_real_memory_leaks()
        self.test_buffer_overflows()
        self.test_null_pointer_dereferences()
        self.test_division_by_zero()
        self.test_double_free()
        
        # 显示结果
        self.print_report()
    
    def should_ignore_file(self, file_path):
        """检查是否应该忽略该文件"""
        for pattern in self.ignore_patterns:
            if pattern in file_path:
                return True
        return False
    
    def test_real_memory_leaks(self):
        """测试真正的内存泄漏（分配后没有对应的释放）"""
        print("检查真正的内存泄漏...")
        
        c_files = Path(self.project_root).glob("**/*.c")
        
        for c_file in c_files:
            if self.should_ignore_file(str(c_file)):
                continue
                
            try:
                with open(c_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                # 查找分配内存但没有释放的模式
                # 查找单独的 malloc/allocate_memory 调用（不在函数中）
                malloc_patterns = [
                    (r"malloc\s*\(\s*(\w+)\s*\)", "malloc"),
                    (r"allocate_memory\s*\(\s*(\w+)\s*\)", "allocate_memory"),
                ]
                
                for pattern, alloc_func in malloc_patterns:
                    matches = re.finditer(pattern, content, re.IGNORECASE)
                    for match in matches:
                        line_number = content[:match.start()].count('\n') + 1
                        
                        # 检查同一函数中是否有对应的 free 调用
                        # 找到函数范围
                        func_start = content.rfind("{", 0, match.start())
                        func_end = content.find("}", match.end())
                        
                        if func_start != -1 and func_end != -1:
                            func_content = content[func_start:func_end]
                            # 检查函数中是否有释放调用
                            if "free" not in func_content and "free_memory" not in func_content:
                                self.bugs_found.append({
                                    'file': str(c_file),
                                    'line': line_number,
                                    'type': '内存泄漏',
                                    'description': f'分配内存后未释放 ({alloc_func})',
                                    'code': match.group(0)[:50] + "..." if len(match.group(0)) > 50 else match.group(0)
                                })
            except Exception as e:
                print(f"  读取文件 {c_file} 时出错: {e}")
    
    def test_buffer_overflows(self):
        """测试缓冲区溢出"""
        print("检查缓冲区溢出...")
        
        c_files = Path(self.project_root).glob("**/*.c")
        
        dangerous_functions = [
            (r"strcpy\s*\(", "使用了不安全的strcpy函数"),
            (r"strcat\s*\(", "使用了不安全的strcat函数"),
            (r"gets\s*\(", "使用了不安全的gets函数"),
            (r"sprintf\s*\(", "使用了不安全的sprintf函数"),
        ]
        
        for c_file in c_files:
            if self.should_ignore_file(str(c_file)):
                continue
                
            try:
                with open(c_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                for pattern, description in dangerous_functions:
                    matches = re.finditer(pattern, content, re.IGNORECASE)
                    for match in matches:
                        line_number = content[:match.start()].count('\n') + 1
                        self.bugs_found.append({
                            'file': str(c_file),
                            'line': line_number,
                            'type': '缓冲区溢出',
                            'description': description,
                            'code': match.group(0)[:50] + "..." if len(match.group(0)) > 50 else match.group(0)
                        })
            except Exception as e:
                print(f"  读取文件 {c_file} 时出错: {e}")
    
    def test_null_pointer_dereferences(self):
        """测试空指针解引用"""
        print("检查空指针解引用...")
        
        c_files = Path(self.project_root).glob("**/*.c")
        
        patterns = [
            (r"(\w+)\s*=\s*NULL\s*;.*?\1->", "可能的空指针解引用"),
            (r"(\w+)\s*=\s*0\s*;.*?\1->", "可能的空指针解引用"),
        ]
        
        for c_file in c_files:
            if self.should_ignore_file(str(c_file)):
                continue
                
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
                            'type': '空指针解引用',
                            'description': description,
                            'code': match.group(0)[:50] + "..." if len(match.group(0)) > 50 else match.group(0)
                        })
            except Exception as e:
                print(f"  读取文件 {c_file} 时出错: {e}")
    
    def test_division_by_zero(self):
        """测试除零错误"""
        print("检查除零错误...")
        
        c_files = Path(self.project_root).glob("**/*.c")
        
        patterns = [
            (r"/\s*0\s*(?![xX0-9a-fA-F])", "可能的除零错误"),
            (r"%\s*0\s*(?![xX0-9a-fA-F])", "可能的模零错误"),
        ]
        
        for c_file in c_files:
            if self.should_ignore_file(str(c_file)):
                continue
                
            try:
                with open(c_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                for pattern, description in patterns:
                    matches = re.finditer(pattern, content)
                    for match in matches:
                        line_number = content[:match.start()].count('\n') + 1
                        # 检查是否在注释中
                        line_start = content.rfind('\n', 0, match.start()) + 1
                        line_end = content.find('\n', match.end())
                        line = content[line_start:line_end] if line_end != -1 else content[line_start:]
                        
                        if '//' not in line or line.find('//') > line.find(match.group(0)):
                            self.bugs_found.append({
                                'file': str(c_file),
                                'line': line_number,
                                'type': '除零错误',
                                'description': description,
                                'code': match.group(0)[:50] + "..." if len(match.group(0)) > 50 else match.group(0)
                            })
            except Exception as e:
                print(f"  读取文件 {c_file} 时出错: {e}")
    
    def test_double_free(self):
        """测试双重释放内存"""
        print("检查双重释放内存...")
        
        c_files = Path(self.project_root).glob("**/*.c")
        
        for c_file in c_files:
            if self.should_ignore_file(str(c_file)):
                continue
                
            try:
                with open(c_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                # 查找连续的free调用
                free_patterns = [
                    (r"free\s*\(\s*(\w+)\s*\).*?free\s*\(\s*\1\s*\)", "可能的双重释放"),
                    (r"free_memory\s*\(\s*(\w+)\s*\).*?free_memory\s*\(\s*\1\s*\)", "可能的双重释放"),
                ]
                
                for pattern, description in free_patterns:
                    matches = re.finditer(pattern, content, re.IGNORECASE | re.DOTALL)
                    for match in matches:
                        line_number = content[:match.start()].count('\n') + 1
                        self.bugs_found.append({
                            'file': str(c_file),
                            'line': line_number,
                            'type': '双重释放',
                            'description': description,
                            'code': match.group(0)[:50] + "..." if len(match.group(0)) > 50 else match.group(0)
                        })
            except Exception as e:
                print(f"  读取文件 {c_file} 时出错: {e}")
    
    def print_report(self):
        """打印bug报告"""
        print("\n" + "=" * 50)
        print("改进版Bug测试报告")
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
    
    tester = ImprovedBugTester(project_root)
    tester.run_all_tests()

if __name__ == "__main__":
    main()