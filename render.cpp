// render.cpp
// 图形渲染模块

#define _CRT_SECURE_NO_WARNINGS

#include <graphics.h>
#include <stdio.h>
#include <time.h>
#include "render.h"

// 常量定义
#define CELL_SIZE       20
#define GRID_OFFSET_X   50
#define GRID_OFFSET_Y   80

#define COLOR_BG            RGB(30, 30, 40)
#define COLOR_GRID          RGB(60, 60, 80)
#define COLOR_CELL_ALIVE    RGB(0, 255, 150)
#define COLOR_CELL_DEAD     RGB(45, 45, 55)
#define COLOR_TEXT          RGB(200, 200, 220)
#define COLOR_TITLE         RGB(255, 200, 50)      // 改名避免重复
#define COLOR_BORDER        RGB(100, 100, 140)

// 全局变量
static int g_window_width = 800;
static int g_window_height = 600;
static int g_grid_rows = ROWS;
static int g_grid_cols = COLS;
static float g_current_fps = 0.0f;
static int g_frame_count = 0;
static clock_t g_fps_update_time = 0;
static clock_t g_last_frame_time = 0;

// 初始化渲染系统
void initRenderer(int rows, int cols) {
    g_grid_rows = rows;
    g_grid_cols = cols;
    g_window_width = GRID_OFFSET_X * 2 + cols * CELL_SIZE + 200;
    g_window_height = GRID_OFFSET_Y + rows * CELL_SIZE + 50;

    if (g_window_width < 800) {
        g_window_width = 800;
    }
    if (g_window_height < 600) {
        g_window_height = 600;
    }

    initgraph(g_window_width, g_window_height);
    setbkcolor(COLOR_BG);
    cleardevice();

    g_last_frame_time = clock();
    g_fps_update_time = clock();

    BeginBatchDraw();
}

// 关闭渲染系统
void closeRenderer(void) {
    EndBatchDraw();
    closegraph();
}

// 绘制单个细胞
void drawCell(int x, int y, int alive) {
    int px = GRID_OFFSET_X + x * CELL_SIZE;
    int py = GRID_OFFSET_Y + y * CELL_SIZE;

    if (alive) {
        setfillcolor(COLOR_CELL_ALIVE);
        setlinecolor(RGB(0, 200, 120));
        fillroundrect(px + 2, py + 2, px + CELL_SIZE - 2, py + CELL_SIZE - 2, 4, 4);
        // 高光效果
        setfillcolor(RGB(100, 255, 200));
        solidcircle(px + CELL_SIZE / 3, py + CELL_SIZE / 3, 2);
    }
    else {
        setfillcolor(COLOR_CELL_DEAD);
        setlinecolor(COLOR_GRID);
        fillrectangle(px + 1, py + 1, px + CELL_SIZE - 1, py + CELL_SIZE - 1);
    }
}

// 绘制网格
void drawGrid(void) {
    setbkcolor(COLOR_BG);
    cleardevice();

    // 绘制边框
    setlinecolor(COLOR_BORDER);
    setlinestyle(PS_SOLID, 2);
    rectangle(GRID_OFFSET_X - 2, GRID_OFFSET_Y - 2,
        GRID_OFFSET_X + g_grid_cols * CELL_SIZE + 2,
        GRID_OFFSET_Y + g_grid_rows * CELL_SIZE + 2);
    setlinestyle(PS_SOLID, 1);

    // 绘制网格线
    setlinecolor(COLOR_GRID);
    for (int x = 0; x <= g_grid_cols; x++) {
        int px = GRID_OFFSET_X + x * CELL_SIZE;
        line(px, GRID_OFFSET_Y, px, GRID_OFFSET_Y + g_grid_rows * CELL_SIZE);
    }
    for (int y = 0; y <= g_grid_rows; y++) {
        int py = GRID_OFFSET_Y + y * CELL_SIZE;
        line(GRID_OFFSET_X, py, GRID_OFFSET_X + g_grid_cols * CELL_SIZE, py);
    }

    // 绘制坐标轴标记
    settextcolor(RGB(120, 120, 150));
    settextstyle(12, 0, _T("Consolas"));
    TCHAR buf[16];
    for (int x = 0; x < g_grid_cols; x += 5) {
        _stprintf_s(buf, 16, _T("%d"), x);
        outtextxy(GRID_OFFSET_X + x * CELL_SIZE + 6, GRID_OFFSET_Y - 18, buf);
    }
    for (int y = 0; y < g_grid_rows; y += 5) {
        _stprintf_s(buf, 16, _T("%d"), y);
        outtextxy(GRID_OFFSET_X - 25, GRID_OFFSET_Y + y * CELL_SIZE + 4, buf);
    }
}

// 更新FPS
static void update_fps(void) {
    g_frame_count++;
    clock_t current_time = clock();
    float elapsed = (float)(current_time - g_fps_update_time) / CLOCKS_PER_SEC;

    if (elapsed >= 0.5f) {
        g_current_fps = g_frame_count / elapsed;
        g_frame_count = 0;
        g_fps_update_time = current_time;
    }
    g_last_frame_time = current_time;
}

// 绘制UI面板
static void draw_ui_panel(int panel_x, int panel_y, int generation, int is_paused, int live_count) {
    // 面板背景
    setfillcolor(RGB(40, 40, 55));
    setlinecolor(COLOR_BORDER);
    fillroundrect(panel_x, panel_y, panel_x + 160, panel_y + 280, 8, 8);

    // 标题
    settextcolor(COLOR_TITLE);
    settextstyle(18, 0, _T("Consolas"));
    outtextxy(panel_x + 20, panel_y + 15, _T("Game of Life"));

    // 分隔线
    setlinecolor(COLOR_BORDER);
    line(panel_x + 10, panel_y + 45, panel_x + 150, panel_y + 45);

    // 状态信息
    TCHAR buf[64];
    settextcolor(COLOR_TEXT);
    settextstyle(14, 0, _T("Consolas"));

    _stprintf_s(buf, 64, _T("Generation: %d"), generation);
    outtextxy(panel_x + 15, panel_y + 60, buf);

    _stprintf_s(buf, 64, _T("Alive: %d"), live_count);
    outtextxy(panel_x + 15, panel_y + 85, buf);

    // 状态显示
    if (is_paused) {
        settextcolor(RGB(255, 100, 100));
        outtextxy(panel_x + 15, panel_y + 120, _T("[ PAUSED ]"));
    }
    else {
        settextcolor(RGB(100, 255, 100));
        outtextxy(panel_x + 15, panel_y + 120, _T("[ RUNNING ]"));
    }

    // 操作提示
    settextcolor(RGB(150, 150, 180));
    settextstyle(12, 0, _T("Consolas"));
    outtextxy(panel_x + 15, panel_y + 155, _T("Controls:"));
    outtextxy(panel_x + 15, panel_y + 175, _T("SPACE - Pause"));
    outtextxy(panel_x + 15, panel_y + 191, _T("CLICK - Toggle"));
    outtextxy(panel_x + 15, panel_y + 207, _T("R - Random"));
    outtextxy(panel_x + 15, panel_y + 223, _T("C - Clear"));
    outtextxy(panel_x + 15, panel_y + 239, _T("S - Save"));
    outtextxy(panel_x + 15, panel_y + 255, _T("L - Load"));
    outtextxy(panel_x + 15, panel_y + 271, _T("ESC - Exit"));
}

// 主渲染函数
void render(int grid[][COLS], int rows, int cols, int generation, int is_paused) {
    update_fps();
    drawGrid();

    // 绘制所有细胞并统计活细胞数
    int live_count = 0;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            drawCell(x, y, grid[y][x]);
            if (grid[y][x]) {
                live_count++;
            }
        }
    }

    // 绘制UI面板
    int panel_x = GRID_OFFSET_X + g_grid_cols * CELL_SIZE + 30;
    int panel_y = GRID_OFFSET_Y;
    draw_ui_panel(panel_x, panel_y, generation, is_paused, live_count);

    // 绘制FPS
    TCHAR buf[32];
    _stprintf_s(buf, 32, _T("FPS: %.1f"), g_current_fps);
    settextcolor(RGB(100, 200, 100));
    settextstyle(14, 0, _T("Consolas"));
    outtextxy(g_window_width - 100, 10, buf);

    FlushBatchDraw();
}

// 屏幕坐标转网格坐标
int screenToGridX(int screen_x) {
    int grid_x = (screen_x - GRID_OFFSET_X) / CELL_SIZE;
    if (grid_x < 0 || grid_x >= g_grid_cols) {
        return -1;
    }
    return grid_x;
}

int screenToGridY(int screen_y) {
    int grid_y = (screen_y - GRID_OFFSET_Y) / CELL_SIZE;
    if (grid_y < 0 || grid_y >= g_grid_rows) {
        return -1;
    }
    return grid_y;
}

// 获取当前FPS
float getFPS(void) {
    return g_current_fps;
}

// 帧率限制
void limitFPS(int target_fps) {
    if (target_fps <= 0) {
        return;
    }
    clock_t current = clock();
    float frame_time = (float)(current - g_last_frame_time) / CLOCKS_PER_SEC;
    float target_time = 1.0f / target_fps;

    if (frame_time < target_time) {
        Sleep((DWORD)((target_time - frame_time) * 1000));
    }
}