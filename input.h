#ifndef INPUT_H
#define INPUT_H

#include <tchar.h>

// ==========================================
//  工具类型枚举
// ==========================================
typedef enum {
    TOOL_PEN = 0,
    TOOL_ERASER,
    TOOL_STAMP_GLIDER,
    TOOL_STAMP_LWSS,
    TOOL_STAMP_PULSAR,
    TOOL_STAMP_GOSPER
} ToolType;

// ==========================================
//  按钮结构体
// ==========================================
typedef struct {
    int x, y, w, h;
    int id;
    TCHAR text[32];
    bool is_hovered;
    bool is_clicked;
} Button;

// ==========================================
//  外部变量声明
// ==========================================
extern ToolType current_tool;
extern int mouse_grid_x;
extern int mouse_grid_y;

// ==========================================
//  输入函数声明
// ==========================================
void initInput(void);
void handleInput(void);

Button* getButtonList(void);
int getButtonCount(void);

void saveGame(void);
void loadGame(void);
void saveGameSlot(int slot_id);
void loadGameSlot(int slot_id);

#endif // INPUT_H