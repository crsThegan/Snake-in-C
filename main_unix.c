#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define WIDTH 40
#define HEIGHT 15

#define BLANK_CH ' '
#define WALL_CH '#'
#define SNAKEHEAD_CH 'O'
#define SNAKEEL_CH 'o'
#define FRUIT_CH 'F'

char board[WIDTH][HEIGHT] = {0};
int game_over = 0;
int snake_len = 1;
int *snake_x, *snake_y;
int fruit_x, fruit_y;
int prev_x, prev_y;
int score = 0;
int delay = 50;

pthread_t kb_thr;

enum {
    DIR_UP,
    DIR_DOWN,
    DIR_RIGHT,
    DIR_LEFT
} dir = DIR_RIGHT;

void gen_fruit() {
    fruit_x = rand() % (WIDTH - 3) + 1;
    fruit_y = rand() % (HEIGHT - 3) + 1;
}

void setup() {
    srand(time(NULL));
    gen_fruit();

    snake_x = malloc(sizeof(int));
    snake_y = malloc(sizeof(int));
    snake_x[0] = WIDTH / 2;
    snake_y[0] = HEIGHT / 2;

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1)
                board[x][y] = WALL_CH;
        }
    }
}

void check_board() {
    for (int x = 1; x < WIDTH - 1; x++) {
        for (int y = 1; y < HEIGHT - 1; y++) {
            if (fruit_x == x && fruit_y == y) board[x][y] = FRUIT_CH;
            else if (snake_x[0] == x && snake_y[0] == y) board[x][y] = SNAKEHEAD_CH;
            else board[x][y] = BLANK_CH;
            for (int i = 1; i < snake_len; i++) {
                if (snake_x[i] == x && snake_y[i] == y) board[x][y] = SNAKEEL_CH;
            }
        }
    }
}

void draw_board() {
    clrtobot();
    move(0, 0);
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            addch(board[x][y]);
        }
        addch('\n');
    }
    refresh();
}

void logics() {
    prev_x = snake_x[snake_len - 1];
    prev_y = snake_y[snake_len - 1];

    memmove(snake_x + 1, snake_x, (snake_len - 1) * sizeof(int));
    memmove(snake_y + 1, snake_y, (snake_len - 1) * sizeof(int));
    switch (dir) {
    case DIR_UP:
        snake_y[0]++;
        usleep(delay * 2 * 1000);
        break;
    case DIR_DOWN:
        snake_y[0]--;
        usleep(delay * 2 * 1000);
        break;
    case DIR_RIGHT:
        snake_x[0]++;
        usleep(delay * 1000);
        break;
    case DIR_LEFT:
        snake_x[0]--;
        usleep(delay * 1000);
        break;
    }

    if (snake_x[0] == WIDTH - 1) snake_x[0] = 1;
    else if (snake_x[0] == 0) snake_x[0] = WIDTH - 2;
    else if (snake_y[0] == HEIGHT - 1) snake_y[0] = 1;
    else if (snake_y[0] == 0) snake_y[0] = HEIGHT - 2;

    for (int i = 1; i < snake_len; i++) {
        if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i])
            game_over = 1;
    }

    if (snake_x[0] == fruit_x && snake_y[0] == fruit_y) {
        score += 5;
        while (snake_x[0] == fruit_x && snake_y[0] == fruit_y)
            gen_fruit();
        snake_len++;
        int *temp_x = realloc(snake_x, snake_len * sizeof(int));
        int *temp_y = realloc(snake_y, snake_len * sizeof(int));
        if (!temp_x || !temp_y) exit(1);
        snake_x = temp_x;
        snake_y = temp_y;
        snake_x[snake_len - 1] = prev_x;
        snake_y[snake_len - 1] = prev_y;
    }
}

void *check_kb(void *noarg) {
    while (!game_over) {
        switch (getch()) {
        case 'w':
            dir = DIR_UP;
            break;
        case 'd':
            dir = DIR_RIGHT;
            break;
        case 'a':
            dir = DIR_LEFT;
            break;
        case 's':
            dir = DIR_DOWN;
            break;
        case 'p':
            game_over = 1;
            break;
        }
    }
    return NULL;
}

int main() {
    initscr();
    noecho();

    setup();
    pthread_create(&kb_thr, NULL, check_kb, NULL);

    while (!game_over) {
        logics();
        check_board();
        draw_board();
    }

    pthread_join(kb_thr, NULL);
    free(snake_x);
    free(snake_y);
    getch();
    endwin();

    return 0;
}
