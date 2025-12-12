extern int grid[100][100];   // 1=活, 0=死
extern int GRID_ROWS, GRID_COLS;
extern int paused;           // 0=运行, 1=暂停
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include "core.h"
#define BOUNDARY_FIXED 0
#define BOUNDARY_TOROIDAL 1
int boundary_type = BOUNDARY_FIXED;
void set_boundary_type(int type) {               //用来设置边界处理规则，是环形处理还是空白处理（两种处理方式实现效果不同）
	if (type == BOUNDARY_FIXED || type == BOUNDARY_TOROIDAL) {
		boundary_type = type;
		return 1;
	}
	return 0;
}
int get_boundary_type(void) {
	return boundary_type;
}                                               //用来显示边界处理规则，更清晰地让用户切换
int count_live_neighbours(int row, int col) {                        //计算一个细胞邻居有几个活着的
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
void  next_Generation() {                        //生成下一代细胞，并存储在一个新开辟的数组中
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
	for (x = 0; x < GRID_ROWS; x++) {                               //更新新一代细胞
		for (y = 0; y < GRID_COLS; y++)
			grid[x][y] = next_grid[x][y];
	}
}
void restart_grid() {                                              //用于重置界面
	for (x = 0; x < GRID_ROWS; x++)
		for (y = 0; y < GRID_COLS; y++)
			grid[x][y] = 0;
}
void randomize_grid() {                                            //随机生成任意位置细胞的存活状态
	srand((unsigned int)time(NULL));
	int x, y;
	for (x = 0; x < GRID_ROWS; x++) {
		for (y = 0; y < GRID_COLS; y++)
			grid[x][y] = rand() % 2;
	}
}
int count_live_cells() {                                          //统计有效区域有多少个活细胞
	int x, y, sum = 0;
	for (x = 0; x < GRID_ROWS; x++) {
		for (y = 0; y < GRID_COLS; y++)
			sum += grid[x][y] == 1;
     }
	return sum;
}
int set_cell(int x, int y, int state) {                           //设置特定细胞的状态
	if (x >= 0 && x < GRID_ROWS && y >= 0 && y < GRID_COLS && (state == 0 || state == 1)) {
		grid[x][y] = state;
		return 1;
	}
	return 0;
}
int reverse_cell(int x, int y，int state) {                                 //切换特定细胞的状态
	if (x >= 0 && x < GRID_ROWS && y >= 0 && y < GRID_COLS && (state == 0 || state == 1)) {
		grid[x][y] = !grid[x][y];
		return 1;
	}
	return 0;
}
int get_state(int x, int y) {                                            //获取特定细胞的状态
	if (x >= 0 && x < GRID_ROWS && y >= 0 && y < GRID_COLS) {
		return grid[x][y];
	}
	return 0;
}
int is_grid_stable() {                                        //智能检测与自动暂停,防止网格进入稳定状态（空白or循环）后无意义的资源浪费
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
void place_pattern(int pattern[][2], int pattern_size, int start_x, int start_y) {  //可以实现智能图案库，预设场景
	int i;
	for (i = 0; i < pattern_size; i++) {
		int x = start_x + pattern[i][0];
		int y = start_y + pattern[i][1];
		if (x >= 0 && x < GRID_rows && y >= 0 && y < GRID_cols) {
			grid[x][y] = 1;
		}
	}
}
float get_region_density(int start_x, int start_y, int width, int height) {      //计算细胞密度（我也不知道有什么用处）
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
typedef struct {
	int top, bottom, left, right;  // 四边活细胞数
} EdgeCounts;
EdgeCounts get_edge_counts(void) {
	EdgeCounts counts = { 0, 0, 0, 0 };  // 一步初始化为0
    /* 计算上边界（第0行） */
	for (int j = 0; j < GRID_cols; j++) {
		counts.top += grid[0][j];
	}
    /* 计算下边界（最后一行） */
	int last_row = GRID_rows - 1;
	for (int j = 0; j < GRID_cols; j++) {
		counts.bottom += grid[last_row][j];
	}
    /* 计算左边界（第0列） */
	for (int i = 0; i < GRID_rows; i++) {
		counts.left += grid[i][0];
	}
    /* 计算右边界（最后一列） */
	int last_col = GRID_cols - 1;
	for (int i = 0; i < GRID_rows; i++) {
		counts.right += grid[i][last_col];
	}
    return counts;
}





