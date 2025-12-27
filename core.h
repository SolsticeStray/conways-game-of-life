#ifndef CORE_H
#define CORE_H

// ==========================================
//  规则类型枚举
// ==========================================
typedef enum {
    RULE_CONWAY = 0,
    RULE_HIGHLIFE,
    RULE_DAY_NIGHT,
    RULE_SEEDS
} RuleType;

// ==========================================
//  核心函数声明
// ==========================================
void core_init(void);
void next_Generation(void);
void restart_grid(void);
void randomize_grid(float density);
int count_live_cells(void);
int is_grid_stable(void);

// 规则系统
void set_rule_type(RuleType type);
const char* get_rule_name(void);

// 边界模式
void set_boundary_type(int type);
int get_boundary_type(void);

// 历史/撤销系统
void save_snapshot(void);
int undo_generation(void);
int get_history_count(void);

// RLE 解析
void load_rle_pattern(int start_x, int start_y, const char* rle);

#endif // CORE_H