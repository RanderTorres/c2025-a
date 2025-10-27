#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

// 游戏元素定义
#define WALL '#'     // 墙
#define PLAYER '@'   // 玩家
#define BOX '$'      // 箱子
#define TARGET '.'   // 目标位置
#define BOX_ON_TARGET '*'  // 箱子在目标位置上
#define PLAYER_ON_TARGET '+' // 玩家在目标位置上
#define FLOOR ' '    // 空地

// 方向控制
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

// 游戏结构体
typedef struct {
    char **map;
    int width;
    int height;
    int player_x;
    int player_y;
    int box_count;
    int placed_boxes;
    int steps;
    int level;
} Game;

// 函数声明
void initGame(Game *game);
void loadLevel(Game *game, int level);
void drawMap(Game *game);
int movePlayer(Game *game, int dx, int dy);
int checkWin(Game *game);
void saveScore(Game *game);
void freeGame(Game *game);
void clearScreen();

int main() {
    Game game;
    int ch;
    int playing = 1;

    initGame(&game);

    printf("=== 推箱子游戏 ===\n");
    printf("操作说明:\n");
    printf("方向键 - 移动玩家\n");
    printf("R - 重新开始当前关卡\n");
    printf("Q - 退出游戏\n");
    printf("按任意键开始游戏...");
    getch();

    while (playing) {
        clearScreen();
        drawMap(&game);
        printf("\n关卡: %d  步数: %d  箱子: %d/%d\n",
               game.level, game.steps, game.placed_boxes, game.box_count);
        printf("操作: 方向键移动, R重新开始, Q退出\n");

        if (checkWin(&game)) {
            printf("\n恭喜！你通过了第%d关！\n", game.level);
            saveScore(&game);

            printf("按任意键进入下一关，或按Q退出...");
            ch = getch();
            if (ch == 'q' || ch == 'Q') {
                break;
            }

            game.level++;
            loadLevel(&game, game.level);
            continue;
        }

        ch = getch();
        if (ch == 224) { // 方向键前缀
            ch = getch();
            int dx = 0, dy = 0;

            switch (ch) {
                case UP: dy = -1; break;
                case DOWN: dy = 1; break;
                case LEFT: dx = -1; break;
                case RIGHT: dx = 1; break;
            }

            if (dx != 0 || dy != 0) {
                movePlayer(&game, dx, dy);
            }
        } else {
            switch (ch) {
                case 'r': case 'R': // 重新开始
                    loadLevel(&game, game.level);
                    break;
                case 'q': case 'Q': // 退出游戏
                    playing = 0;
                    break;
            }
        }
    }

    freeGame(&game);
    printf("\n游戏结束！感谢游玩！\n");
    return 0;
}

void initGame(Game *game) {
    game->map = NULL;
    game->width = 0;
    game->height = 0;
    game->player_x = 0;
    game->player_y = 0;
    game->box_count = 0;
    game->placed_boxes = 0;
    game->steps = 0;
    game->level = 1;
    loadLevel(game, game->level);
}

void loadLevel(Game *game, int level) {
    // 释放之前的地图
    freeGame(game);

    char filename[20];
    sprintf(filename, "level%d.txt", level);

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("无法加载关卡 %d，使用默认关卡\n", level);
        // 创建默认关卡
        game->width = 10;
        game->height = 8;
        game->box_count = 2;
        game->placed_boxes = 0;
        game->steps = 0;

        // 分配内存
        game->map = (char**)malloc(game->height * sizeof(char*));
        for (int i = 0; i < game->height; i++) {
            game->map[i] = (char*)malloc(game->width * sizeof(char));
        }

        // 默认地图
        char default_map[8][11] = {
            "##########",
            "#        #",
            "# $  @   #",
            "#        #",
            "#   .    #",
            "#   $    #",
            "#   .    #",
            "##########"
        };

        for (int y = 0; y < game->height; y++) {
            for (int x = 0; x < game->width; x++) {
                game->map[y][x] = default_map[y][x];
                if (game->map[y][x] == PLAYER) {
                    game->player_x = x;
                    game->player_y = y;
                }
            }
        }
        return;
    }

    // 读取地图尺寸
    fscanf(file, "%d %d", &game->width, &game->height);
    fgetc(file); // 读取换行符

    // 分配内存
    game->map = (char**)malloc(game->height * sizeof(char*));
    for (int i = 0; i < game->height; i++) {
        game->map[i] = (char*)malloc(game->width * sizeof(char));
    }

    // 读取地图数据
    game->box_count = 0;
    game->placed_boxes = 0;
    game->steps = 0;

    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            game->map[y][x] = fgetc(file);
            if (game->map[y][x] == PLAYER || game->map[y][x] == PLAYER_ON_TARGET) {
                game->player_x = x;
                game->player_y = y;
            }
            if (game->map[y][x] == BOX) {
                game->box_count++;
            } else if (game->map[y][x] == BOX_ON_TARGET) {
                game->box_count++;
                game->placed_boxes++;
            }
        }
        fgetc(file); // 读取换行符
    }

    fclose(file);
}

void drawMap(Game *game) {
    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            putchar(game->map[y][x]);
        }
        putchar('\n');
    }
}

int movePlayer(Game *game, int dx, int dy) {
    int new_x = game->player_x + dx;
    int new_y = game->player_y + dy;

    // 检查边界
    if (new_x < 0 || new_x >= game->width || new_y < 0 || new_y >= game->height) {
        return 0;
    }

    char current_cell = game->map[game->player_y][game->player_x];
    char next_cell = game->map[new_y][new_x];

    // 如果是墙，不能移动
    if (next_cell == WALL) {
        return 0;
    }

    // 如果是箱子，检查箱子后面是什么
    if (next_cell == BOX || next_cell == BOX_ON_TARGET) {
        int box_new_x = new_x + dx;
        int box_new_y = new_y + dy;

        // 检查箱子移动后的位置
        if (box_new_x < 0 || box_new_x >= game->width || box_new_y < 0 || box_new_y >= game->height) {
            return 0;
        }

        char box_next_cell = game->map[box_new_y][box_new_x];

        // 箱子后面是墙或另一个箱子，不能移动
        if (box_next_cell == WALL || box_next_cell == BOX || box_next_cell == BOX_ON_TARGET) {
            return 0;
        }

        // 移动箱子
        if (box_next_cell == TARGET) {
            game->map[box_new_y][box_new_x] = BOX_ON_TARGET;
            game->placed_boxes++;
        } else {
            game->map[box_new_y][box_new_x] = BOX;
        }

        // 更新原箱子位置
        if (next_cell == BOX_ON_TARGET) {
            game->map[new_y][new_x] = TARGET;
            game->placed_boxes--;
        } else {
            game->map[new_y][new_x] = FLOOR;
        }
    }

    // 移动玩家
    if (next_cell == FLOOR || next_cell == TARGET) {
        // 更新玩家原位置
        if (current_cell == PLAYER_ON_TARGET) {
            game->map[game->player_y][game->player_x] = TARGET;
        } else {
            game->map[game->player_y][game->player_x] = FLOOR;
        }

        // 设置玩家新位置
        if (next_cell == TARGET) {
            game->map[new_y][new_x] = PLAYER_ON_TARGET;
        } else {
            game->map[new_y][new_x] = PLAYER;
        }

        game->player_x = new_x;
        game->player_y = new_y;
        game->steps++;
        return 1;
    }

    return 0;
}

int checkWin(Game *game) {
    return game->placed_boxes == game->box_count;
}

void saveScore(Game *game) {
    FILE *file = fopen("scores.txt", "a");
    if (file) {
        fprintf(file, "关卡: %d, 步数: %d\n", game->level, game->steps);
        fclose(file);
    }
}

void freeGame(Game *game) {
    if (game->map) {
        for (int i = 0; i < game->height; i++) {
            free(game->map[i]);
        }
        free(game->map);
        game->map = NULL;
    }
}

void clearScreen() {
    system("cls");
}