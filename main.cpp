#include "global.h"
#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <stdio.h>
#include <time.h>

 // 引入队友的“契约”
#include "core.h"
#include "render.h"
#include "input.h"

// ============================================
// 全局变量定义 (实体在这里！)
// ============================================
int grid[100][100] = { 0 };
int GRID_ROWS = 60;
int GRID_COLS = 80;
int paused = 1; // 1=暂停, 0=运行
int generation = 0;

// 定义存档文件名
const char* SAVE_FILE = "save.txt";

// ============================================
// 系统功能：存档 (Save Game)
// 格式：简单文本，每一行存一个格子：行 列 状态
// 或者更紧凑：直接把 0/1 矩阵写进去
// ============================================/*

 
// ============================================
// 主程序入口
// ============================================
int main() {
    // 1. 初始化窗口
    initgraph(800, 600);

    // 设置背景色（可选，根据 Render 组的设计）
    setbkcolor(BLACK);
    cleardevice();

    // 开启批量绘图
    BeginBatchDraw();

    // 2. 游戏主循环
    while (true) {

        // --- Input 阶段 ---
        handleInput();

        // 扩展：System 组额外接管了 S 和 L 键的逻辑
        // 因为 handleInput 主要是处理游戏内的交互
        // 存档读档通常是 System 级的指令
        if (GetAsyncKeyState('S') & 0x8000) {
            saveGame();
        }
        if (GetAsyncKeyState('L') & 0x8000) {
            loadGame();
        }

        // --- Core 阶段 ---
        if (paused == 0) {
            next_Generation();
			generation++;
            Sleep(100); // 控制演化速度
        }
        else {
            Sleep(10);  // 暂停时省点 CPU
        }

        // --- Render 阶段 ---
        cleardevice();

        render( grid, GRID_ROWS, GRID_COLS, generation, paused); // 画网格和细胞

        // System 组负责绘制 UI 覆盖层 (Overlay)
        settextcolor(WHITE);
        setbkmode(TRANSPARENT);
        if (paused) {
            outtextxy(10, 10, "PAUSED - [Space] Run | [L] Load | [S] Save | [R] Reset");
        }
        else {
            outtextxy(10, 10, "RUNNING - [Space] Pause");
        }

        FlushBatchDraw();
    }

    EndBatchDraw();
    closegraph();
    return 0;
}