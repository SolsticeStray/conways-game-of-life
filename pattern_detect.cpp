#define _CRT_SECURE_NO_WARNINGS
#include "pattern_detect.h"
#include "global.h"
#include <string.h>
#include <stdlib.h>

// ==========================================
//  检测结果存储
// ==========================================
static DetectedPattern detected_list[MAX_DETECTED];
static int detected_count = 0;

// ==========================================
//  图案模板定义
// ==========================================

// 方块 Block (2x2)
static const int TEMPLATE_BLOCK[] = {
    1, 1,
    1, 1
};
static const int BLOCK_W = 2, BLOCK_H = 2;

// 蜂巢 Beehive (4x3)
static const int TEMPLATE_BEEHIVE[] = {
    0, 1, 1, 0,
    1, 0, 0, 1,
    0, 1, 1, 0
};
static const int BEEHIVE_W = 4, BEEHIVE_H = 3;

// 面包 Loaf (4x4)
static const int TEMPLATE_LOAF[] = {
    0, 1, 1, 0,
    1, 0, 0, 1,
    0, 1, 0, 1,
    0, 0, 1, 0
};
static const int LOAF_W = 4, LOAF_H = 4;

// 船 Boat (3x3)
static const int TEMPLATE_BOAT[] = {
    1, 1, 0,
    1, 0, 1,
    0, 1, 0
};
static const int BOAT_W = 3, BOAT_H = 3;

// 浴缸 Tub (3x3)
static const int TEMPLATE_TUB[] = {
    0, 1, 0,
    1, 0, 1,
    0, 1, 0
};
static const int TUB_W = 3, TUB_H = 3;

// 信号灯 Blinker - 水平 (3x1)
static const int TEMPLATE_BLINKER_H[] = {
    1, 1, 1
};
static const int BLINKER_H_W = 3, BLINKER_H_H = 1;

// 信号灯 Blinker - 垂直 (1x3)
static const int TEMPLATE_BLINKER_V[] = {
    1,
    1,
    1
};
static const int BLINKER_V_W = 1, BLINKER_V_H = 3;

// 蟾蜍 Toad - 相位1 (4x2)
static const int TEMPLATE_TOAD_1[] = {
    0, 1, 1, 1,
    1, 1, 1, 0
};
static const int TOAD_1_W = 4, TOAD_1_H = 2;

// 滑翔机 Glider - 4个相位
static const int TEMPLATE_GLIDER_1[] = {
    0, 1, 0,
    0, 0, 1,
    1, 1, 1
};

static const int TEMPLATE_GLIDER_2[] = {
    1, 0, 1,
    0, 1, 1,
    0, 1, 0
};

static const int TEMPLATE_GLIDER_3[] = {
    0, 0, 1,
    1, 0, 1,
    0, 1, 1
};

static const int TEMPLATE_GLIDER_4[] = {
    1, 0, 0,
    0, 1, 1,
    1, 1, 0
};

static const int GLIDER_W = 3, GLIDER_H = 3;

// ==========================================
//  辅助函数：模板匹配
// ==========================================
static int match_template(int start_row, int start_col,
    const int* tmpl, int tw, int th) {
    for (int r = 0; r < th; r++) {
        for (int c = 0; c < tw; c++) {
            int gr = start_row + r;
            int gc = start_col + c;

            if (gr < 0 || gr >= GRID_ROWS || gc < 0 || gc >= GRID_COLS) {
                return 0;
            }

            int expected = tmpl[r * tw + c];
            int actual = grid[gr][gc] ? 1 : 0;

            if (expected != actual) {
                return 0;
            }
        }
    }
    return 1;
}

// 检查位置是否已被其他检测覆盖
static int is_position_covered(int row, int col) {
    for (int i = 0; i < detected_count; i++) {
        DetectedPattern* p = &detected_list[i];
        if (row >= p->y && row < p->y + p->height &&
            col >= p->x && col < p->x + p->width) {
            return 1;
        }
    }
    return 0;
}

// 添加检测结果
static void add_detection(PatternType type, int col, int row, int w, int h) {
    if (detected_count >= MAX_DETECTED) return;

    detected_list[detected_count].type = type;
    detected_list[detected_count].x = col;
    detected_list[detected_count].y = row;
    detected_list[detected_count].width = w;
    detected_list[detected_count].height = h;
    detected_count++;
}

// ==========================================
//  初始化
// ==========================================
void init_pattern_detector(void) {
    detected_count = 0;
}

// ==========================================
//  主检测函数
// ==========================================
void detect_all_patterns(void) {
    detected_count = 0;

    // 遍历整个网格
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            // 跳过空白区域
            if (!grid[row][col]) continue;

            // 跳过已检测区域
            if (is_position_covered(row, col)) continue;

            // ===== 检测滑翔机（4个相位）=====
            if (match_template(row, col, TEMPLATE_GLIDER_1, GLIDER_W, GLIDER_H) ||
                match_template(row, col, TEMPLATE_GLIDER_2, GLIDER_W, GLIDER_H) ||
                match_template(row, col, TEMPLATE_GLIDER_3, GLIDER_W, GLIDER_H) ||
                match_template(row, col, TEMPLATE_GLIDER_4, GLIDER_W, GLIDER_H)) {
                add_detection(PATTERN_GLIDER, col, row, GLIDER_W, GLIDER_H);
                continue;
            }

            // ===== 检测方块 =====
            if (match_template(row, col, TEMPLATE_BLOCK, BLOCK_W, BLOCK_H)) {
                add_detection(PATTERN_BLOCK, col, row, BLOCK_W, BLOCK_H);
                continue;
            }

            // ===== 检测蜂巢 =====
            if (match_template(row, col, TEMPLATE_BEEHIVE, BEEHIVE_W, BEEHIVE_H)) {
                add_detection(PATTERN_BEEHIVE, col, row, BEEHIVE_W, BEEHIVE_H);
                continue;
            }

            // ===== 检测面包 =====
            if (match_template(row, col, TEMPLATE_LOAF, LOAF_W, LOAF_H)) {
                add_detection(PATTERN_LOAF, col, row, LOAF_W, LOAF_H);
                continue;
            }

            // ===== 检测船 =====
            if (match_template(row, col, TEMPLATE_BOAT, BOAT_W, BOAT_H)) {
                add_detection(PATTERN_BOAT, col, row, BOAT_W, BOAT_H);
                continue;
            }

            // ===== 检测浴缸 =====
            if (match_template(row, col, TEMPLATE_TUB, TUB_W, TUB_H)) {
                add_detection(PATTERN_TUB, col, row, TUB_W, TUB_H);
                continue;
            }

            // ===== 检测信号灯 =====
            if (match_template(row, col, TEMPLATE_BLINKER_H, BLINKER_H_W, BLINKER_H_H)) {
                add_detection(PATTERN_BLINKER, col, row, BLINKER_H_W, BLINKER_H_H);
                continue;
            }
            if (match_template(row, col, TEMPLATE_BLINKER_V, BLINKER_V_W, BLINKER_V_H)) {
                add_detection(PATTERN_BLINKER, col, row, BLINKER_V_W, BLINKER_V_H);
                continue;
            }

            // ===== 检测蟾蜍 =====
            if (match_template(row, col, TEMPLATE_TOAD_1, TOAD_1_W, TOAD_1_H)) {
                add_detection(PATTERN_TOAD, col, row, TOAD_1_W, TOAD_1_H);
                continue;
            }
        }
    }
}

// ==========================================
//  获取检测结果
// ==========================================
int get_detected_count(void) {
    return detected_count;
}

DetectedPattern* get_detected_patterns(void) {
    return detected_list;
}

const char* get_pattern_name(PatternType type) {
    switch (type) {
    case PATTERN_BLOCK:    return "Block";
    case PATTERN_BEEHIVE:  return "Beehive";
    case PATTERN_LOAF:     return "Loaf";
    case PATTERN_BOAT:     return "Boat";
    case PATTERN_TUB:      return "Tub";
    case PATTERN_BLINKER:  return "Blinker";
    case PATTERN_TOAD:     return "Toad";
    case PATTERN_BEACON:   return "Beacon";
    case PATTERN_GLIDER:   return "Glider";
    case PATTERN_LWSS:     return "LWSS";
    default:               return "Unknown";
    }
}