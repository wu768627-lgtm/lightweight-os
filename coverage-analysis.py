#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
代码覆盖率分析工具
用于分析操作系统代码的模块覆盖情况
"""

import os
import sys
from pathlib import Path

class CoverageAnalyzer:
    """覆盖率分析器"""
    
    def __init__(self, project_root):
        self.project_root = project_root
        self.modules = {}
        self.analyze_modules()
    
    def analyze_modules(self):
        """分析项目模块"""
        # 内核模块
        kernel_dir = os.path.join(self.project_root, 'kernel')
        if os.path.exists(kernel_dir):
            for file in os.listdir(kernel_dir):
                if file.endswith('.c'):
                    module_name = file[:-2]  # 移除 .c 扩展名
                    self.modules[f'kernel/{module_name}'] = {
                        'implemented': True,
                        'tested': self.is_module_tested(f'kernel/{file}'),
                        'size': self.get_file_size(os.path.join(kernel_dir, file))
                    }
        
        # 驱动模块
        drivers_dir = os.path.join(self.project_root, 'drivers')
        if os.path.exists(drivers_dir):
            for file in os.listdir(drivers_dir):
                if file.endswith('.c'):
                    module_name = file[:-2]  # 移除 .c 扩展名
                    self.modules[f'drivers/{module_name}'] = {
                        'implemented': True,
                        'tested': self.is_module_tested(f'drivers/{file}'),
                        'size': self.get_file_size(os.path.join(drivers_dir, file))
                    }
        
        # 用户空间模块
        userland_dir = os.path.join(self.project_root, 'userland')
        if os.path.exists(userland_dir):
            for file in os.listdir(userland_dir):
                if file.endswith('.c'):
                    module_name = file[:-2]  # 移除 .c 扩展名
                    self.modules[f'userland/{module_name}'] = {
                        'implemented': True,
                        'tested': self.is_module_tested(f'userland/{file}'),
                        'size': self.get_file_size(os.path.join(userland_dir, file))
                    }
        
        # 工具模块
        tools_dir = os.path.join(self.project_root, 'tools')
        if os.path.exists(tools_dir):
            for file in os.listdir(tools_dir):
                if file.endswith('.c'):
                    module_name = file[:-2]  # 移除 .c 扩展名
                    self.modules[f'tools/{module_name}'] = {
                        'implemented': True,
                        'tested': self.is_module_tested(f'tools/{file}'),
                        'size': self.get_file_size(os.path.join(tools_dir, file))
                    }
    
    def is_module_tested(self, module_path):
        """检查模块是否被测试"""
        # 简单检查：查看模块名是否在测试文件中被提及
        test_files = [
            os.path.join(self.project_root, 'kernel', 'test.c'),
            os.path.join(self.project_root, 'userland', 'test.c')
        ]
        
        module_name = module_path.split('/')[-1].replace('.c', '')
        
        for test_file in test_files:
            if os.path.exists(test_file):
                try:
                    with open(test_file, 'r', encoding='utf-8') as f:
                        content = f.read()
                        if module_name in content:
                            return True
                except:
                    pass
        
        return False
    
    def get_file_size(self, file_path):
        """获取文件大小"""
        try:
            return os.path.getsize(file_path)
        except:
            return 0
    
    def print_coverage_report(self):
        """打印覆盖率报告"""
        print("LightweightOS 代码覆盖率分析报告")
        print("=" * 60)
        
        # 按目录分组显示
        categories = {
            '内核模块': [m for m in self.modules.keys() if m.startswith('kernel/')],
            '驱动模块': [m for m in self.modules.keys() if m.startswith('drivers/')],
            '用户空间模块': [m for m in self.modules.keys() if m.startswith('userland/')],
            '工具模块': [m for m in self.modules.keys() if m.startswith('tools/')]
        }
        
        total_modules = len(self.modules)
        implemented_modules = sum(1 for m in self.modules.values() if m['implemented'])
        tested_modules = sum(1 for m in self.modules.values() if m['tested'])
        
        for category, modules in categories.items():
            if not modules:
                continue
                
            print(f"\n{category}:")
            print("-" * 30)
            
            category_tested = 0
            for module in sorted(modules):
                info = self.modules[module]
                status = "[OK]" if info['tested'] else "[  ]"
                print(f"  {status} {module.split('/')[-1]:<15} ({info['size']} bytes)")
                if info['tested']:
                    category_tested += 1
            
            print(f"  覆盖率: {category_tested}/{len(modules)} ({100*category_tested/len(modules):.1f}%)")
        
        print("\n" + "=" * 60)
        print("总体统计:")
        print(f"  总模块数: {total_modules}")
        print(f"  已实现模块: {implemented_modules}")
        print(f"  已测试模块: {tested_modules}")
        print(f"  测试覆盖率: {100*tested_modules/total_modules:.1f}%")
        
        if tested_modules == total_modules:
            print("\nOK 100% 代码覆盖率!")
        elif tested_modules >= total_modules * 0.8:
            print(f"\nGOOD 良好的测试覆盖率 ({100*tested_modules/total_modules:.1f}%)")
        else:
            print(f"\nWARNING 需要增加测试 ({100*tested_modules/total_modules:.1f}% 覆盖率)")

def main():
    """主函数"""
    # 修复Windows上的编码问题
    if os.name == 'nt':
        os.system('chcp 65001 > nul')
    
    project_root = os.path.dirname(os.path.abspath(__file__))
    
    analyzer = CoverageAnalyzer(project_root)
    analyzer.print_coverage_report()

if __name__ == "__main__":
    main()