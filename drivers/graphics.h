#ifndef GRAPHICS_H
#define GRAPHICS_H

// 颜色定义
#define COLOR_BLACK     0x000000
#define COLOR_WHITE     0xFFFFFF
#define COLOR_RED       0xFF0000
#define COLOR_GREEN     0x00FF00
#define COLOR_BLUE      0x0000FF
#define COLOR_YELLOW    0xFFFF00
#define COLOR_MAGENTA   0xFF00FF
#define COLOR_CYAN      0x00FFFF
#define COLOR_GRAY      0x808080
#define COLOR_DARKGRAY  0x404040
#define COLOR_LIGHTGRAY 0xC0C0C0

// 图形上下文结构
struct graphics_context {
    unsigned int width;         // 宽度（像素）
    unsigned int height;        // 高度（像素）
    unsigned int bpp;           // 每像素位数
    unsigned int pitch;         // 每行字节数
    void* framebuffer;          // 帧缓冲区地址
    unsigned int foreground;    // 前景色
    unsigned int background;    // 背景色
};

// 矩形结构
struct rect {
    int x, y;           // 左上角坐标
    int width, height;  // 宽度和高度
};

// 字体结构
struct font {
    unsigned int width;     // 字符宽度
    unsigned int height;    // 字符高度
    unsigned char* data;    // 字体数据
};

// 窗口结构
struct window {
    struct rect bounds;         // 窗口边界
    char title[64];             // 窗口标题
    unsigned int flags;         // 窗口标志
    unsigned int bg_color;      // 背景色
    struct window* parent;      // 父窗口
    struct window* children;    // 子窗口链表
    struct window* next;        // 链表中的下一个窗口
    void* data;                 // 窗口特定数据
};

// 事件结构
struct event {
    unsigned int type;      // 事件类型
    unsigned int code;      // 事件代码
    int value;              // 事件值
    void* data;             // 事件数据
};

// 事件类型
#define EVENT_MOUSE_MOVE    1
#define EVENT_MOUSE_CLICK   2
#define EVENT_KEY_PRESS     3
#define EVENT_KEY_RELEASE   4
#define EVENT_WINDOW_CLOSE  5
#define EVENT_WINDOW_RESIZE 6

// 函数声明
void gfx_init();
struct graphics_context* gfx_get_context();
void gfx_set_pixel(struct graphics_context* ctx, int x, int y, unsigned int color);
unsigned int gfx_get_pixel(struct graphics_context* ctx, int x, int y);
void gfx_draw_line(struct graphics_context* ctx, int x0, int y0, int x1, int y1, unsigned int color);
void gfx_draw_rect(struct graphics_context* ctx, struct rect* r, unsigned int color);
void gfx_fill_rect(struct graphics_context* ctx, struct rect* r, unsigned int color);
void gfx_draw_circle(struct graphics_context* ctx, int x, int y, int radius, unsigned int color);
void gfx_fill_circle(struct graphics_context* ctx, int x, int y, int radius, unsigned int color);
void gfx_draw_string(struct graphics_context* ctx, int x, int y, const char* str, unsigned int color);
void gfx_set_font(struct font* f);
struct font* gfx_get_font();
void gfx_clear_screen(struct graphics_context* ctx, unsigned int color);

// 窗口管理
struct window* window_create(struct rect* bounds, const char* title, unsigned int flags);
void window_destroy(struct window* win);
void window_show(struct window* win);
void window_hide(struct window* win);
void window_set_title(struct window* win, const char* title);
void window_set_background(struct window* win, unsigned int color);
void window_draw_rect(struct window* win, struct rect* r, unsigned int color);
void window_fill_rect(struct window* win, struct rect* r, unsigned int color);
void window_draw_string(struct window* win, int x, int y, const char* str, unsigned int color);

// 事件处理
void event_init();
int event_poll(struct event* evt);
void event_push(struct event* evt);
void event_register_handler(unsigned int type, void (*handler)(struct event* evt));

// 输入处理
void input_init();
void mouse_set_position(int x, int y);
void mouse_get_position(int* x, int* y);
int keyboard_get_key_state(unsigned int keycode);

#endif