#pragma once
#ifndef CORE_H
#define CORE_H
#ifdef __cplusplus
extern "C" {
#endif
#define BOUNDARY_FIXED 0
#define BOUNDARY_TOROIDAL 1
	void core_init(void);
	void core_update(void);
	void core_randomize(float density);
	void core_clear(void);
	int core_count_live_cells(void);
	int core_set_cell(int x, int y, int state);
	int core_get_cell(int x, int y);
	int set_boundary_type(int type);
	int get_boundary_type(void);
	void next_Generation(void);
	void restart_grid(void);
	void randomize_grid(void);
	int count_live_cells(void);
	int set_cell(int x, int y, int state);
	int reverse_cell(int x, int y);
	int get_state(int x, int y);
	int is_grid_stable(void);
	void place_pattern(int pattern[][2], int pattern_size, int start_x, int start_y);
	float get_region_density(int start_x, int start_y, int width, int height)£»
		typedef struct {
		int top, bottom, left, right;
	} EdgeCounts;
    EdgeCounts get_edge_counts(void);
#ifdef __cplusplus
}
#endif
#endif 