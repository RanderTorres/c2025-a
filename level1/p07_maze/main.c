#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define WIDTH 15
#define HEIGHT 10

#define WALL '#'
#define PLAYER 'P'
#define EXIT 'E'
#define EMPTY ' '

// 方向控制
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

// 游戏状态
int gameRunning = 1;
int playerX = 1, playerY = 1;
int exitX = WIDTH - 2, exitY = HEIGHT - 2;

// 迷宫地图
char maze[HEIGHT][WIDTH] = {
    "###############",
    "#P            #",
    "# ##########  #",
    "# #        #  #",
    "# # ###### #  #",
    "# # #      #  #",
    "# # # ####### #",
    "#   #        E#",
    "#   ###########",
    "###############"
};


void drawMaze();
void movePlayer(int direction);
int isValidMove(int x, int y);
void checkWin();
void clearScreen();
void setCursorPosition(int x, int y);

int main() {
    printf("=== 迷宫小游戏 ===\n");
    printf("使用方向键控制玩家(P)移动到出口(E)\n");
    printf("按任意键开始游戏...");
    getch();

    while (gameRunning) {
        clearScreen();
        drawMaze();

        printf("\n使用方向键移动，按ESC退出游戏\n");


        int key = getch();

        // 处理特殊键（方向键）
        if (key == 0 || key == 224) {
            key = getch(); // 获取扩展键码
            movePlayer(key);
        }
        // 处理ESC键退出
        else if (key == 27) {
            gameRunning = 0;
        }

        checkWin();
    }

    return 0;
}


void drawMaze() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", maze[y][x]);
        }
        printf("\n");
    }
}


void movePlayer(int direction) {
    int newX = playerX;
    int newY = playerY;

    // 根据方向计算新位置
    switch (direction) {
        case UP:    newY--; break;
        case DOWN:  newY++; break;
        case LEFT:  newX--; break;
        case RIGHT: newX++; break;
        default: return; // 无效方向
    }

    // 检查移动是否有效
    if (isValidMove(newX, newY)) {
        // 清除原位置
        maze[playerY][playerX] = EMPTY;

        // 更新玩家位置
        playerX = newX;
        playerY = newY;

        // 在新位置绘制玩家
        maze[playerY][playerX] = PLAYER;
    }
}

// 检查移动是否有效
int isValidMove(int x, int y) {
    // 检查边界
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return 0;
    }

    // 检查是否是墙
    if (maze[y][x] == WALL) {
        return 0;
    }

    return 1;
}


void checkWin() {
    if (playerX == exitX && playerY == exitY) {
        clearScreen();
        printf("\n\n\n");
        printf("    *************************\n");
        printf("    *                       *\n");
        printf("    *     恭喜你赢了！      *\n");
        printf("    *                       *\n");
        printf("    *************************\n");
        printf("\n按任意键退出...");
        getch();
        gameRunning = 0;
    }
}



void clearScreen() {
    system("cls");
}


void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}