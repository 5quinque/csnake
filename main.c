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
void chop_snake();
void handleinput(int c);
void change_direction(int d);

struct point nexttail();
struct point nexthead(int d);

int **board;
unsigned int *snake_length;
int corners;
int snake_direction = RIGHT;
int pause = 0;
int running = 1;
int screen_rows, screen_cols, game_columns;

struct point {
  int y;
  int x;
};

struct point head;
struct point tail;

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
  //tempboard = malloc(screen_rows * sizeof(int *));

  for (int i = 0; i < screen_rows; i++) {
    board[i] = malloc(game_columns * sizeof(int));
    //tempboard[i] = malloc(game_columns * sizeof(int));
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
      change_direction(DOWN);
      break;
    case 's':
      change_direction(LEFT);
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
  int shift_bit = CHAR_BIT * sizeof(int) - 2;
  snake_direction = direction;

  corners += 1;
  snake_length = realloc(snake_length, corners * sizeof(int));

  snake_length[corners] = (snake_direction << shift_bit) | 1;
}

void chop_snake() {
  memmove(snake_length, snake_length + 1, corners * sizeof(int));

  corners -= 1;
  snake_length = realloc(snake_length, corners * sizeof(int));
}

struct point nexttail() {
  int shift_bit = CHAR_BIT * sizeof(int) - 2;
  struct point nexttail = tail;

  int direction = snake_length[0] >> shift_bit;
  int count = snake_length[0] & ~(3 >> shift_bit);

  //if (count < 1) {
  //  chop_snake();
  //} else {
    snake_length[0] -= 1;
  //}

  /*mvprintw(0, 0, "direction: %d", direction);*/
  switch (direction) {
    case UP:
      mvprintw(0, 0, "up: %d", nexttail.y);
      nexttail.y -= 1;
      break;
    case DOWN:
      nexttail.y += 1;
      break;
    case LEFT:
      nexttail.x -= 1;
      break;
    case RIGHT:
      mvprintw(0, 0, "right: %d", nexttail.x);
      nexttail.x += 1;
      break;
  }

  return nexttail;
}

void update() {
  head = nexthead(snake_direction);
  tail = nexttail();

  board[head.y][head.x] = 1;
  board[tail.y][tail.x] = 0;
 
  snake_length[corners] += 1;
}

struct point nexthead(int direction) {
  struct point nexthead = head;

  switch (direction) {
    case UP:
      nexthead.y -= 1;
      break;
    case DOWN:
      nexthead.y += 1;
      break;
    case LEFT:
      nexthead.x -= 1;
      break;
    case RIGHT:
      nexthead.x += 1;
      break;
  }

  return nexthead;
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
  int shift_bit = CHAR_BIT * sizeof(int) - 2;

  board[10][20] = 1;
  board[10][21] = 1;
  board[10][22] = 1;
  board[10][23] = 1;

  tail.y = 10;
  tail.x = 20;
  head.y = 10;
  head.x = 23;

  corners = 0;
  snake_length = malloc(1 * sizeof(int));

  snake_length[corners] = (snake_direction << shift_bit) | 3;
}
