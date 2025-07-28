#include "graphics.h"
#include "../kernel/kernel.h"
#include "../libs/string.h"

// 全局图形上下文
static struct graphics_context g_context;

// 当前字体
static struct font* g_font = 0;

// 窗口链表
static struct window* g_windows = 0;

// 事件处理相关
static struct event g_event_queue[256];
static int g_event_head = 0;
static int g_event_tail = 0;

// 初始化图形子系统
void gfx_init() {
    print_string("Initializing graphics subsystem...\n");
    
    // 初始化图形上下文
    // 在实际实现中，这些值应该从显示设备获取
    g_context.width = 1024;
    g_context.height = 768;
    g_context.bpp = 32;
    g_context.pitch = g_context.width * 4;
    g_context.framebuffer = (void*)0xE0000000; // 假设的帧缓冲区地址
    g_context.foreground = COLOR_WHITE;
    g_context.background = COLOR_BLACK;
    
    // 初始化输入子系统
    input_init();
    
    // 初始化事件系统
    event_init();
    
    print_string("Graphics subsystem initialized.\n");
}

// 获取图形上下文
struct graphics_context* gfx_get_context() {
    return &g_context;
}

// 设置像素
void gfx_set_pixel(struct graphics_context* ctx, int x, int y, unsigned int color) {
    if (!ctx || x < 0 || y < 0 || x >= (int)ctx->width || y >= (int)ctx->height) {
        return;
    }
    
    unsigned int* pixel = (unsigned int*)((char*)ctx->framebuffer + y * ctx->pitch + x * (ctx->bpp / 8));
    *pixel = color;
}

// 获取像素
unsigned int gfx_get_pixel(struct graphics_context* ctx, int x, int y) {
    if (!ctx || x < 0 || y < 0 || x >= (int)ctx->width || y >= (int)ctx->height) {
        return 0;
    }
    
    unsigned int* pixel = (unsigned int*)((char*)ctx->framebuffer + y * ctx->pitch + x * (ctx->bpp / 8));
    return *pixel;
}

// 绘制直线
void gfx_draw_line(struct graphics_context* ctx, int x0, int y0, int x1, int y1, unsigned int color) {
    // 使用Bresenham算法绘制直线
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = (dx > 0) ? 1 : -1;
    int sy = (dy > 0) ? 1 : -1;
    dx = (dx < 0) ? -dx : dx;
    dy = (dy < 0) ? -dy : dy;
    
    int err = dx - dy;
    
    while (1) {
        gfx_set_pixel(ctx, x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// 绘制矩形
void gfx_draw_rect(struct graphics_context* ctx, struct rect* r, unsigned int color) {
    if (!ctx || !r) return;
    
    // 绘制四条边
    gfx_draw_line(ctx, r->x, r->y, r->x + r->width - 1, r->y, color);
    gfx_draw_line(ctx, r->x + r->width - 1, r->y, r->x + r->width - 1, r->y + r->height - 1, color);
    gfx_draw_line(ctx, r->x + r->width - 1, r->y + r->height - 1, r->x, r->y + r->height - 1, color);
    gfx_draw_line(ctx, r->x, r->y + r->height - 1, r->x, r->y, color);
}

// 填充矩形
void gfx_fill_rect(struct graphics_context* ctx, struct rect* r, unsigned int color) {
    if (!ctx || !r) return;
    
    for (int y = r->y; y < r->y + (int)r->height; y++) {
        for (int x = r->x; x < r->x + (int)r->width; x++) {
            gfx_set_pixel(ctx, x, y, color);
        }
    }
}

// 绘制圆形
void gfx_draw_circle(struct graphics_context* ctx, int x, int y, int radius, unsigned int color) {
    if (!ctx || radius <= 0) return;
    
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int xi = 0;
    int yi = radius;
    
    gfx_set_pixel(ctx, x, y + radius, color);
    gfx_set_pixel(ctx, x, y - radius, color);
    gfx_set_pixel(ctx, x + radius, y, color);
    gfx_set_pixel(ctx, x - radius, y, color);
    
    while (xi < yi) {
        if (f >= 0) {
            yi--;
            ddF_y += 2;
            f += ddF_y;
        }
        xi++;
        ddF_x += 2;
        f += ddF_x;
        
        gfx_set_pixel(ctx, x + xi, y + yi, color);
        gfx_set_pixel(ctx, x - xi, y + yi, color);
        gfx_set_pixel(ctx, x + xi, y - yi, color);
        gfx_set_pixel(ctx, x - xi, y - yi, color);
        gfx_set_pixel(ctx, x + yi, y + xi, color);
        gfx_set_pixel(ctx, x - yi, y + xi, color);
        gfx_set_pixel(ctx, x + yi, y - xi, color);
        gfx_set_pixel(ctx, x - yi, y - xi, color);
    }
}

// 填充圆形
void gfx_fill_circle(struct graphics_context* ctx, int x, int y, int radius, unsigned int color) {
    if (!ctx || radius <= 0) return;
    
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                gfx_set_pixel(ctx, x + dx, y + dy, color);
            }
        }
    }
}

// 绘制字符串
void gfx_draw_string(struct graphics_context* ctx, int x, int y, const char* str, unsigned int color) {
    if (!ctx || !str) return;
    
    int orig_x = x;
    while (*str) {
        if (*str == '\n') {
            x = orig_x;
            y += 16; // 假设字符高度为16
        } else {
            // 在实际实现中，这里会根据字体数据绘制字符
            // 简化实现，只绘制字符占位符
            struct rect r = {x, y, 8, 16}; // 假设字符宽度为8，高度为16
            gfx_fill_rect(ctx, &r, color);
            x += 8;
        }
        str++;
    }
}

// 设置字体
void gfx_set_font(struct font* f) {
    g_font = f;
}

// 获取字体
struct font* gfx_get_font() {
    return g_font;
}

// 清屏
void gfx_clear_screen(struct graphics_context* ctx, unsigned int color) {
    if (!ctx) return;
    
    struct rect r = {0, 0, ctx->width, ctx->height};
    gfx_fill_rect(ctx, &r, color);
}

// 创建窗口
struct window* window_create(struct rect* bounds, const char* title, unsigned int flags) {
    // 在实际实现中，这里会分配内存并初始化窗口结构
    print_string("Creating window: ");
    print_string((char*)title);
    print_string("\n");
    
    return 0; // 简化实现
}

// 销毁窗口
void window_destroy(struct window* win) {
    if (!win) return;
    
    print_string("Destroying window\n");
    // 在实际实现中，这里会释放窗口资源
}

// 显示窗口
void window_show(struct window* win) {
    if (!win) return;
    
    print_string("Showing window\n");
    // 在实际实现中，这里会将窗口添加到显示列表
}

// 隐藏窗口
void window_hide(struct window* win) {
    if (!win) return;
    
    print_string("Hiding window\n");
    // 在实际实现中，这里会将窗口从显示列表移除
}

// 设置窗口标题
void window_set_title(struct window* win, const char* title) {
    if (!win || !title) return;
    
    // 在实际实现中，这里会更新窗口标题
    print_string("Setting window title: ");
    print_string((char*)title);
    print_string("\n");
}

// 设置窗口背景色
void window_set_background(struct window* win, unsigned int color) {
    if (!win) return;
    
    win->bg_color = color;
    print_string("Setting window background color\n");
}

// 在窗口中绘制矩形
void window_draw_rect(struct window* win, struct rect* r, unsigned int color) {
    if (!win || !r) return;
    
    print_string("Drawing rectangle in window\n");
    // 在实际实现中，这里会在窗口中绘制矩形
}

// 在窗口中填充矩形
void window_fill_rect(struct window* win, struct rect* r, unsigned int color) {
    if (!win || !r) return;
    
    print_string("Filling rectangle in window\n");
    // 在实际实现中，这里会在窗口中填充矩形
}

// 在窗口中绘制字符串
void window_draw_string(struct window* win, int x, int y, const char* str, unsigned int color) {
    if (!win || !str) return;
    
    print_string("Drawing string in window: ");
    print_string((char*)str);
    print_string("\n");
    // 在实际实现中，这里会在窗口中绘制字符串
}

// 初始化事件系统
void event_init() {
    g_event_head = 0;
    g_event_tail = 0;
    print_string("Event system initialized.\n");
}

// 轮询事件
int event_poll(struct event* evt) {
    if (!evt) return -1;
    
    // 检查事件队列是否为空
    if (g_event_head == g_event_tail) {
        return 0; // 没有事件
    }
    
    // 从队列中取出事件
    *evt = g_event_queue[g_event_head];
    g_event_head = (g_event_head + 1) % 256;
    
    return 1; // 成功获取事件
}

// 推入事件
void event_push(struct event* evt) {
    if (!evt) return;
    
    // 检查事件队列是否已满
    int next_tail = (g_event_tail + 1) % 256;
    if (next_tail == g_event_head) {
        print_string("Event queue full, dropping event.\n");
        return;
    }
    
    // 将事件添加到队列
    g_event_queue[g_event_tail] = *evt;
    g_event_tail = next_tail;
}

// 注册事件处理程序
void event_register_handler(unsigned int type, void (*handler)(struct event* evt)) {
    print_string("Registering event handler for type: ");
    char type_str[12];
    int_to_string(type, type_str);
    print_string(type_str);
    print_string("\n");
    // 在实际实现中，这里会注册事件处理程序
}

// 初始化输入子系统
void input_init() {
    print_string("Input subsystem initialized.\n");
}

// 设置鼠标位置
void mouse_set_position(int x, int y) {
    print_string("Setting mouse position: (");
    char x_str[12], y_str[12];
    int_to_string(x, x_str);
    int_to_string(y, y_str);
    print_string(x_str);
    print_string(", ");
    print_string(y_str);
    print_string(")\n");
    // 在实际实现中，这里会设置鼠标位置
}

// 获取鼠标位置
void mouse_get_position(int* x, int* y) {
    if (x) *x = 0;
    if (y) *y = 0;
    // 在实际实现中，这里会获取鼠标位置
}

// 获取键盘按键状态
int keyboard_get_key_state(unsigned int keycode) {
    // 在实际实现中，这里会获取键盘按键状态
    return 0;
}

// 整数转字符串辅助函数
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