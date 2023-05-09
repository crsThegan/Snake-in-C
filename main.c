#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <Windows.h>

#define WIDTH 50
#define HEIGHT 20

char board[WIDTH][HEIGHT];
bool gameOver = false;
int del, stdDel = 25;

int *snakeX, *snakeY, snakeLen = 1;
int prevX, prevY;

int fruitX, fruitY;
int score = 0;

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum Direction dir = RIGHT;

void setup(char (*board)[HEIGHT]);
void draw(char (*board)[HEIGHT]);
void changeBoard(char (*board)[HEIGHT]);
void logics(char (*board)[HEIGHT]);
void terminate();

void setColor_green();
void setColor_red();
void resetColor();

int main() {
    setup(board);
    draw(board);
    while (!gameOver) {
        if (kbhit()) {
            switch (getch()) {
            case 'w':
                dir = UP;
                del = stdDel * 2;
                break;
            case 'd':
                dir = RIGHT;
                del = stdDel;
                break;
            case 's':
                dir = DOWN;
                del = stdDel * 2;
                break;
            case 'a':
                dir = LEFT;
                del = stdDel;
                break;
            case 'p':
                gameOver = true;
                break;
            }
        }
        logics(board);
        changeBoard(board);
        system("cls");
        draw(board);

        Sleep(del);
    }
    system("cls");
    draw(board);
    terminate();
    system("pause");
    return 0;
}

void setup(char (*board)[HEIGHT]) {
    memset(board, ' ', WIDTH * HEIGHT);

    del = stdDel;

    srand(time(NULL));
    fruitX = rand() % (WIDTH - 2) + 1;
    fruitY = rand() % (HEIGHT - 2) + 1;

    snakeX = malloc(sizeof(int) * 1);
    snakeY = malloc(sizeof(int) * 1);
    snakeX[0] = WIDTH / 2;
    snakeY[0] = HEIGHT / 2;

    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++) {
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) board[x][y] = '#';
            else if (x == snakeX[0] && y == snakeY[0]) board[x][y] = 'O';
        }
}

void draw(char (*board)[HEIGHT]) {
    char boardStr[HEIGHT][WIDTH + 64] = {0};
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            boardStr[y][x] = board[x][y];
        }
        if (y == HEIGHT / 2) strcat(boardStr[y], "     Controls: W, A, S, D. P - stop the game");
        else if (y == HEIGHT / 2 - 1) {
            char tempbuf[32] = {0};
            sprintf(tempbuf, "     Score: %d", score);
            strcat(boardStr[y], tempbuf);
        }
        else if (gameOver && y == HEIGHT / 2 - 2) strcat(boardStr[y], "     Game over!");
        puts(boardStr[y]);
    }
}

void changeBoard(char (*board)[HEIGHT]) {
    for (int x = 1; x < WIDTH - 1; x++) {
        for (int y = 1; y < HEIGHT - 1; y++) {
            board[x][y] = ' ';
            if (x == fruitX && y == fruitY) board[x][y] = 'F';
            else if (x == snakeX[0] && y == snakeY[0]) board[x][y] = 'O';
            else if (board[x][y] == ' ') {
                for (int i = 1; i < snakeLen; i++) {
                    if (x == snakeX[i] && y == snakeY[i]) board[x][y] = 'o';
                }
            }
            else board[x][y] = ' ';

        }
    }
}

void logics(char (*board)[HEIGHT]) {
    prevX = snakeX[snakeLen - 1];
    prevY = snakeY[snakeLen - 1];

    memmove(snakeX + 1, snakeX, sizeof(int) * (snakeLen - 1));
    memmove(snakeY + 1, snakeY, sizeof(int) * (snakeLen - 1));

    switch (dir) {
    case RIGHT:
        snakeX[0]++;
        break;
    case LEFT:
        snakeX[0]--;
        break;
    case UP:
        snakeY[0]++;
        break;
    case DOWN:
        snakeY[0]--;
        break;
    }

    if (snakeX[0] == WIDTH - 1) snakeX[0] = 0;
    else if (snakeX[0] == 0) snakeX[0] = WIDTH - 2;
    else if (snakeY[0] == HEIGHT - 1) snakeY[0] = 0;
    else if (snakeY[0] == 0) snakeY[0] = HEIGHT - 2;

    for (int i = 1; i < snakeLen; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) gameOver = true;
    }

    if (snakeX[0] == fruitX && snakeY[0] == fruitY) {
        score += 5;

        snakeLen++;
        int *tempX = realloc(snakeX, sizeof(int) * snakeLen);
        int *tempY = realloc(snakeY, sizeof(int) * snakeLen);
        assert(tempX != NULL && tempY != NULL);
        snakeX = tempX;
        snakeY = tempY;
        tempX = tempY = NULL;

        snakeX[snakeLen - 1] = prevX;
        snakeX[snakeLen - 1] = prevY;

        fruitX = rand() % (WIDTH - 2) + 1;
        fruitY = rand() % (HEIGHT - 2) + 1;
        while (board[fruitX][fruitY] == 'O' || board[fruitX][fruitY] == 'o') {
            fruitX = rand() % (WIDTH - 2) + 1;
            fruitY = rand() % (HEIGHT - 2) + 1;
        }
    }

}

void terminate() {
    free(snakeX);
    free(snakeY);
    snakeX = snakeY = NULL;
}

void setColor_green() {
    printf("\033[0;32m");
}

void setColor_red() {
    printf("\033[0;31m");
}

void resetColor() {
    printf("\033[0m");
}