#define _CRT_SECURE_NO_WARNINGS
#include "global.h"
#include "core.h"
#include "render.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// ==========================================
//  内部结构与静态变量
// ==========================================
typedef struct {
    char name[32];
    int birth[9];
    int survive[9];
} LifeRule;

static LifeRule current_rule;
static RuleType current_rule_type = RULE_CONWAY;

#define MAX_HISTORY 128
static int history_buffer[MAX_HISTORY][100][100];
static int history_head = 0;
static int history_size = 0;
static int boundary_mode = 0;

// 热力图衰减因子
#define HEATMAP_MAX 255
#define HEATMAP_DECAY 2

// ==========================================
//  规则预设
// ==========================================
void apply_rule_preset(RuleType type) {
    current_rule_type = type;
    memset(current_rule.birth, 0, sizeof(current_rule.birth));
    memset(current_rule.survive, 0, sizeof(current_rule.survive));

    switch (type) {
    case RULE_CONWAY:
        strcpy(current_rule.name, "Conway (B3/S23)");
        current_rule.birth[3] = 1;
        current_rule.survive[2] = 1;
        current_rule.survive[3] = 1;
        break;
    case RULE_HIGHLIFE:
        strcpy(current_rule.name, "HighLife (B36/S23)");
        current_rule.birth[3] = 1;
        current_rule.birth[6] = 1;
        current_rule.survive[2] = 1;
        current_rule.survive[3] = 1;
        break;
    case RULE_DAY_NIGHT:
        strcpy(current_rule.name, "Day & Night");
        current_rule.birth[3] = 1; current_rule.birth[6] = 1;
        current_rule.birth[7] = 1; current_rule.birth[8] = 1;
        current_rule.survive[3] = 1; current_rule.survive[4] = 1;
        current_rule.survive[6] = 1; current_rule.survive[7] = 1;
        current_rule.survive[8] = 1;
        break;
    case RULE_SEEDS:
        strcpy(current_rule.name, "Seeds (B2/S)");
        current_rule.birth[2] = 1;
        break;
    default:
        strcpy(current_rule.name, "Unknown");
        break;
    }
}

// ==========================================
//  初始化
// ==========================================
void core_init(void) {
    srand((unsigned int)time(NULL));
    apply_rule_preset(RULE_CONWAY);
    restart_grid();
}

// ==========================================
//  历史系统实现
// ==========================================
void save_snapshot() {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            history_buffer[history_head][row][col] = grid[row][col];
        }
    }

    history_head = (history_head + 1) % MAX_HISTORY;
    if (history_size < MAX_HISTORY) {
        history_size++;
    }
}

int undo_generation() {
    if (history_size == 0) return 0;

    history_head = (history_head - 1 + MAX_HISTORY) % MAX_HISTORY;
    history_size--;

    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            grid[row][col] = history_buffer[history_head][row][col];
        }
    }

    if (generation > 0) generation--;

    return 1;
}

int get_history_count() {
    return history_size;
}

// ==========================================
//  核心演化逻辑
// ==========================================
int get_neighbors(int row, int col) {
    int count = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;

            int nr = row + dr;
            int nc = col + dc;

            if (boundary_mode == 1) {
                nr = (nr + GRID_ROWS) % GRID_ROWS;
                nc = (nc + GRID_COLS) % GRID_COLS;
                if (grid[nr][nc]) count++;
            }
            else {
                if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS) {
                    if (grid[nr][nc]) count++;
                }
            }
        }
    }
    return count;
}

// 新增：更新热力图数据
void update_heatmap() {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            // 更新邻居密度图
            neighbor_density[row][col] = get_neighbors(row, col);

            // 更新存活时间热力图
            if (grid[row][col]) {
                // 细胞存活，增加热度
                if (heatmap[row][col] < HEATMAP_MAX) {
                    heatmap[row][col] += 10;
                    if (heatmap[row][col] > HEATMAP_MAX) {
                        heatmap[row][col] = HEATMAP_MAX;
                    }
                }
            }
            else {
                // 细胞死亡，热度衰减
                if (heatmap[row][col] > 0) {
                    heatmap[row][col] -= HEATMAP_DECAY;
                    if (heatmap[row][col] < 0) {
                        heatmap[row][col] = 0;
                    }
                }
            }
        }
    }
}

void next_Generation() {
    save_snapshot();

    static int next_grid[100][100];

    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            int n = get_neighbors(row, col);
            int current_state = grid[row][col];
            int next_state = 0;

            if (current_state == 1) {
                if (current_rule.survive[n]) next_state = 1;
            }
            else {
                if (current_rule.birth[n]) next_state = 1;
            }

            next_grid[row][col] = next_state;

            if (current_state == 1 && next_state == 0) {
                if (rand() % 2 == 0) {
                    spawnDeathParticles(col, row);
                }
            }
        }
    }

    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            grid[row][col] = next_grid[row][col];
        }
    }

    // 新增：更新热力图
    update_heatmap();
}

// ==========================================
//  RLE 解析器
// ==========================================
void load_rle_pattern(int start_x, int start_y, const char* rle) {
    int x = 0;
    int y = 0;
    int count = 0;
    int len = (int)strlen(rle);

    for (int i = 0; i < len; i++) {
        char ch = rle[i];

        if (isdigit(ch)) {
            count = count * 10 + (ch - '0');
        }
        else {
            if (count == 0) count = 1;

            if (ch == 'b') {
                x += count;
            }
            else if (ch == 'o') {
                for (int k = 0; k < count; k++) {
                    int draw_x = start_x + x + k;
                    int draw_y = start_y + y;
                    if (draw_y >= 0 && draw_y < GRID_ROWS && draw_x >= 0 && draw_x < GRID_COLS) {
                        grid[draw_y][draw_x] = 1;
                    }
                }
                x += count;
            }
            else if (ch == '$') {
                y += count;
                x = 0;
            }
            else if (ch == '!') {
                break;
            }

            count = 0;
        }
    }
}

// ==========================================
//  辅助功能实现
// ==========================================
void restart_grid() {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            grid[row][col] = 0;
            heatmap[row][col] = 0;           // 新增
            neighbor_density[row][col] = 0;   // 新增
        }
    }

    generation = 0;
    history_head = 0;
    history_size = 0;
}

void randomize_grid(float density) {
    restart_grid();
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            if ((rand() % 100) < (int)(density * 100)) {
                grid[row][col] = 1;
            }
        }
    }
}

int count_live_cells() {
    int sum = 0;
    for (int row = 0; row < GRID_ROWS; row++)
        for (int col = 0; col < GRID_COLS; col++)
            sum += grid[row][col];
    return sum;
}

int is_grid_stable() {
    if (history_size < 1) return 0;

    int prev_idx = (history_head - 1 + MAX_HISTORY) % MAX_HISTORY;

    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (grid[row][col] != history_buffer[prev_idx][row][col]) {
                return 0;
            }
        }
    }
    return 1;
}

void set_rule_type(RuleType type) {
    apply_rule_preset(type);
}

const char* get_rule_name(void) {
    return current_rule.name;
}

void set_boundary_type(int type) {
    boundary_mode = type;
}

int get_boundary_type(void) {
    return boundary_mode;
}