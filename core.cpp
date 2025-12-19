#include "global.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include "core.h"
#define BOUNDARY_FIXED 0
#define BOUNDARY_TOROIDAL 1
int boundary_type = BOUNDARY_FIXED;
int set_boundary_type(int type) {               //       ñ߽紦       ǻ  δ      ǿհ״        ִ     ʽʵ  Ч    ͬ  
	if (type == BOUNDARY_FIXED || type == BOUNDARY_TOROIDAL) {
		boundary_type = type;
		return 1;
	}
	return 0;
}
int get_boundary_type(void) {
	return boundary_type;
}                                               //      ʾ ߽紦     򣬸          û  л 
int count_live_neighbours(int row, int col) {                        //    һ  ϸ   ھ  м      ŵ 
	int i, j, count = 0;
	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			int x, y;
			if (i == 0 && j == 0)
				continue;
			if (boundary_type == BOUNDARY_FIXED) {
				x = row + i;
				y = col + j;
				if (x<0 || x>=GRID_ROWS || y<0 || y>=GRID_COLS)
					continue;
			}
			else {
				x = (row + i + GRID_ROWS) % GRID_ROWS;
				y = (col + j + GRID_COLS) % GRID_COLS;
			}
			if (grid[x][y] == 1)
				count++;
		}
	}
	return count;
}
void  next_Generation() {                        //      һ  ϸ       洢  һ   ¿  ٵ       
	if (paused)
		return;
	int next_grid[100][100] = { 0 };
	int x, y;
	for (x = 0; x < GRID_ROWS; x++) {
		for (y = 0; y < GRID_COLS; y++) {
			int live_neigbours;
			live_neigbours = count_live_neighbours(x,y);
			if (grid[x][y] == 1) {
				if (live_neigbours < 2 || live_neigbours>3)
					next_grid[x][y] = 0;
				else if (live_neigbours == 2 || live_neigbours == 3)
					next_grid[x][y] = 1;
			}
			else {
				if (live_neigbours == 3)
					next_grid[x][y] = 1;
				else
					next_grid[x][y] = 0;
			}
		}
	}
	for (x = 0; x < GRID_ROWS; x++) {                               //      һ  ϸ  
		for (y = 0; y < GRID_COLS; y++)
			grid[x][y] = next_grid[x][y];
	}
}
void restart_grid() {                                              //       ý   
	for (int x = 0; x < GRID_ROWS; x++)
		for (int y = 0; y < GRID_COLS; y++)
			grid[x][y] = 0;
}
void randomize_grid() {                                            //           λ  ϸ   Ĵ  ״̬
	srand((unsigned int)time(NULL));
	int x, y;
	for (x = 0; x < GRID_ROWS; x++) {
		for (y = 0; y < GRID_COLS; y++)
			grid[x][y] = rand() % 2;
	}
}
int count_live_cells() {                                          //ͳ    Ч     ж  ٸ   ϸ  
	int x, y, sum = 0;
	for (x = 0; x < GRID_ROWS; x++) {
		for (y = 0; y < GRID_COLS; y++)
			sum += grid[x][y] == 1;
     }
	return sum;
}
int set_cell(int x, int y, int state) {                           //     ض ϸ    ״̬
	if (x >= 0 && x < GRID_ROWS && y >= 0 && y < GRID_COLS && (state == 0 || state == 1)) {
		grid[x][y] = state;
		return 1;
	}
	return 0;
}
int reverse_cell(int x, int y, int state) {                                 // л  ض ϸ    ״̬
	if (x >= 0 && x < GRID_ROWS && y >= 0 && y < GRID_COLS && (state == 0 || state == 1)) {
		grid[x][y] = !grid[x][y];
		return 1;
	}
	return 0;
}
int get_state(int x, int y) {                                            //  ȡ ض ϸ    ״̬
	if (x >= 0 && x < GRID_ROWS && y >= 0 && y < GRID_COLS) {
		return grid[x][y];
	}
	return 0;
}
int is_grid_stable() {                                        //   ܼ     Զ   ͣ,  ֹ        ȶ ״̬   հ orѭ               Դ ˷ 
	static int last_grid[100][100] = { 0 };
	int changed = 0;
	int x, y;
	for (x = 0; x < GRID_ROWS; x++) {
		for (y = 0; y < GRID_COLS; y++) {
			if (grid[x][y] != last_grid[x][y]) {
				changed = 1;
			}
			last_grid[x][y] = grid[x][y];
		}
	}
    return !changed;
}
void place_pattern(int pattern[][2], int pattern_size, int start_x, int start_y) {  //    ʵ      ͼ   ⣬Ԥ 賡  
	int i;
	for (i = 0; i < pattern_size; i++) {
		int x = start_x + pattern[i][0];
		int y = start_y + pattern[i][1];
		if (x >= 0 && x < GRID_ROWS && y >= 0 && y < GRID_COLS) {
			grid[x][y] = 1;
		}
	}
}
float get_region_density(int start_x, int start_y, int width, int height) {      //    ϸ   ܶȣ   Ҳ  ֪    ʲô ô   
	int live_count = 0;
	int total_cells = 0;
    for (int x = start_x; x < start_x + width && x < GRID_ROWS; x++) {
		if (x < 0) 
			continue;
		for (int y = start_y; y < start_y + height && y < GRID_COLS; y++) {
			if (y < 0) 
				continue;
			live_count += grid[x][y];
			total_cells++;
		}
	}
    if (total_cells == 0) 
		return 0.0f;
	return (float)live_count / total_cells;
}

EdgeCounts get_edge_counts(void) {
	EdgeCounts counts = { 0, 0, 0, 0 };  // һ    ʼ  Ϊ0
    /*      ϱ߽磨  0 У  */
	for (int j = 0; j < GRID_COLS; j++) {
		counts.top += grid[0][j];
	}
    /*      ±߽磨   һ У  */
	int last_row = GRID_ROWS - 1;
	for (int j = 0; j < GRID_COLS; j++) {
		counts.bottom += grid[last_row][j];
	}
    /*       ߽磨  0 У  */
	for (int i = 0; i < GRID_ROWS; i++) {
		counts.left += grid[i][0];
	}
    /*      ұ߽磨   һ У  */
	int last_col = GRID_COLS - 1;
	for (int i = 0; i < GRID_ROWS; i++) {
		counts.right += grid[i][last_col];
	}
    return counts;
}




