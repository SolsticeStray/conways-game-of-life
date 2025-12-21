#include <graphics.h>
#include <stdio.h>
#include <math.h>
#include <tchar.h> // 【关键】引入通用字符头文件
#include "global.h"

// ---------------------------------------------------------
// 辅助配色
// ---------------------------------------------------------
COLORREF get_thermal_color(int neighbors) {
    if (neighbors <= 1) return RGB(50, 50, 255);      // 蓝
    if (neighbors == 2) return RGB(0, 200, 0);        // 绿
    if (neighbors == 3) return RGB(50, 255, 50);      // 亮绿
    if (neighbors >= 4) return RGB(255, 50, 0);       // 红
    return WHITE;
}

// ---------------------------------------------------------
// 核心 UI 绘制 (TCHAR 通用版)
// ---------------------------------------------------------
void draw_hud() {
    int w = getwidth();

    // 1. 背景条 (强制覆盖)
    setbkmode(OPAQUE);
    setfillcolor(RGB(30, 30, 35));
    setlinecolor(RGB(30, 30, 35));
    solidrectangle(0, 0, w, 50);

    // 2. 字体设置 (通用)
    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 255, 255));
    // 使用 Consolas 字体，高度 16
    settextstyle(16, 0, _T("Consolas"));

    // 3. 准备文字 (使用 TCHAR 数组)
    TCHAR left_msg[256];

    // 【关键】使用 _stprintf_s 代替 sprintf_s
    // 它可以自动识别是宽字符还是窄字符
    _stprintf_s(left_msg, _T("GEN: %-5d | SPD: x%-2d | [H]EAT: %s | [V]ISION: %s"),
        generation,
        speed_level,
        show_heatmap ? _T("ON") : _T("OFF"),
        show_ai_vision ? _T("ON") : _T("OFF")
    );

    outtextxy(10, 15, left_msg);

    // 4. 右侧趋势分析 (逻辑不变，只改字符处理)
    int current_idx = history_cursor - 1;
    if (current_idx < 0) current_idx = HISTORY_LEN - 1;

    int past_idx = current_idx - 10;
    if (past_idx < 0) past_idx += HISTORY_LEN;

    int pop_now = population_history[current_idx];
    int pop_old = population_history[past_idx];
    int delta = pop_now - pop_old;

    TCHAR right_msg[256];
    COLORREF trend_color = WHITE;

    if (pop_now == 0) {
        _stprintf_s(right_msg, _T("STATUS: DEAD"));
        trend_color = DARKGRAY;
    }
    else if (abs(delta) <= 2) {
        _stprintf_s(right_msg, _T("STATUS: STABLE"));
        trend_color = CYAN;
    }
    else if (delta > 0) {
        _stprintf_s(right_msg, _T("GROW: +%d/10gen"), delta);
        trend_color = GREEN;
    }
    else {
        _stprintf_s(right_msg, _T("DIE: %d/10gen"), delta);
        trend_color = RED;
    }

    settextcolor(trend_color);

    // 简单的右对齐计算
    int text_width = 250;
    // 防止覆盖左边的文字
    int safe_x = w - text_width;
    if (safe_x < 400) safe_x = 400;

    outtextxy(safe_x, 15, right_msg);

    // 5. 分隔线
    setlinecolor(RGB(100, 100, 100));
    line(0, 50, w, 50);
}

// ---------------------------------------------------------
// 主渲染
// ---------------------------------------------------------
void render() {
    setbkmode(OPAQUE);
    setbkcolor(BLACK);

    // Grid Loop
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            if (grid[i][j]) {
                int x1 = j * CELL_SIZE + GRID_OFFSET_X;
                int y1 = i * CELL_SIZE + GRID_OFFSET_Y;
                int x2 = x1 + CELL_SIZE;
                int y2 = y1 + CELL_SIZE;

                if (y1 < 50) continue; // UI Protection

                if (show_heatmap) {
                    int n = 0;
                    for (int dx = -1; dx <= 1; dx++)
                        for (int dy = -1; dy <= 1; dy++)
                            if ((dx != 0 || dy != 0) && i + dx >= 0 && i + dx < GRID_ROWS && j + dy >= 0 && j + dy < GRID_COLS)
                                n += grid[i + dx][j + dy];
                    setfillcolor(get_thermal_color(n));
                    setlinecolor(RGB(30, 30, 30));
                }
                else {
                    setfillcolor(RGB(220, 220, 220));
                    setlinecolor(RGB(40, 40, 40));
                }
                fillrectangle(x1, y1, x2, y2);
            }
        }
    }

    // AI Vision
    if (show_ai_vision) {
        setlinecolor(CYAN);
        setlinestyle(PS_SOLID, 2);
        setbkmode(TRANSPARENT);
        settextcolor(CYAN);
        settextstyle(12, 0, _T("System")); // 小标签字体

        int visited[100][100] = { 0 };

        for (int i = 0; i < GRID_ROWS - 3; i++) {
            for (int j = 0; j < GRID_COLS - 3; j++) {
                if (visited[i][j] || !grid[i][j]) continue;
                int x = j * CELL_SIZE + GRID_OFFSET_X;
                int y = i * CELL_SIZE + GRID_OFFSET_Y;
                if (y < 45) continue;

                // Block
                if (grid[i][j + 1] && grid[i + 1][j] && grid[i + 1][j + 1] &&
                    !visited[i][j + 1] && !visited[i + 1][j] && !visited[i + 1][j + 1]) {
                    visited[i][j] = visited[i][j + 1] = visited[i + 1][j] = visited[i + 1][j + 1] = 1;
                    rectangle(x - 2, y - 2, x + 2 * CELL_SIZE + 2, y + 2 * CELL_SIZE + 2);
                    outtextxy(x, y - 15, _T("BLK"));
                }
                // Blinker
                else if (grid[i + 1][j] && grid[i + 2][j] && !visited[i + 1][j] && !visited[i + 2][j]) {
                    if ((j == 0 || !grid[i + 1][j - 1]) && (j == GRID_COLS - 1 || !grid[i + 1][j + 1])) {
                        visited[i][j] = visited[i + 1][j] = visited[i + 2][j] = 1;
                        rectangle(x - 2, y - 2, x + CELL_SIZE + 2, y + 3 * CELL_SIZE + 2);
                        outtextxy(x, y - 15, _T("OSC"));
                    }
                }
                // Glider
                else if (grid[i][j + 1] && grid[i + 1][j + 2] && grid[i + 2][j] && grid[i + 2][j + 1] && grid[i + 2][j + 2]) {
                    setlinecolor(MAGENTA);
                    rectangle(x - 2, y - 2, x + 3 * CELL_SIZE + 2, y + 3 * CELL_SIZE + 2);
                    outtextxy(x, y - 15, _T("GLD"));
                    setlinecolor(CYAN);
                }
            }
        }
        setlinestyle(PS_SOLID, 1);
    }

    // UI 最后绘制
    draw_hud();
}