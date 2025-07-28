#ifndef STRING_H
#define STRING_H

// 字符串处理函数

typedef unsigned int size_t;

// 字符串复制函数
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);

// 字符串连接函数
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);

// 字符串比较函数
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);

// 字符串搜索函数
char* strchr(const char* str, int c);
char* strrchr(const char* str, int c);
char* strstr(const char* haystack, const char* needle);

// 内存操作函数
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void* memset(void* ptr, int value, size_t n);
int memcmp(const void* ptr1, const void* ptr2, size_t n);
void* memchr(const void* ptr, int value, size_t n);

// 其他字符串函数
size_t strlen(const char* str);
size_t strnlen(const char* str, size_t maxlen);

#endif