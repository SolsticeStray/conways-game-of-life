// render.h
//   Ⱦģ  ͷ ļ 

#ifndef RENDER_H
#define RENDER_H

#define ROWS 100
#define COLS 100
#define CELL_SIZE       20
#define GRID_OFFSET_X   50
#define GRID_OFFSET_Y   80

//   ʼ    Ⱦģ  
void initRenderer(int rows, int cols);

//  ر   Ⱦģ  
void closeRenderer(void);

//        񱳾 
void drawGrid(void);

//    Ƶ   ϸ  
void drawCell(int x, int y, int alive);

//   Ⱦ      Ϸ    
void render(int grid[100][100], int rows, int cols, int generation, int is_paused);

//   Ļ    ת        
int screenToGridX(int screen_x);
int screenToGridY(int screen_y);

// ֡      
void limitFPS(int target_fps);

//   ȡ  ǰFPS
float getFPS(void);

#endif