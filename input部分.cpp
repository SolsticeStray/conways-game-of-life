//input部分 
#define _CRT_SECURE_NO_WARNINGS

#include<graphics.h>
#include<conio.h>
#include<windows.h>
#include<stdio.h>
#include<string.h>

extern int grid[100][100];
extern int GRID_ROWS;
extern int GRID_COLS;
extern int paused;
extern int CELL_SIZE;
extern int OFFSET_X;
extern int OFFSET_Y;
extern char message[64];
extern int message_timer;

extern void next_Generation(void);
extern void randomize_grid(void);
extern void restart_grid(void);

//保存游戏 
void saveGame(void)
{
    FILE*fp=fopen("save.txt", "w");
    if(fp==NULL)
	{
        strcpy(message,"Save Failed!");
        message_timer=20;
        return;
    }

    //网格尺寸
    fprintf(fp,"%d %d\n",GRID_ROWS,GRID_COLS);

    //细胞的状态
    for (int i=0;i<GRID_ROWS;i++)
	{
        for(int j=0;j<GRID_COLS;j++)
		{
            fprintf(fp,"%d ",grid[i][j]);
        }
        fprintf(fp,"\n");
    }
    
    fclose(fp);
    
    //保存成功
    strcpy(message,"Saved!");
    message_timer=20;
}

//从 save.txt 文件加载游戏状态

void loadGame(void)
{
    FILE* fp=fopen("save.txt","r");
    if(fp==NULL)
	{
        strcpy(message,"Load Failed!");
        message_timer=20;
        return;
    }
    
    int rows,cols;
    
    //读取网格尺寸
    fscanf(fp,"%d %d",&rows,&cols);
    
    //读取细胞状态
    for (int i=0;i<rows&&i<GRID_ROWS;i++)
	{
        for(int j=0;j<cols&&j<GRID_COLS;j++)
		{
            fscanf(fp,"%d",&grid[i][j]);
        }
    }
    
    fclose(fp);
    
    //显示加载成功提示
    strcpy(message,"Loaded!");
    message_timer=20;
}

//键盘输入处理—————————————————————————————————————— 

void handleKeyboard(void)
{
    if(!_kbhit())
	{
        return;
    }

    char ch=_getch();

    switch(ch)
	{
        //空格：暂停/继续
        case ' ':
            paused=!paused;
            if(paused)
			{
                strcpy(message,"Paused");
            }
			else
			{
                strcpy(message,"Running");
            }
            message_timer=15;
            break;
        
        //R：随机重置
        case 'r':
        case 'R':
            randomize_grid();
            strcpy(message,"Randomized!");
            message_timer=20;
            break;
        
        //C：清空
        case 'c':
        case 'C':
            restart_grid();
            strcpy(message,"Cleared!");
            message_timer=20;
            break;
        
        //S：保存
        case 's':
        case 'S':
            saveGame();
            break;
        
        //L：加载
        case 'l':
        case 'L':
            loadGame();
            break;
        
        //ESC：退出游戏
        case 27:
            EndBatchDraw();
            closegraph();
            exit(0);
            break;
        
        default: 
            break;
    }
}

//鼠标输入处理———————————————————————————————————————— 
//将屏幕坐标转换为网格列坐标
int screenToGridCol(int screen_x)
{
    int col=(screen_x-OFFSET_X)/CELL_SIZE;
    if(col<0||col>=GRID_COLS)
	{
        return -1;
    }
    return col;
}

//将屏幕坐标转换为网格行坐标
int screenToGridRow(int screen_y)
{
    int row=(screen_y-OFFSET_Y)/CELL_SIZE;
    if(row<0||row>=GRID_ROWS)
	{
        return -1;
    }
    return row;
}

//鼠标点击 
void handleMouse(void)
{
    while (MouseHit())
	{
        MOUSEMSG msg=GetMouseMsg();
        
        if(msg.uMsg==WM_LBUTTONDOWN)
		{
            int col=screenToGridCol(msg.x);
            int row=screenToGridRow(msg.y);
            
            if(row>=0&&row<GRID_ROWS&&col>=0&&col<GRID_COLS)
			{
                // 切换细胞状态
                grid[row][col]=!grid[row][col];
            }
        }
    }
}

void handleInput(void)
{
    handleKeyboard();
    handleMouse();
}
