#include <ncurses.h>
// memcpy(), memset()
#include <string.h>
// time(), nanosleep()
#include <time.h>
// srand(), rand(), malloc()
#include <stdlib.h>
// CHAR_BIT
#include <limits.h>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

void update();
void printgame();
void birthsnake();
void handleinput(int c);
void change_direction(int d);
struct ends next();

int **board;
int snake_length;
struct point *snake;

int snake_direction = RIGHT;
int pause = 0;
int running = 1;
int screen_rows, screen_cols, game_columns;

struct point {
  int y;
  int x;
};

struct ends {
  struct point head;
  struct point tail;
};

int main() {
  struct timespec ts = {0, 100000000L};
  int c;

  srand(time(NULL));

  initscr();

  // Hide the cursor
  curs_set(0);

  // Don't show keypress on screen
  cbreak();
  noecho();

  // Enable colours
  start_color();
  // Snake
  init_pair(2, COLOR_GREEN, COLOR_GREEN);
  // Food
  init_pair(4, COLOR_RED, COLOR_RED);
  // Background
  init_pair(3, COLOR_BLACK, COLOR_BLACK);

  // Stop getch() from blocking
  nodelay(stdscr, TRUE);

  getmaxyx(stdscr, screen_rows, screen_cols);
  game_columns = screen_cols / 2;

  board = malloc(screen_rows * sizeof(int *));

  for (int i = 0; i < screen_rows; i++) {
    board[i] = malloc(game_columns * sizeof(int));
  }

  birthsnake();

  while (running) {
    c = getch();
    if (~c >> 31) handleinput(c);

    if (!pause) update();

    printgame();
    refresh();
    nanosleep(&ts, NULL);
  }

  // Clean up after ourselves
  endwin();

  return 0;
}

void handleinput(int c) {
  switch (c) {
    case 'w':
      change_direction(UP);
      break;
    case 'a':
      change_direction(LEFT);
      break;
    case 's':
      change_direction(DOWN);
      break;
    case 'd':
      change_direction(RIGHT);
      break;
    case ' ':
      // toggle pause
      pause ^= 1;
      break;
    case 'q':
      running = 0;
      break;
  }
}

void change_direction(int direction) {
  if ( snake_direction == direction ) return;
  if ( snake_direction == UP && direction == DOWN) return;
  if ( snake_direction == DOWN && direction == UP) return;
  if ( snake_direction == LEFT && direction == RIGHT) return;
  if ( snake_direction == RIGHT && direction == LEFT) return;

  snake_direction = direction;
}

void update() {
  struct ends nextthing = next(snake_direction);
  struct point head = nextthing.head;
  struct point tail = nextthing.tail;

  mvprintw(0, 5, "y: %d\tx: %d\t length: %d\t", head.y, head.x, snake_length);
  board[head.y][head.x] = 1;
  board[tail.y][tail.x] = 0;
}

struct ends next(int direction) {
  struct ends next;
  next.head = snake[snake_length];
  next.tail = snake[1];

  switch (direction) {
    case UP:
      /*mvprintw(0, 0, "up: %d", nexthead.y);*/
      next.head.y -= 1;
      break;
    case DOWN:
      //mvprintw(0, 0, "down: %d", nexthead.y);
      next.head.y += 1;
      break;
    case LEFT:
      next.head.x -= 1;
      break;
    case RIGHT:
      next.head.x += 1;
      break;
  }


  // works -
  //snake_length += 1;
  //snake = realloc(snake, snake_length * sizeof *snake);
  //--


  memmove(snake, snake + 1, snake_length * sizeof *snake);
  snake[snake_length] = next.head;
  snake[0] = next.tail;

  return next;
}

void printgame() {
  int c;
  for (int y = 0; y < screen_rows; y++) {
    for (int x = 0; x < game_columns; x++) {
      c = board[y][x] ^ 3;
      attron(COLOR_PAIR(c));
      mvaddch(y + 1, x * 2, '.');
      mvaddch(y + 1, x * 2 + 1, '.');
      attroff(COLOR_PAIR(c));
    }
  }
  if (pause) mvprintw(1, screen_cols / 2 - 3, "Paused");
}

void birthsnake() {
  board[10][20] = 1;
  board[10][21] = 1;
  board[10][22] = 1;
  board[10][23] = 1;
  board[10][24] = 1;
  board[10][26] = 1;

  snake = malloc(6 * sizeof *snake);
  snake_length = 5;

  snake[0].y = 10;
  snake[0].x = 20;
  snake[1].y = 10;
  snake[1].x = 21;
  snake[2].y = 10;
  snake[2].x = 22;
  snake[3].y = 10;
  snake[3].x = 23;
  snake[3].y = 10;
  snake[3].x = 24;
  snake[3].y = 10;
  snake[3].x = 25;
}
