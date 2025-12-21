#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <stdio.h>
#include "global.h"
#include "core.h"
#include "render.h"
#include "input.h"

// 全局变量定义
int grid[100][100] = { 0 };
int paused = 1;
int generation = 0;

// 窗口参数
int GRID_ROWS = 60;
int GRID_COLS = 80;
int CELL_SIZE = 10;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int GRID_OFFSET_X = 0;
int GRID_OFFSET_Y = 50; // 给 UI 留 50 像素高度

// 开关
int show_heatmap = 0;
int show_ai_vision = 0;
int speed_level = 1;
int fullscreen_mode = 0;

// 历史数据
int population_history[HISTORY_LEN] = { 0 };
int history_cursor = 0;

// ----------------------------------------------
// 读取配置
// ----------------------------------------------
void load_config() {
    FILE* fp = fopen("config.ini", "r");
    if (fp == NULL) return;
    char line[100], key[50];
    int value;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%[^=]=%d", key, &value) == 2) {
            if (strcmp(key, "width") == 0) WINDOW_WIDTH = value;
            else if (strcmp(key, "height") == 0) WINDOW_HEIGHT = value;
            else if (strcmp(key, "rows") == 0) GRID_ROWS = value;
            else if (strcmp(key, "cols") == 0) GRID_COLS = value;
            else if (strcmp(key, "cell_size") == 0) CELL_SIZE = value;
        }
    }
    fclose(fp);
    if (GRID_ROWS > 100) GRID_ROWS = 100;
    if (GRID_COLS > 100) GRID_COLS = 100;
}

// ----------------------------------------------
// 记录数据
// ----------------------------------------------
void record_population() {
    int pop = 0;
    for (int i = 0; i < GRID_ROWS; i++)
        for (int j = 0; j < GRID_COLS; j++) if (grid[i][j]) pop++;

    population_history[history_cursor] = pop;
    history_cursor = (history_cursor + 1) % HISTORY_LEN;
}

// ----------------------------------------------
// 自适应网格逻辑
// ----------------------------------------------
void resize_grid() {
    int w = getwidth();
    int h = getheight();
    WINDOW_WIDTH = w; WINDOW_HEIGHT = h;

    int new_cols = w / CELL_SIZE;
    int new_rows = (h - GRID_OFFSET_Y) / CELL_SIZE;

    if (new_cols > 100) new_cols = 100;
    if (new_rows > 100) new_rows = 100;

    GRID_COLS = new_cols;
    GRID_ROWS = new_rows;
}

// ----------------------------------------------
// 主入口
// ----------------------------------------------
int main() {
    load_config();

    // 1. 初始化窗口 (允许System Menu)
    HWND hwnd = initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, 1);

    // 2. 【核心】激活“最大化按钮”和“调整边框”
    // 获取当前窗口样式
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    // 加上“最大化按钮”和“可拖拽边框”属性
    style = style | WS_MAXIMIZEBOX | WS_THICKFRAME;
    // 应用样式
    SetWindowLong(hwnd, GWL_STYLE, style);
    // 刷新窗口框架 (必做，否则按钮不出来)
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    SetWindowText(hwnd, "Conway's Game of Life - Final");
    BeginBatchDraw();

    int last_w = WINDOW_WIDTH;
    int last_h = WINDOW_HEIGHT;

    while (true) {
        // --- 检测窗口变化 ---
        int w = getwidth();
        int h = getheight();
        if (w != last_w || h != last_h) {
            resize_grid();
            last_w = w; last_h = h;
            cleardevice();
        }

        handleInput();

        if (paused == 0) {
            next_Generation();
            generation++;
            record_population();

            int sleep = 100 / speed_level;
            Sleep(sleep > 0 ? sleep : 0);
        }
        else {
            Sleep(10);
        }

        cleardevice();
        render();
        FlushBatchDraw();
    }
    EndBatchDraw();
    closegraph();
    return 0;
}