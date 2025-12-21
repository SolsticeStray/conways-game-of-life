// render.h
#ifndef RENDER_H
#define RENDER_H

// 1. 修改网格大小为 100x100
#define ROWS 100
#define COLS 100

// 初始化渲染窗口
void initRenderer(int rows, int cols);

// 关闭渲染窗口
void closeRenderer(void);

// 渲染主函数：绘制网格、细胞和侧边栏 UI
void render();

// --- 新增控制功能 ---

// 随机初始化网格 (C语言实现)
void randomizeGrid(int grid[ROWS][COLS]);

// 屏幕点击坐标转网格坐标 (返回 -1 表示未点中)
int getGridX(int screen_x);
int getGridY(int screen_y);

// 简单的延时函数用于控制速度 (替代复杂的FPS计算，适合C语言初学)
void waitDelay(int speed_level);

#endif