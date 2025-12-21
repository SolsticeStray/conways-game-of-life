#ifndef GLOBAL_H
#define GLOBAL_H

// --- 基础数据 ---
extern int grid[100][100];
extern int paused;
extern int generation;

// --- 配置参数 ---
extern int GRID_ROWS;
extern int GRID_COLS;
extern int CELL_SIZE;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

// --- 界面偏移 ---
extern int GRID_OFFSET_X;
extern int GRID_OFFSET_Y;

// --- 功能开关 ---
extern int show_heatmap;
extern int show_ai_vision;
extern int speed_level;

// --- 新增：全屏开关 ---
extern int fullscreen_mode; // 0=窗口, 1=全屏

// --- 新增：数据分析用的历史记录 ---
// 我们记录过去 50 代的人口数量，用来算导数/趋势
#define HISTORY_LEN 50
extern int population_history[HISTORY_LEN];
extern int history_cursor; // 当前记录到哪了

#endif