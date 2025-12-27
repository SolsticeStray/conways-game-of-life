#define _CRT_SECURE_NO_WARNINGS

#include <graphics.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <string.h>

#include "global.h"
#include "core.h"
#include "render.h"
#include "input.h"
#include "pattern_detect.h"
// ==========================================
//  1. 全局变量定义
// ==========================================

int grid[100][100] = { 0 };
int heatmap[100][100] = { 0 };           // 新增
int neighbor_density[100][100] = { 0 };   // 新增
int paused = 1;
int generation = 0;

int GRID_ROWS = 60;
int GRID_COLS = 80;
int CELL_SIZE = 12;
int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 768;

int GRID_OFFSET_X = 0;
int GRID_OFFSET_Y = 50;

int show_heatmap = 0;
int heatmap_mode = 0;                     // 新增
int show_ai_vision = 0;
int speed_level = 1;
int fullscreen_mode = 0;

int population_history[HISTORY_LEN] = { 0 };
int history_cursor = 0;

// ==========================================
//  2. 高精度计时器 (修复版)
// ==========================================
typedef struct {
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER last_frame;
} Timer;

Timer sys_timer;

void timer_init(Timer* t) {
    QueryPerformanceFrequency(&t->frequency);
    QueryPerformanceCounter(&t->start);
    t->last_frame = t->start;
}

// 修复：获取从上一帧到现在的时间，并更新 last_frame
float timer_get_delta(Timer* t) {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    long long elapsed = current.QuadPart - t->last_frame.QuadPart;
    t->last_frame = current;
    return (float)elapsed / (float)t->frequency.QuadPart;
}

float timer_get_total(Timer* t) {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    return (float)(current.QuadPart - t->start.QuadPart) / (float)t->frequency.QuadPart;
}

// 新增：获取当前时间点（不更新 last_frame）
float timer_peek_elapsed(Timer* t) {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    long long elapsed = current.QuadPart - t->last_frame.QuadPart;
    return (float)elapsed / (float)t->frequency.QuadPart;
}

// ==========================================
//  3. 配置加载
// ==========================================
void trim_string(char* str) {
    char* p = strchr(str, '\n');
    if (p) *p = 0;
    p = strchr(str, '\r');
    if (p) *p = 0;
}

void sys_load_config() {
    FILE* fp = fopen("config.ini", "r");
    if (fp == NULL) {
        printf("Config not found, using defaults.\n");
        return;
    }

    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        trim_string(line);
        if (strlen(line) == 0 || line[0] == ';' || line[0] == '#') continue;

        char key[64], val_str[64];
        if (sscanf(line, "%[^=]=%s", key, val_str) == 2) {
            int val = atoi(val_str);
            if (strcmp(key, "width") == 0) WINDOW_WIDTH = val;
            else if (strcmp(key, "height") == 0) WINDOW_HEIGHT = val;
            else if (strcmp(key, "rows") == 0) {
                GRID_ROWS = (val > 100) ? 100 : val;
            }
            else if (strcmp(key, "cols") == 0) {
                GRID_COLS = (val > 100) ? 100 : val;
            }
            else if (strcmp(key, "cell_size") == 0) CELL_SIZE = val;
            else if (strcmp(key, "fullscreen") == 0) fullscreen_mode = val;
        }
    }
    fclose(fp);
}

// ==========================================
//  4. 系统初始化
// ==========================================
void sys_init() {
    sys_load_config();

    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, EX_SHOWCONSOLE);

    HWND hwnd = GetHWnd();
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style |= (WS_MAXIMIZEBOX | WS_THICKFRAME);
    SetWindowLong(hwnd, GWL_STYLE, style);

    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    SetWindowText(hwnd, _T("Conway's Game of Life - Ultimate Edition"));

    core_init();
    initInput();
    initRenderer();
    init_pattern_detector();
    timer_init(&sys_timer);

    printf("[System] Initialization Complete.\n");
    printf("[System] Resolution: %dx%d, Grid: %dx%d\n",
        WINDOW_WIDTH, WINDOW_HEIGHT, GRID_ROWS, GRID_COLS);
}

// ==========================================
//  5. 逻辑更新
// ==========================================
float simulation_timer = 0.0f;
float stats_timer = 0.0f;

void sys_update(float delta_time) {
    static int last_w = 0;
    static int last_h = 0;

    // 首次初始化
    if (last_w == 0) last_w = WINDOW_WIDTH;
    if (last_h == 0) last_h = WINDOW_HEIGHT;

    int cur_w = getwidth();
    int cur_h = getheight();

    if (cur_w != last_w || cur_h != last_h) {
        WINDOW_WIDTH = cur_w;
        WINDOW_HEIGHT = cur_h;

        int new_cols = (cur_w - UI_WIDTH) / CELL_SIZE;
        int new_rows = (cur_h - GRID_OFFSET_Y - CHART_HEIGHT) / CELL_SIZE;

        if (new_cols > 100) new_cols = 100;
        if (new_rows > 100) new_rows = 100;
        if (new_cols < 10) new_cols = 10; // 最小值保护
        if (new_rows < 10) new_rows = 10;

        GRID_COLS = new_cols;
        GRID_ROWS = new_rows;

        last_w = cur_w;
        last_h = cur_h;

        cleardevice();
    }

    handleInput();

    float update_interval = 0.5f / (float)speed_level;
    if (speed_level > 10) update_interval = 0.01f;

    if (!paused) {
        simulation_timer += delta_time;
        if (simulation_timer >= update_interval) {
            simulation_timer = 0.0f;
            next_Generation();
            generation++;

            population_history[history_cursor] = count_live_cells();
            history_cursor = (history_cursor + 1) % HISTORY_LEN;
        }
    }

    stats_timer += delta_time;
    if (stats_timer >= 1.0f) {
        stats_timer = 0.0f;
    }
}

// ==========================================
//  6. 渲染
// ==========================================
void sys_render() {
    render();
}

// ==========================================
//  7. 清理
// ==========================================
void sys_cleanup() {
    closeRenderer();
    closegraph();
    printf("[System] Shutdown.\n");
}

// ==========================================
//  主入口 (修复：帧率控制逻辑)
// ==========================================
int main() {
    sys_init();

    const float TARGET_FPS = 60.0f;
    const float FRAME_TIME = 1.0f / TARGET_FPS;

    while (true) {
        // 1. 获取本帧开始时的 delta（上一帧到现在的时间）
        float delta = timer_get_delta(&sys_timer);

        // 2. 逻辑更新
        sys_update(delta);

        // 3. 渲染
        sys_render();

        // 4. 帧率控制 (修复：使用 peek 而不是再次 get_delta)
        float elapsed = timer_peek_elapsed(&sys_timer);
        if (elapsed < FRAME_TIME) {
            float sleep_ms = (FRAME_TIME - elapsed) * 1000.0f;
            if (sleep_ms > 1.0f) {
                Sleep((DWORD)sleep_ms);
            }
        }
    }

    sys_cleanup();
    return 0;
}