#ifndef GLOBAL_H
#define GLOBAL_H


#define HISTORY_LEN 200
#define UI_WIDTH 120
#define CHART_HEIGHT 100


extern int grid[100][100];
extern int heatmap[100][100];      
extern int neighbor_density[100][100]; 
extern int paused;
extern int generation;

extern int GRID_ROWS;
extern int GRID_COLS;
extern int CELL_SIZE;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

extern int GRID_OFFSET_X;
extern int GRID_OFFSET_Y;

extern int show_heatmap;
extern int heatmap_mode;           
extern int show_ai_vision;
extern int speed_level;
extern int fullscreen_mode;

extern int population_history[HISTORY_LEN];
extern int history_cursor;

#endif 