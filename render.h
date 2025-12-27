#ifndef RENDER_H
#define RENDER_H

#include <graphics.h>

// ==========================================
//  相机结构体
// ==========================================
typedef struct {
    int offset_x;
    int offset_y;
    float zoom;
} Camera;

// ==========================================
//  粒子结构体
// ==========================================
typedef struct {
    float x, y;
    float vx, vy;
    int life;
    bool active;
    COLORREF color;
} Particle;

// ==========================================
//  外部变量声明 (定义在 render.cpp 中)
// ==========================================
extern Camera main_camera;

// ==========================================
//  渲染函数声明
// ==========================================
void initRenderer(void);
void closeRenderer(void);
void render(void);

void screenToGrid(int sx, int sy, int* gx, int* gy);
void spawnDeathParticles(int grid_x, int grid_y);

#endif // RENDER_H