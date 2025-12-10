// render.h
// 图形渲染模块头文件

#ifndef RENDER_H
#define RENDER_H

#define ROWS 20
#define COLS 30

// 初始化渲染系统
void initRenderer(int rows, int cols);

// 关闭渲染系统
void closeRenderer(void);

// 绘制网格背景
void drawGrid(void);

// 绘制单个细胞
void drawCell(int x, int y, int alive);

// 渲染整个游戏画面
void render(int grid[][COLS], int rows, int cols, int generation, int is_paused);

// 屏幕坐标转网格坐标
int screenToGridX(int screen_x);
int screenToGridY(int screen_y);

// 帧率控制
void limitFPS(int target_fps);

// 获取当前FPS
float getFPS(void);

#endif
