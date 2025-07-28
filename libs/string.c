#include "string.h"

// 字符串复制函数
char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* ret = dest;
    while (n-- && (*dest++ = *src++))
        ;
    return ret;
}

// 字符串连接函数
char* strcat(char* dest, const char* src) {
    char* ret = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
    return ret;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* ret = dest;
    while (*dest)
        dest++;
    while (n-- && (*dest++ = *src++))
        ;
    *dest = '\0';
    return ret;
}

// 字符串比较函数
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

int strncmp(const char* str1, const char* str2, size_t n) {
    while (n && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// 字符串搜索函数
char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == c)
            return (char*)str;
        str++;
    }
    return 0;
}

char* strrchr(const char* str, int c) {
    char* last = 0;
    while (*str) {
        if (*str == c)
            last = (char*)str;
        str++;
    }
    return last;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    
    for (; *haystack; haystack++) {
        if (*haystack == *needle && strncmp(haystack, needle, strlen(needle)) == 0) {
            return (char*)haystack;
        }
    }
    return 0;
}

// 内存操作函数
void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (n--)
        *d++ = *s++;
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    
    if (d < s) {
        while (n--)
            *d++ = *s++;
    } else {
        d += n;
        s += n;
        while (n--)
            *--d = *--s;
    }
    return dest;
}

void* memset(void* ptr, int value, size_t n) {
    unsigned char* p = (unsigned char*)ptr;
    unsigned char v = (unsigned char)value;
    while (n--)
        *p++ = v;
    return ptr;
}

int memcmp(const void* ptr1, const void* ptr2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    
    while (n--) {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

void* memchr(const void* ptr, int value, size_t n) {
    const unsigned char* p = (const unsigned char*)ptr;
    unsigned char v = (unsigned char)value;
    
    while (n--) {
        if (*p == v)
            return (void*)p;
        p++;
    }
    return 0;
}

// 其他字符串函数
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

size_t strnlen(const char* str, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && str[len])
        len++;
    return len;
}