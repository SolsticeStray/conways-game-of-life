#define _CRT_SECURE_NO_WARNINGS
#include "render.h"
#include "global.h"
#include "input.h"
#include "core.h"
#include <stdio.h>
#include <math.h>
#include "pattern_detect.h"
// ==========================================
//  定义全局变量
// ==========================================
Camera main_camera = { 0, 0, 1.0f };

// ==========================================
//  内部静态变量
// ==========================================
#define MAX_PARTICLES 1000
static Particle particle_pool[MAX_PARTICLES];

// 配色方案
static const COLORREF COLOR_BG = RGB(10, 10, 15);
static const COLORREF COLOR_GRID = RGB(25, 25, 30);
static const COLORREF COLOR_CELL_ALIVE = RGB(0, 255, 200);
static const COLORREF COLOR_CELL_DYING = RGB(255, 50, 50);
static const COLORREF COLOR_UI_BG = RGB(40, 40, 45);
static const COLORREF COLOR_UI_BORDER = RGB(100, 100, 100);
static const COLORREF COLOR_UI_TEXT = RGB(200, 200, 200);
static const COLORREF COLOR_UI_ACTIVE = RGB(255, 200, 0);

// ==========================================
//  图案预览数据
// ==========================================
static const int P_GLIDER[] = { 0, -1, 1, 0, -1, 1, 0, 1, 1, 1 };
static const int P_LWSS[] = {
    1, -1, 2, -1, 3, -1, 4, -1,
    0, 0, 4, 0,
    4, 1,
    0, 2, 3, 2
};
static const int P_PULSAR[] = { 0, -1, 1, -1, -1, 0, 0, 0, 0, 1 };

const int* get_preview_pattern(int tool_id, int* count) {
    switch (tool_id) {
    case TOOL_STAMP_GLIDER: *count = 5; return P_GLIDER;
    case TOOL_STAMP_LWSS:   *count = 9; return P_LWSS;
    case TOOL_STAMP_PULSAR: *count = 5; return P_PULSAR;
    default: *count = 0; return NULL;
    }
}

// ==========================================
//  新增：热力图颜色计算
// ==========================================

// 将值映射到蓝->青->绿->黄->红的渐变色
COLORREF value_to_heatcolor(int value, int max_value) {
    if (max_value <= 0) max_value = 1;

    float ratio = (float)value / (float)max_value;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;

    int r, g, b;

    if (ratio < 0.25f) {
        // 蓝 -> 青
        float t = ratio / 0.25f;
        r = 0;
        g = (int)(255 * t);
        b = 255;
    }
    else if (ratio < 0.5f) {
        // 青 -> 绿
        float t = (ratio - 0.25f) / 0.25f;
        r = 0;
        g = 255;
        b = (int)(255 * (1 - t));
    }
    else if (ratio < 0.75f) {
        // 绿 -> 黄
        float t = (ratio - 0.5f) / 0.25f;
        r = (int)(255 * t);
        g = 255;
        b = 0;
    }
    else {
        // 黄 -> 红
        float t = (ratio - 0.75f) / 0.25f;
        r = 255;
        g = (int)(255 * (1 - t));
        b = 0;
    }

    return RGB(r, g, b);
}

// 邻居密度颜色（0-8映射到蓝紫色系）
COLORREF density_to_color(int density) {
    // density: 0-8
    int intensity = density * 28; // 0-224
    if (intensity > 255) intensity = 255;

    return RGB(intensity, 50, 255 - intensity / 2);
}

// ==========================================
//  初始化
// ==========================================
void initRenderer(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_pool[i].active = false;
    }

    settextstyle(16, 0, _T("Consolas"));
    setbkmode(TRANSPARENT);
}

void closeRenderer(void) {
    // 释放资源
}

// ==========================================
//  粒子系统逻辑
// ==========================================
void spawnDeathParticles(int grid_x, int grid_y) {
    int cx = grid_x * CELL_SIZE + GRID_OFFSET_X + CELL_SIZE / 2;
    int cy = grid_y * CELL_SIZE + GRID_OFFSET_Y + CELL_SIZE / 2;

    int spawn_count = 5;
    int spawned = 0;

    for (int i = 0; i < MAX_PARTICLES && spawned < spawn_count; i++) {
        if (!particle_pool[i].active) {
            particle_pool[i].active = true;
            particle_pool[i].x = (float)cx;
            particle_pool[i].y = (float)cy;
            particle_pool[i].vx = (rand() % 10 - 5) * 0.5f;
            particle_pool[i].vy = (rand() % 10 - 5) * 0.5f;
            particle_pool[i].life = 20 + rand() % 10;
            particle_pool[i].color = COLOR_CELL_DYING;
            spawned++;
        }
    }
}

void updateAndDrawParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particle_pool[i].active) {
            particle_pool[i].x += particle_pool[i].vx;
            particle_pool[i].y += particle_pool[i].vy;
            particle_pool[i].life--;

            if (particle_pool[i].life <= 0) {
                particle_pool[i].active = false;
            }
            else {
                int radius = (particle_pool[i].life > 10) ? 2 : 1;
                setfillcolor(particle_pool[i].color);
                solidcircle((int)particle_pool[i].x, (int)particle_pool[i].y, radius);
            }
        }
    }
}
// ==========================================
//  新增：AI 视觉层绘制
// ==========================================

// 图案类型对应的颜色
static COLORREF get_pattern_color(PatternType type) {
    switch (type) {
    case PATTERN_BLOCK:
    case PATTERN_BEEHIVE:
    case PATTERN_LOAF:
    case PATTERN_BOAT:
    case PATTERN_TUB:
        return RGB(100, 200, 255);  // 静物：蓝色

    case PATTERN_BLINKER:
    case PATTERN_TOAD:
    case PATTERN_BEACON:
        return RGB(255, 200, 100);  // 振荡器：橙色

    case PATTERN_GLIDER:
    case PATTERN_LWSS:
        return RGB(255, 100, 255);  // 飞船：紫色

    default:
        return RGB(150, 150, 150);
    }
}

void drawAIVisionLayer() {
    if (!show_ai_vision) return;

    // 执行图案检测
    detect_all_patterns();

    int count = get_detected_count();
    DetectedPattern* patterns = get_detected_patterns();

    // 绘制检测框和标签
    for (int i = 0; i < count; i++) {
        DetectedPattern p = patterns[i];

        int px = p.x * CELL_SIZE + GRID_OFFSET_X;
        int py = p.y * CELL_SIZE + GRID_OFFSET_Y;
        int pw = p.width * CELL_SIZE;
        int ph = p.height * CELL_SIZE;

        COLORREF box_color = get_pattern_color(p.type);

        // 绘制边界框
        setlinecolor(box_color);
        setlinestyle(PS_SOLID, 2);
        rectangle(px - 2, py - 2, px + pw + 2, py + ph + 2);
        setlinestyle(PS_SOLID, 1);

        // 绘制标签背景
        const char* name = get_pattern_name(p.type);
        int text_w = (int)strlen(name) * 8;
        int text_h = 14;
        int label_x = px - 2;
        int label_y = py - text_h - 4;

        if (label_y < GRID_OFFSET_Y) {
            label_y = py + ph + 4;
        }

        setfillcolor(RGB(0, 0, 0));
        solidrectangle(label_x, label_y, label_x + text_w + 4, label_y + text_h);

        // 绘制标签文字
        settextcolor(box_color);
        outtextxy(label_x + 2, label_y + 1, name);
    }

    // 绘制统计信息
    char stats[128];

    // 统计各类图案数量
    int still_count = 0, osc_count = 0, ship_count = 0;
    for (int i = 0; i < count; i++) {
        PatternType t = patterns[i].type;
        if (t >= PATTERN_BLOCK && t <= PATTERN_TUB) still_count++;
        else if (t >= PATTERN_BLINKER && t <= PATTERN_BEACON) osc_count++;
        else if (t >= PATTERN_GLIDER && t <= PATTERN_LWSS) ship_count++;
    }

    sprintf_s(stats, "AI VISION | Still: %d | Oscillators: %d | Spaceships: %d",
        still_count, osc_count, ship_count);

    // 绘制统计栏背景
    int bar_y = GRID_OFFSET_Y - 20;
    setfillcolor(RGB(30, 30, 50));
    solidrectangle(0, bar_y, WINDOW_WIDTH, bar_y + 18);

    settextcolor(RGB(200, 255, 200));
    outtextxy(10, bar_y + 2, stats);
}
// ==========================================
//  子模块：绘制网格与细胞（支持热力图）
// ==========================================
void drawGridLayer() {
    setbkcolor(COLOR_BG);
    cleardevice();

    setlinecolor(COLOR_GRID);
    for (int i = 0; i <= GRID_ROWS; i++) {
        int y = i * CELL_SIZE + GRID_OFFSET_Y;
        line(0, y, WINDOW_WIDTH, y);
    }
    for (int j = 0; j <= GRID_COLS; j++) {
        int x = j * CELL_SIZE + GRID_OFFSET_X;
        line(x, 0, x, WINDOW_HEIGHT);
    }

    int inset = 1;

    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            int x = col * CELL_SIZE + GRID_OFFSET_X;
            int y = row * CELL_SIZE + GRID_OFFSET_Y;

            if (show_heatmap) {
                // ========== 热力图模式 ==========
                COLORREF cell_color;
                int draw_cell = 0;

                switch (heatmap_mode) {
                case 0: // 存活时间热力图
                    if (heatmap[row][col] > 0) {
                        cell_color = value_to_heatcolor(heatmap[row][col], 255);
                        draw_cell = 1;
                    }
                    break;

                case 1: // 邻居密度热力图
                    if (neighbor_density[row][col] > 0 || grid[row][col]) {
                        cell_color = density_to_color(neighbor_density[row][col]);
                        draw_cell = 1;
                    }
                    break;

                case 2: // 混合模式：活细胞+热力图背景
                    if (heatmap[row][col] > 5) {
                        // 绘制半透明热力背景
                        int alpha_heat = heatmap[row][col] / 3;
                        if (alpha_heat > 80) alpha_heat = 80;
                        cell_color = RGB(alpha_heat, alpha_heat / 2, 0);
                        setfillcolor(cell_color);
                        solidrectangle(x + inset, y + inset,
                            x + CELL_SIZE - inset, y + CELL_SIZE - inset);
                    }
                    if (grid[row][col]) {
                        cell_color = COLOR_CELL_ALIVE;
                        draw_cell = 1;
                    }
                    break;

                default:
                    if (grid[row][col]) {
                        cell_color = COLOR_CELL_ALIVE;
                        draw_cell = 1;
                    }
                    break;
                }

                if (draw_cell && heatmap_mode != 2) {
                    setfillcolor(cell_color);
                    setlinecolor(cell_color);
                    fillrectangle(x + inset, y + inset,
                        x + CELL_SIZE - inset, y + CELL_SIZE - inset);
                }
                else if (draw_cell && heatmap_mode == 2) {
                    // 混合模式下，活细胞用亮色覆盖
                    setfillcolor(cell_color);
                    setlinecolor(cell_color);
                    fillrectangle(x + inset + 1, y + inset + 1,
                        x + CELL_SIZE - inset - 1, y + CELL_SIZE - inset - 1);
                }
            }
            else {
                // ========== 普通模式 ==========
                if (grid[row][col]) {
                    setfillcolor(COLOR_CELL_ALIVE);
                    setlinecolor(COLOR_CELL_ALIVE);
                    fillrectangle(x + inset, y + inset,
                        x + CELL_SIZE - inset, y + CELL_SIZE - inset);
                }
            }
        }
    }
}

// ==========================================
//  子模块：绘制预览印章
// ==========================================
void drawPreviewLayer() {
    if (mouse_grid_x >= 0 && mouse_grid_x < GRID_COLS &&
        mouse_grid_y >= 0 && mouse_grid_y < GRID_ROWS) {

        int count = 0;
        const int* pattern = get_preview_pattern(current_tool, &count);

        if (pattern && count > 0) {
            setlinecolor(RGB(255, 255, 255));
            setlinestyle(PS_DOT);

            for (int i = 0; i < count; i++) {
                int dx = pattern[i * 2];
                int dy = pattern[i * 2 + 1];

                int tx = (mouse_grid_x + dx + GRID_COLS) % GRID_COLS;
                int ty = (mouse_grid_y + dy + GRID_ROWS) % GRID_ROWS;

                int px = tx * CELL_SIZE + GRID_OFFSET_X;
                int py = ty * CELL_SIZE + GRID_OFFSET_Y;

                rectangle(px, py, px + CELL_SIZE, py + CELL_SIZE);
            }
            setlinestyle(PS_SOLID);
        }
        else if (current_tool == TOOL_PEN || current_tool == TOOL_ERASER) {
            int px = mouse_grid_x * CELL_SIZE + GRID_OFFSET_X;
            int py = mouse_grid_y * CELL_SIZE + GRID_OFFSET_Y;
            setlinecolor(current_tool == TOOL_PEN ? RGB(0, 255, 0) : RGB(255, 0, 0));
            rectangle(px, py, px + CELL_SIZE, py + CELL_SIZE);
        }
    }
}

// ==========================================
//  子模块：绘制 UI 按钮
// ==========================================
void drawUILayer() {
    Button* buttons = getButtonList();
    int count = getButtonCount();

    if (buttons == NULL || count <= 0) return;

    for (int i = 0; i < count; i++) {
        Button b = buttons[i];

        COLORREF bg_color = COLOR_UI_BG;
        COLORREF text_color = COLOR_UI_TEXT;

        if (b.id < 100 && (int)current_tool == b.id) {
            bg_color = COLOR_UI_ACTIVE;
            text_color = RGB(0, 0, 0);
        }
        else if (b.is_hovered) {
            bg_color = RGB(60, 60, 70);
        }

        setfillcolor(bg_color);
        setlinecolor(COLOR_UI_BORDER);
        fillrectangle(b.x, b.y, b.x + b.w, b.y + b.h);

        int tx = b.x + 5;
        int ty = b.y + 5;

        settextcolor(text_color);
        outtextxy(tx, ty, b.text);
    }

    // 状态栏
    settextcolor(RGB(255, 255, 255));
    char status_msg[128];
    sprintf_s(status_msg, "GEN: %d | SPEED: x%d | PAUSED: %s",
        generation, speed_level, paused ? "YES" : "NO");
    outtextxy(10, 10, status_msg);

    // 热力图模式提示
    if (show_heatmap) {
        const char* mode_names[] = { "LIFETIME", "DENSITY", "MIXED" };
        char heatmap_msg[64];
        sprintf_s(heatmap_msg, "HEATMAP: %s (press M to switch)",
            mode_names[heatmap_mode % 3]);
        settextcolor(RGB(255, 200, 0));
        outtextxy(200, 10, heatmap_msg);
    }

    settextcolor(RGB(200, 200, 200));
    outtextxy(10, 30, "L-Click: Draw | H: Heatmap | M: Mode | S: Save | L: Load");
}

// ==========================================
//  子模块：绘制数据图表
// ==========================================
void drawChartLayer() {
    int chart_y = WINDOW_HEIGHT - CHART_HEIGHT;
    int chart_w = WINDOW_WIDTH;

    setfillcolor(RGB(20, 20, 25));
    solidrectangle(0, chart_y, chart_w, WINDOW_HEIGHT);

    setlinecolor(RGB(50, 50, 50));
    line(0, chart_y, chart_w, chart_y);

    if (HISTORY_LEN < 2) return;

    int max_pop = 1;
    for (int i = 0; i < HISTORY_LEN; i++) {
        if (population_history[i] > max_pop) max_pop = population_history[i];
    }
    max_pop = (int)(max_pop * 1.2f);
    if (max_pop < 1) max_pop = 1;

    int step_w = chart_w / (HISTORY_LEN - 1);

    setlinecolor(RGB(0, 255, 200));

    for (int i = 0; i < HISTORY_LEN - 1; i++) {
        int idx1 = (history_cursor + i) % HISTORY_LEN;
        int idx2 = (history_cursor + i + 1) % HISTORY_LEN;

        int val1 = population_history[idx1];
        int val2 = population_history[idx2];

        int x1 = i * step_w;
        int x2 = (i + 1) * step_w;

        int y1 = WINDOW_HEIGHT - (val1 * CHART_HEIGHT / max_pop);
        int y2 = WINDOW_HEIGHT - (val2 * CHART_HEIGHT / max_pop);

        if (y1 > WINDOW_HEIGHT) y1 = WINDOW_HEIGHT;
        if (y1 < chart_y) y1 = chart_y;
        if (y2 > WINDOW_HEIGHT) y2 = WINDOW_HEIGHT;
        if (y2 < chart_y) y2 = chart_y;

        line(x1, y1, x2, y2);
    }

    char pop_str[32];
    int last_idx = (history_cursor - 1 + HISTORY_LEN) % HISTORY_LEN;
    sprintf_s(pop_str, "POP: %d", population_history[last_idx]);
    settextcolor(RGB(0, 255, 200));
    outtextxy(chart_w - 100, chart_y + 10, pop_str);
}

// ==========================================
//  主渲染循环
// ==========================================
void render() {
    BeginBatchDraw();

    drawGridLayer();
    updateAndDrawParticles();
    drawAIVisionLayer();    // 新增：AI 视觉层
    drawPreviewLayer();
    drawChartLayer();
    drawUILayer();

    EndBatchDraw();
}

void screenToGrid(int sx, int sy, int* gx, int* gy) {
    *gx = (sx - GRID_OFFSET_X) / CELL_SIZE;
    *gy = (sy - GRID_OFFSET_Y) / CELL_SIZE;
}