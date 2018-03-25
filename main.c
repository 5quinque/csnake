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
void printgameover();
void birthsnake();
void makeapple();
void handleinput(int c);
void change_direction(int d);
void remove_tail();
int snaketouchingitself();

int snake_length;
struct point *snake;
struct point apple;

int snake_direction = RIGHT;
int pause = 0;
int gameover = 0;
int running = 1;
int screen_rows, screen_cols, game_columns;

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
  init_pair(2, COLOR_GREEN, COLOR_GREEN);
  // Food
  init_pair(1, COLOR_RED, COLOR_RED);
  // Background
  init_pair(3, COLOR_BLACK, COLOR_BLACK);

  // Stop getch() from blocking
  nodelay(stdscr, TRUE);

  getmaxyx(stdscr, screen_rows, screen_cols);
  game_columns = screen_cols / 2;

  birthsnake();
  makeapple();

  while (running) {
    c = getch();
    if (~c >> 31) handleinput(c);

    if (!pause && !gameover) update();

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
  if (snake_direction == direction) return;
  if (snake_direction == UP && direction == DOWN) return;
  if (snake_direction == DOWN && direction == UP) return;
  if (snake_direction == LEFT && direction == RIGHT) return;
  if (snake_direction == RIGHT && direction == LEFT) return;

  snake_direction = direction;
}

void remove_tail() {
  struct point tail = snake[0];
  mvprintw(0, 5, "y: %d\tx: %d\t length: %d\t", tail.y, tail.x, snake_length);

  attron(COLOR_PAIR(3));
  mvaddch(tail.y, tail.x * 2, '.');
  mvaddch(tail.y, tail.x * 2 + 1, '.');
  attroff(COLOR_PAIR(3));
}

void update() {
  struct point head = snake[snake_length];

  remove_tail();

  switch (snake_direction) {
    case UP:
      head.y -= 1;
      break;
    case DOWN:
      head.y += 1;
      break;
    case LEFT:
      head.x -= 1;
      break;
    case RIGHT:
      head.x += 1;
      break;
  }

  if (head.x > game_columns || head.x < 0 || head.y > screen_rows ||
      head.y < 0) {
    gameover = 1;
    printgameover();
  }

  // works -
  // snake_length += 1;
  // snake = realloc(snake, snake_length * sizeof *snake);
  //--

  memmove(snake, snake + 1, snake_length * sizeof *snake);
  snake[snake_length] = head;

  if (snaketouchingitself()) {
    gameover = 1;
    printgameover();
  }
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
    attron(COLOR_PAIR(2));
    mvprintw(snake[i].y, snake[i].x * 2, "ss");
    attroff(COLOR_PAIR(2));
  }

  attron(COLOR_PAIR(1));
  mvprintw(apple.y, apple.x * 2, "aa");
  attroff(COLOR_PAIR(1));

  if (pause) mvprintw(1, screen_cols / 2 - 3, "Paused");
}

void printgameover() {
  mvprintw(screen_rows / 2 - 1, screen_cols / 2 - 5, "Game Over!");
  mvprintw(screen_rows / 2, screen_cols / 2 - 5, "Game Over!");
  mvprintw(screen_rows / 2 + 1, screen_cols / 2 - 5, "Game Over!");
}

void makeapple() {
  apple.y = rand() % screen_rows;
  apple.x = rand() % game_columns;
}

void birthsnake() {
  snake_length = 15;
  snake = malloc((snake_length + 1) * sizeof *snake);

  for (int i = 0; i <= snake_length; i++) {
    snake[i].y = 10;
    snake[i].x = 21 + i;
  }
}
