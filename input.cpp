#include "global.h"
#define _CRT_SECURE_NO_WARNINGS

#include<graphics.h>
#include<conio.h>
#include<windows.h>
#include<stdio.h>
#include<string.h>
#include"core.h"
#include"render.h"

extern char message[64] = {0};
extern int message_timer=0;

//      戏 
void saveGame(void)
{
    FILE* fp = fopen("save.txt", "w");
    if (fp == NULL)
    {
        strcpy(message, "Save Failed!");
        message_timer = 20;
        return;
    }

    //    叽 
    fprintf(fp, "%d %d\n", GRID_ROWS, GRID_COLS);

    //细    状态
    for (int i = 0; i < GRID_ROWS; i++)
    {
        for (int j = 0; j < GRID_COLS; j++)
        {
            fprintf(fp, "%d ", grid[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    //    晒 
    strcpy(message, "Saved!");
    message_timer = 20;
}

//   save.txt  募       戏状态

void loadGame(void)
{
    FILE* fp = fopen("save.txt", "r");
    if (fp == NULL)
    {
        strcpy(message, "Load Failed!");
        message_timer = 20;
        return;
    }

    int rows, cols;

    //  取    叽 
    fscanf(fp, "%d %d", &rows, &cols);

    //  取细  状态
    for (int i = 0; i < rows && i < GRID_ROWS; i++)
    {
        for (int j = 0; j < cols && j < GRID_COLS; j++)
        {
            fscanf(fp, "%d", &grid[i][j]);
        }
    }

    fclose(fp);

    //  示   爻晒   示
    strcpy(message, "Loaded!");
    message_timer = 20;
}

//       氪?                                                                              

void handleKeyboard(void)
{
    if (!_kbhit())
    {
        return;
    }

    char ch = _getch();

    switch (ch)
    {
        // 崭   停/    
    case ' ':
        paused = !paused;
        if (paused)
        {
            strcpy(message, "Paused");
        }
        else
        {
            strcpy(message, "Running");
        }
        message_timer = 15;
        break;

        //R         
    case 'r':
    case 'R':
        randomize_grid();
        strcpy(message, "Randomized!");
        message_timer = 20;
        break;

        //C     
    case 'c':
    case 'C':
        restart_grid();
        strcpy(message, "Cleared!");
        message_timer = 20;
        break;

        //S      
    case 's':
    case 'S':
        saveGame();
        break;

        //L      
    case 'l':
    case 'L':
        loadGame();
        break;

        //ESC   顺   戏
    case 27:
        EndBatchDraw();
        closegraph();
        exit(0);
        break;

    default:
        break;
    }
}

//      氪?                                                                                  
//    幕    转  为          
int screenToGridCol(int screen_x)
{
    int col = (screen_x - GRID_OFFSET_X) / CELL_SIZE;
    if (col < 0 || col >= GRID_COLS)
    {
        return -1;
    }
    return col;
}

//    幕    转  为          
int screenToGridRow(int screen_y)
{
    int row = (screen_y - GRID_OFFSET_Y) / CELL_SIZE;
    if (row < 0 || row >= GRID_ROWS)
    {
        return -1;
    }
    return row;
}

//      
void handleMouse(void)
{
    while (MouseHit())
    {
        MOUSEMSG msg = GetMouseMsg();

        if (msg.uMsg == WM_LBUTTONDOWN)
        {
            int col = screenToGridCol(msg.x);
            int row = screenToGridRow(msg.y);

            if (row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS)
            {
                //  谢 细  状态
                grid[row][col] = !grid[row][col];
            }
        }
    }
}

void handleInput(void)
{
    handleKeyboard();
    handleMouse();
}