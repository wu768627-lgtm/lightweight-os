; LightweightOS 简单引导程序
; 16位实模式，显示消息后停止

BITS 16
ORG 0x7c00

start:
    ; 初始化段寄存器
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; 设置光标位置
    mov ah, 0x02
    mov bh, 0
    mov dh, 0
    mov dl, 0
    int 0x10
    
    ; 显示启动消息
    mov si, boot_msg
    call print_string
    
    ; 等待按键
    mov ah, 0x00
    int 0x16
    
    ; 重启系统
    mov ax, 0
    int 0x19
    
    ; 无限循环
    jmp $

; 打印字符串函数
print_string:
    lodsb
    cmp al, 0
    je print_done
    mov ah, 0x0E
    mov bx, 0x07
    int 0x10
    jmp print_string
print_done:
    ret

; 消息字符串
boot_msg db 'LightweightOS Booting...', 0x0D, 0x0A, 0x0A, 'Press any key to restart...', 0x0D, 0x0A, 0

; 填充引导扇区
times 510-($-$$) db 0
dw 0xAA55  ; 引导扇区标识