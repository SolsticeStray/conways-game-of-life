#define _CRT_SECURE_NO_WARNINGS
#include "global.h"
#include "input.h"
#include "core.h"
#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>

// ==========================================
//  定义全局变量 (头文件中声明为 extern)
// ==========================================
ToolType current_tool = TOOL_PEN;
int mouse_grid_x = -1;
int mouse_grid_y = -1;

// ==========================================
//  内部静态变量
// ==========================================
#define BUTTON_COUNT 6
static Button ui_buttons[BUTTON_COUNT];
static bool buttons_initialized = false;
static bool is_mouse_down = false;
static int last_grid_x = -1;
static int last_grid_y = -1;

// ==========================================
//  预设图案库
// ==========================================
static const int PATTERN_GLIDER[5][2] = {
    {0, -1}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}
};

static const int PATTERN_LWSS[9][2] = {
    {1, -1}, {2, -1}, {3, -1}, {4, -1},
    {0, 0}, {4, 0},
    {4, 1},
    {0, 2}, {3, 2}
};

static const int PATTERN_PULSAR[5][2] = {
    {0, -1}, {1, -1},
    {-1, 0}, {0, 0},
    {0, 1}
};

static const int PATTERN_GOSPER[9][2] = {
    {0,0}, {1,0}, {0,1}, {1,1},
    {10,0}, {10,1}, {10,2},
    {11,-1}, {11,3}
};

// ... 其余代码保持不变 ...

// ==========================================
//  辅助函数：Bresenham 直线算法
// ==========================================
void draw_line_on_grid(int x0, int y0, int x1, int y1, int state) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {
        if (x0 >= 0 && x0 < GRID_COLS && y0 >= 0 && y0 < GRID_ROWS) {
            grid[y0][x0] = state;
        }

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// ==========================================
//  辅助函数：放置图案
// ==========================================
void apply_pattern(int center_x, int center_y, const int pattern[][2], int count) {
    for (int i = 0; i < count; i++) {
        int nx = center_x + pattern[i][0];
        int ny = center_y + pattern[i][1];

        int final_x = (nx + GRID_COLS) % GRID_COLS;
        int final_y = (ny + GRID_ROWS) % GRID_ROWS;

        grid[final_y][final_x] = 1;
    }
}

// ==========================================
//  初始化：UI 布局 (修复：确保正确初始化)
// ==========================================
void initInput() {
    int start_x = WINDOW_WIDTH - 100;
    int start_y = 60;
    int w = 80;
    int h = 25;
    int gap = 30;

    // 修复：即使位置不理想也要初始化，防止垃圾值
    if (start_x < 10) {
        start_x = 10;
    }

    sprintf_s(ui_buttons[0].text, "PEN");      ui_buttons[0].id = TOOL_PEN;
    sprintf_s(ui_buttons[1].text, "ERASER");   ui_buttons[1].id = TOOL_ERASER;
    sprintf_s(ui_buttons[2].text, "GLIDER");   ui_buttons[2].id = TOOL_STAMP_GLIDER;
    sprintf_s(ui_buttons[3].text, "LWSS");     ui_buttons[3].id = TOOL_STAMP_LWSS;
    sprintf_s(ui_buttons[4].text, "CLEAR");    ui_buttons[4].id = 100;
    sprintf_s(ui_buttons[5].text, "PAUSE");    ui_buttons[5].id = 101;

    for (int i = 0; i < BUTTON_COUNT; i++) {
        ui_buttons[i].x = start_x;
        ui_buttons[i].y = start_y + i * gap;
        ui_buttons[i].w = w;
        ui_buttons[i].h = h;
        ui_buttons[i].is_hovered = false;
        ui_buttons[i].is_clicked = false;
    }

    buttons_initialized = true;
}

Button* getButtonList() {
    if (!buttons_initialized) return NULL;
    return ui_buttons;
}

int getButtonCount() {
    if (!buttons_initialized) return 0;
    return BUTTON_COUNT;
}

// ==========================================
//  核心逻辑：鼠标处理
// ==========================================
void handleMouseLogic() {
    MOUSEMSG msg;
    bool event_processed = false;

    while (MouseHit()) {
        msg = GetMouseMsg();

        mouse_grid_x = (msg.x - GRID_OFFSET_X) / CELL_SIZE;
        mouse_grid_y = (msg.y - GRID_OFFSET_Y) / CELL_SIZE;

        // UI 按钮检测
        if (buttons_initialized) {
            for (int i = 0; i < BUTTON_COUNT; i++) {
                bool hit = (msg.x >= ui_buttons[i].x && msg.x <= ui_buttons[i].x + ui_buttons[i].w &&
                    msg.y >= ui_buttons[i].y && msg.y <= ui_buttons[i].y + ui_buttons[i].h);
                ui_buttons[i].is_hovered = hit;

                if (hit && msg.uMsg == WM_LBUTTONDOWN) {
                    ui_buttons[i].is_clicked = true;
                    if (ui_buttons[i].id < 100) {
                        current_tool = (ToolType)ui_buttons[i].id;
                    }
                    else if (ui_buttons[i].id == 100) {
                        restart_grid();
                    }
                    else if (ui_buttons[i].id == 101) {
                        paused = !paused;
                    }
                    event_processed = true;
                }
                else if (msg.uMsg == WM_LBUTTONUP) {
                    ui_buttons[i].is_clicked = false;
                }
            }
        }

        if (event_processed) continue;

        // 画布交互
        if (mouse_grid_y >= 0 && mouse_grid_y < GRID_ROWS &&
            mouse_grid_x >= 0 && mouse_grid_x < GRID_COLS) {

            if (msg.uMsg == WM_LBUTTONDOWN) {
                is_mouse_down = true;
                last_grid_x = mouse_grid_x;
                last_grid_y = mouse_grid_y;

                switch (current_tool) {
                case TOOL_STAMP_GLIDER:
                    apply_pattern(mouse_grid_x, mouse_grid_y, PATTERN_GLIDER, 5);
                    break;
                case TOOL_STAMP_LWSS:
                    apply_pattern(mouse_grid_x, mouse_grid_y, PATTERN_LWSS, 9);
                    break;
                case TOOL_STAMP_PULSAR:
                    apply_pattern(mouse_grid_x, mouse_grid_y, PATTERN_PULSAR, 5);
                    break;
                case TOOL_PEN:
                    grid[mouse_grid_y][mouse_grid_x] = 1;
                    break;
                case TOOL_ERASER:
                    grid[mouse_grid_y][mouse_grid_x] = 0;
                    break;
                default: break;
                }
            }
            else if (msg.uMsg == WM_MOUSEMOVE && is_mouse_down) {
                if (current_tool == TOOL_PEN || current_tool == TOOL_ERASER) {
                    int target_state = (current_tool == TOOL_PEN) ? 1 : 0;
                    draw_line_on_grid(last_grid_x, last_grid_y, mouse_grid_x, mouse_grid_y, target_state);
                    last_grid_x = mouse_grid_x;
                    last_grid_y = mouse_grid_y;
                }
            }
        }

        if (msg.uMsg == WM_LBUTTONUP) {
            is_mouse_down = false;
            last_grid_x = -1;
            last_grid_y = -1;
        }
    }
}

// ==========================================
//  核心逻辑：键盘处理
// ==========================================
// 在 handleKeyboardLogic() 函数中添加：

void handleKeyboardLogic() {
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        exit(0);
    }

    if (GetAsyncKeyState('1') & 0x8000) current_tool = TOOL_PEN;
    if (GetAsyncKeyState('2') & 0x8000) current_tool = TOOL_ERASER;
    if (GetAsyncKeyState('3') & 0x8000) current_tool = TOOL_STAMP_GLIDER;
    if (GetAsyncKeyState('4') & 0x8000) current_tool = TOOL_STAMP_LWSS;

    if (_kbhit()) {
        char ch = _getch();
        switch (ch) {
        case ' ': paused = !paused; break;
        case 'c': case 'C': restart_grid(); break;
        case 'r': case 'R': randomize_grid(0.2f); break;
        case 'h': case 'H': show_heatmap = !show_heatmap; break;
        case 'm': case 'M':
            heatmap_mode = (heatmap_mode + 1) % 3;  // 新增：切换热力图模式
            break;
        case 'v': case 'V': show_ai_vision = !show_ai_vision; break;
        case 'f': case 'F':
            speed_level = (speed_level >= 10) ? 1 : speed_level + 2;
            break;
        case 's': case 'S': saveGameSlot(0); break;
        case 'l': case 'L': loadGameSlot(0); break;
        case 'u': case 'U': undo_generation(); break;
        }
    }
}

void handleInput() {
    handleMouseLogic();
    handleKeyboardLogic();
}

// ==========================================
//  存档系统
// ==========================================

void saveGameSlot(int slot_id) {
    char filename[64];
    sprintf_s(filename, "save_slot_%d.txt", slot_id);

    FILE* fp = fopen(filename, "w");
    if (fp == NULL) return;

    fprintf(fp, "GOL_SAVE_V2\n");
    fprintf(fp, "%d %d %d\n", GRID_ROWS, GRID_COLS, generation);

    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            fprintf(fp, "%d", grid[i][j] ? 1 : 0);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    HWND hwnd = GetHWnd();
    char title[100];
    sprintf_s(title, "Game Saved to Slot %d!", slot_id);
    SetWindowTextA(hwnd, title);
}

void loadGameSlot(int slot_id) {
    char filename[64];
    sprintf_s(filename, "save_slot_%d.txt", slot_id);

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        if (slot_id == 0) fp = fopen("save.txt", "r");
        if (fp == NULL) return;
    }

    char header[32];
    int r, c, gen;

    long pos = ftell(fp);
    if (fscanf(fp, "%31s", header) != 1) {
        fclose(fp);
        return;
    }

    if (strcmp(header, "GOL_SAVE_V2") == 0) {
        if (fscanf(fp, "%d %d %d", &r, &c, &gen) != 3) {
            fclose(fp);
            return;
        }
        generation = gen;
    }
    else {
        fseek(fp, pos, SEEK_SET);
        if (fscanf(fp, "%d %d", &r, &c) != 2) {
            fclose(fp);
            return;
        }
        generation = 0;
    }

    restart_grid();

    char line_buffer[2048];
    // 吃掉换行符
    if (fgets(line_buffer, sizeof(line_buffer), fp) == NULL) {
        fclose(fp);
        return;
    }

    for (int i = 0; i < r; i++) {
        if (i >= GRID_ROWS) break;

        if (fgets(line_buffer, sizeof(line_buffer), fp)) {
            for (int j = 0; j < c; j++) {
                if (j >= GRID_COLS) break;
                if (j < (int)strlen(line_buffer)) {
                    char ch = line_buffer[j];
                    if (ch == '1') grid[i][j] = 1;
                    else if (ch == '0') grid[i][j] = 0;
                }
            }
        }
    }

    fclose(fp);
    paused = 1;

    HWND hwnd = GetHWnd();
    char title[100];
    sprintf_s(title, "Game Loaded from Slot %d!", slot_id);
    SetWindowTextA(hwnd, title);
}

void saveGame() { saveGameSlot(0); }
void loadGame() { loadGameSlot(0); }