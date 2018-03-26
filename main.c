#include <ncurses.h>
// memcpy(), memset()
#include <string.h>
// time(), nanosleep()
#include <time.h>
// srand(), rand(), malloc()
#include <stdlib.h>
// CHAR_BIT
#include <limits.h>

#define ROWS 20
#define COLS 20

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

void update();
void printgame();
void printgameover();
void birthsnake();
void makeapple();
void newgame();
void handleinput(int c);
void change_direction(int d);
void remove_tail();
int snaketouchingitself();
int snaketouchingapple();

int snake_length;
struct point *snake;
struct point apple;
int snake_direction;
int pause = 0;
int gameover = 0;
int running = 1;
int score;
int screen_rows, screen_cols;
int screen_center_row, screen_center_col, yoffset, xoffset;

struct point {
  int y;
  int x;
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
  init_pair(1, COLOR_GREEN, COLOR_GREEN);
  // Food
  init_pair(2, COLOR_RED, COLOR_RED);
  // Background
  init_pair(3, COLOR_BLACK, COLOR_BLACK);

  // Stop getch() from blocking
  nodelay(stdscr, TRUE);

  getmaxyx(stdscr, screen_rows, screen_cols);
  screen_center_row = screen_rows / 2;
  screen_center_col = screen_cols / 2;
  yoffset = screen_center_row - (ROWS / 2);
  xoffset = screen_center_col - COLS;

  newgame();

  while (running) {
    c = getch();
    if (~c >> 31) handleinput(c);

    if (!pause && !gameover) {
      update();
      printgame();
    }
    if (gameover) printgameover();

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
      if (pause) {
        mvprintw(yoffset, screen_cols / 2 - 3, "Paused");
      } else {
        move(yoffset, 0);
        clrtoeol();
        mvhline(yoffset, xoffset, '-', COLS * 2);
        mvaddch(yoffset, xoffset, '+');
        mvaddch(yoffset, xoffset + (COLS * 2), '+');
      }
      break;
    case 'q':
      running = 0;
      break;
    case 'n':
      newgame();
  }
}

void change_direction(int direction) {
  if (snake_direction == direction) return;
  if (snake_direction == UP && direction == DOWN) return;
  if (snake_direction == DOWN && direction == UP) return;
  if (snake_direction == LEFT && direction == RIGHT) return;
  if (snake_direction == RIGHT && direction == LEFT) return;
  if (pause) return;

  snake_direction = direction;
}

void remove_tail() {
  struct point tail = snake[0];

  attron(COLOR_PAIR(3));
  mvprintw(tail.y + yoffset, tail.x * 2 + xoffset, "ss");
  attroff(COLOR_PAIR(3));
}

void update() {
  struct point head = snake[snake_length];

  switch (snake_direction) {
    case UP:
      head.y--;
      break;
    case DOWN:
      head.y++;
      break;
    case LEFT:
      head.x--;
      break;
    case RIGHT:
      head.x++;
      break;
  }

  if (head.x == COLS || head.x < 1 || head.y == ROWS || head.y < 1 ||
      snaketouchingitself()) {
    gameover = 1;
  }

  if (snaketouchingapple()) {
    score++;
    makeapple();
    snake_length++;
    snake = realloc(snake, (snake_length + 1) * sizeof *snake);
  } else {
    remove_tail();
    memmove(snake, snake + 1, snake_length * sizeof *snake);
  }

  snake[snake_length] = head;
}

int snaketouchingapple() {
  if (snake[snake_length].x == apple.x && snake[snake_length].y == apple.y) {
    return 1;
  }
  return 0;
}

int snaketouchingitself() {
  for (int i = 0; i < snake_length; i++) {
    if (snake[snake_length].x == snake[i].x &&
        snake[snake_length].y == snake[i].y) {
      return 1;
    }
  }

  return 0;
}

void printgame() {
  for (int i = 0; i < snake_length; i++) {
    attron(COLOR_PAIR(1));
    mvprintw(snake[i].y + yoffset, snake[i].x * 2 + xoffset, "ss");
    attroff(COLOR_PAIR(1));
  }

  attron(COLOR_PAIR(2));
  mvprintw(apple.y + yoffset, apple.x * 2 + xoffset, "aa");
  attroff(COLOR_PAIR(2));

  mvprintw(yoffset, xoffset + 3, "Score: %d", score);
}

void printgameover() {
  mvprintw(screen_rows / 2, screen_cols / 2 - 5, "Game Over!");
  mvprintw(screen_rows / 2 + 1, screen_cols / 2 - 15,
           "Press 'n' to start a new game!");
}

void makeapple() {
  int touchingsnake = 0;

  do {
    touchingsnake = 0;
    apple.y = rand() % (ROWS - 1) + 1;
    apple.x = rand() % (COLS - 1) + 1;

    for (int i = 0; i < snake_length; i++) {
      if (apple.x == snake[i].x && apple.y == snake[i].y) touchingsnake = 1;
    }

  } while (touchingsnake);
}

void birthsnake() {
  snake_length = 3;
  snake = malloc((snake_length + 1) * sizeof *snake);

  for (int i = 0; i <= snake_length; i++) {
    snake[i].y = COLS / 2;
    snake[i].x = 2 + i;
  }
}

void newgame() {
  clear();

  // Draw the border
  mvhline(yoffset, xoffset, '-', COLS * 2);
  mvhline(yoffset + ROWS, xoffset, '-', COLS * 2);
  mvvline(yoffset, xoffset, '|', ROWS);
  mvvline(yoffset, xoffset + (COLS * 2), '|', ROWS);
  mvaddch(yoffset, xoffset, '+');
  mvaddch(yoffset, xoffset + (COLS * 2), '+');
  mvaddch(yoffset + ROWS, xoffset, '+');
  mvaddch(yoffset + ROWS, xoffset + (COLS * 2), '+');

  snake_direction = RIGHT;
  score = 0;
  gameover = 0;
  pause = 0;
  birthsnake();
  makeapple();
}
