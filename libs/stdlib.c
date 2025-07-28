#include "stdlib.h"
#include "../kernel/memory.h"

// 分配内存
void* malloc(unsigned int size) {
    return allocate_memory(size);
}

// 释放内存
void free(void* ptr) {
    free_memory(ptr);
}

// 分配并清零内存
void* calloc(unsigned int num, unsigned int size) {
    unsigned int total_size = num * size;
    void* ptr = malloc(total_size);
    
    if (ptr) {
        // 清零分配的内存
        char* bytes = (char*)ptr;
        for (unsigned int i = 0; i < total_size; i++) {
            bytes[i] = 0;
        }
    }
    
    return ptr;
}

// 重新分配内存
void* realloc(void* ptr, unsigned int size) {
    return realloc_memory(ptr, size);
}

// 整数转字符串
void int_to_string(int value, char* str) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    int i = 0;
    int is_negative = 0;

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    char temp[12];
    while (value > 0) {
        temp[i] = (value % 10) + '0';
        value /= 10;
        i++;
    }

    int j = 0;
    if (is_negative) {
        str[j++] = '-';
    }

    for (int k = i - 1; k >= 0; k--) {
        str[j++] = temp[k];
    }
    str[j] = '\0';
}

// 字符串转整数
int atoi(const char* str) {
    if (!str) return 0;
    
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // 处理符号
    if (str[0] == '-') {
        sign = -1;
        i = 1;
    } else if (str[0] == '+') {
        i = 1;
    }
    
    // 转换数字
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return result * sign;
}