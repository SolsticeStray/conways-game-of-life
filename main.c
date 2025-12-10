#include <graphics.h>
#include <conio.h>
#include <windows.h>

//全局变量
int grid[100][100] = {0};
int GRID_ROWS = 60;
int GRID_COLS = 80;
int paused = 0;

//外部函数声明
void nextGeneration(void);
void initGrid(void);
void handleInput(void);
void render(void);

//system组要干的事
void loadConfig(void) {
    // TODO: 用 GetPrivateProfileInt 读 config.ini
}

void saveGame(void) {
    // TODO: 将 grid 写入 save.txt
}

void loadGame(void) {
    // TODO: 从 save.txt 读取 grid
}

//主函数
int main() {
    loadConfig();

    int cell_w = 800 / GRID_COLS;
    int cell_h = 600 / GRID_ROWS;
    initgraph(800, 600);
    BeginBatchDraw();

    initGrid(); // 初始状态

    while (1) {
        handleInput();          // 输入
        if (!paused) {
            nextGeneration();   // 演化
        }
        render();               // 渲染
        Sleep(100);             // 控制速度
    }

    EndBatchDraw();
    closegraph();
    return 0;
}