#ifndef STDLIB_H
#define STDLIB_H

// 标准定义

typedef unsigned int size_t;

// 内存管理函数
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);

// 字符串转换函数
int atoi(const char* str);
long atol(const char* str);
double atof(const char* str);

// 伪随机数生成函数
void srand(unsigned int seed);
int rand(void);

// 系统相关函数
void exit(int status);
void abort(void);

#endif