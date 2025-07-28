# boot.s - 系统引导程序

# 设置代码段和数据段寄存器
.code16
.global start

start:
    # 设置堆栈
    mov $0x7C00, %sp
    
    # 设置屏幕模式为文本模式
    mov $0x03, %ah
    int $0x10
    
    # 显示启动信息
    mov $boot_msg, %si
    call print_string
    
    # 启用A20总线
    call enable_a20
    
    # 加载内核到内存
    call load_kernel
    
    # 切换到保护模式
    call switch_to_pm
    
    # 无限循环
    jmp $

# 打印字符串函数
print_string:
    movb (%si), %al
    cmp $0, %al
    je print_done
    movb $0x0E, %ah
    mov $0x07, %bx
    int $0x10
    inc %si
    jmp print_string
print_done:
    ret

# 启用A20总线
enable_a20:
    # 通过键盘控制器启用A20
    call a20wait
    mov $0xAD, %al
    out %al, $0x64
    
    call a20wait
    mov $0xD0, %al
    out %al, $0x64
    
    call a20wait2
    in $0x60, %al
    push %ax
    
    call a20wait
    mov $0xD1, %al
    out %al, $0x64
    
    call a20wait
    pop %ax
    or $2, %al
    out %al, $0x60
    
    call a20wait
    mov $0xAE, %al
    out %al, $0x64
    
    call a20wait
    ret

# 等待键盘控制器就绪
a20wait:
    in $0x64, %al
    test $2, %al
    jnz a20wait
    ret

# 等待键盘控制器输出就绪
a20wait2:
    in $0x64, %al
    test $1, %al
    jz a20wait2
    ret

# 加载内核
load_kernel:
    # 重置磁盘系统
    mov $0, %ah
    mov $0x80, %dl
    int $0x13
    
    # 读取内核扇区
    mov $0x1000, %ax  # 内核加载地址
    mov %ax, %es
    mov $0, %bx       # 偏移量0
    
    mov $0x02, %ah    # 读取扇区功能
    mov $100, %al     # 读取扇区数
    mov $0x80, %dl    # 驱动器号
    mov $0x0002, %cx  # 柱面0, 扇区2
    mov $0x0000, %dh  # 磁头0
    int $0x13
    
    jc load_error
    ret

load_error:
    mov $error_msg, %si
    call print_string
    jmp $

# 切换到保护模式
switch_to_pm:
    # 加载GDT
    lgdt (gdt_descriptor)
    
    # 启用保护模式
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0
    
    # 刷新CPU流水线并设置段寄存器
    ljmp $0x08, $protected_mode

# GDT表
gdt_start:
    .long 0x00000000  # NULL描述符
    .long 0x00000000
    
    .long 0xFFFFFFFF  # 代码段
    .long 0x00CF9A00
    
    .long 0xFFFFFFFF  # 数据段
    .long 0x00CF9200
    
gdt_end:

gdt_descriptor:
    .word gdt_end - gdt_start - 1
    .long gdt_start

.code32
protected_mode:
    # 设置数据段寄存器
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    
    # 设置堆栈
    mov $0x90000, %esp
    
    # 调用内核主函数
    call 0x10000  # 内核入口点
    
    # 如果内核返回，则无限循环
    jmp $

# 消息字符串
boot_msg: .asciz "LightweightOS Booting...\r\n"
error_msg: .asciz "Error loading kernel!\r\n"

# 填充引导扇区
.fill 510 - (. - start), 1, 0
.word 0xAA55  # 引导扇区标识