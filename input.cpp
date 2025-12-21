#include "global.h"
#define _CRT_SECURE_NO_WARNINGS // 防止 fopen 报错

#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "core.h"
#include "render.h"

// 存档功能
void saveGame(void)
{
    FILE* fp = fopen("save.txt", "w");
    if (fp == NULL) return; // 简单失败处理

    // 1. 先写入当前的尺寸配置
    fprintf(fp, "%d %d\n", GRID_ROWS, GRID_COLS);

    // 2. 写入矩阵
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            fprintf(fp, "%d ", grid[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

// 读档功能
void loadGame(void)
{
    FILE* fp = fopen("save.txt", "r");
    if (fp == NULL) return;

    int r, c;
    // 1. 读取保存的尺寸
    fscanf(fp, "%d %d", &r, &c);

    // 2. 安全措施：先清空当前地图，防止残影
    restart_grid(); // 调用 core 里的清空函数

    // 3. 读取数据
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            int val;
            fscanf(fp, "%d", &val);
            // 只有当坐标在当前 grid 范围内时才赋值
            if (i < GRID_ROWS && j < GRID_COLS) {
                grid[i][j] = val;
            }
        }
    }
    fclose(fp);
    paused = 1; // 读档后暂停方便查看
}

// 键盘处理
void handleKeyboard(void)
{
    // 功能键检测
    if (GetAsyncKeyState('H') & 0x8000) { show_heatmap = !show_heatmap; Sleep(200); }
    if (GetAsyncKeyState('V') & 0x8000) { show_ai_vision = !show_ai_vision; Sleep(200); }
    if (GetAsyncKeyState('F') & 0x8000) {
        if (speed_level == 1) speed_level = 2;
        else if (speed_level == 2) speed_level = 5;
        else speed_level = 1;
        Sleep(200);
    }

    if (!_kbhit()) return;

    char ch = _getch();
    switch (ch) {
    case ' ': paused = !paused; break;
    case 'r': case 'R': randomize_grid(); break;
    case 'c': case 'C': restart_grid(); break;
    case 's': case 'S': saveGame(); break;
    case 'l': case 'L': loadGame(); break;
    case 27: EndBatchDraw(); closegraph(); exit(0); break;
    }
}

// 鼠标处理 (关键修正：偏移量 offset)
void handleMouse(void)
{
    while (MouseHit()) {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            // 【核心修正】：减去偏移量！
            // 否则你看着点的是格子，实际上点歪了
            int col = (msg.x - GRID_OFFSET_X) / CELL_SIZE;
            int row = (msg.y - GRID_OFFSET_Y) / CELL_SIZE;

            if (row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS) {
                grid[row][col] = !grid[row][col];
            }
        }
    }
}

void handleInput(void)
{
    handleKeyboard();
    handleMouse();
}