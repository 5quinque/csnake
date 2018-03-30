#include <ncurses.h>
// memmove()
#include <string.h>
// time(), nanosleep()
#include <time.h>
// srand(), rand(), malloc()
#include <stdlib.h>

#define ROWS 20
#define COLS 20

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

void update();
void print_game();
void print_gameover();
void birth_snake();
void make_apple();
void new_game();
void handle_input(int c);
void change_direction(int d);
void remove_tail();
void pop_direction_key();
void handle_direction_key(int d);
int snake_touching_itself();
int snake_touching_apple();

int snake_length;
struct point *snake;
struct point apple;
int snake_direction;
int paused = 0;
int gameover = 0;
int running = 1;
int score;
int screen_rows, screen_cols;
int screen_center_row, screen_center_col, yoffset, xoffset;
int direction_count = 0;
int *direction_queue;

struct point {
  int y;
  int x;
};

int main() {
  struct timespec ts = {0, 120000000L};
  int c;

  direction_queue = malloc(sizeof(int));
  direction_queue[0] = RIGHT;

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

  new_game();

  while (running) {
    while ((c = getch()) != ERR)
      handle_input(c);
    pop_direction_key();

    if (!paused && !gameover) {
      update();
      print_game();
    }

    if (gameover)
      print_gameover();

    refresh();
    nanosleep(&ts, NULL);
  }

  // Clean up after ourselves
  endwin();

  return 0;
}

void handle_input(int c) {
  switch (c) {
    case 'w':
      handle_direction_key(UP);
      break;
    case 'a':
      handle_direction_key(LEFT);
      break;
    case 's':
      handle_direction_key(DOWN);
      break;
    case 'd':
      handle_direction_key(RIGHT);
      break;
    case ' ':
      // toggle paused
      paused ^= 1;
      if (paused)
        mvprintw(yoffset, screen_cols / 2 - 3, "Paused");
      else
        mvprintw(yoffset, screen_cols / 2 - 3, "------");
      break;
    case 'q':
      running = 0;
      break;
    case 'n':
      new_game();
  }
}

void handle_direction_key(int direction) {
  if (direction_count > 1 && direction_queue[direction_count - 1] == direction)
    return;

  direction_queue =
      realloc(direction_queue, (direction_count + 1) * sizeof(int));

  direction_queue[direction_count] = direction;
  direction_count++;
}

void pop_direction_key() {
  if (direction_count < 1) return;
  change_direction(direction_queue[0]);
  memmove(direction_queue, direction_queue + 1, direction_count * sizeof(int));

  direction_queue =
      realloc(direction_queue, direction_count * sizeof(int));
  direction_count--;
}

void change_direction(int direction) {
  if (paused) return;
  if (snake_direction == direction) return;
  if (snake_direction == UP && direction == DOWN) return;
  if (snake_direction == DOWN && direction == UP) return;
  if (snake_direction == LEFT && direction == RIGHT) return;
  if (snake_direction == RIGHT && direction == LEFT) return;

  snake_direction = direction;
}

void remove_tail() {
  struct point tail = snake[0];

  attron(COLOR_PAIR(3));
  mvprintw(tail.y + yoffset, tail.x * 2 + xoffset, "  ");
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

  if (head.x == COLS || head.x == 0 || head.y == ROWS || head.y == 0 ||
      snake_touching_itself())
    gameover = 1;

  if (snake_touching_apple()) {
    score++;
    make_apple();
    snake_length++;
    snake = realloc(snake, (snake_length + 1) * sizeof *snake);
  } else {
    remove_tail();
    memmove(snake, snake + 1, snake_length * sizeof *snake);
  }

  snake[snake_length] = head;
}

int snake_touching_apple() {
  return (snake[snake_length].x == apple.x && snake[snake_length].y == apple.y);
}

int snake_touching_itself() {
  for (int i = 0; i < snake_length; i++) {
    if (snake[snake_length].x == snake[i].x &&
        snake[snake_length].y == snake[i].y) {
      return 1;
    }
  }

  return 0;
}

void print_game() {
  for (int i = 0; i < snake_length; i++) {
    attron(COLOR_PAIR(1));
    mvprintw(snake[i].y + yoffset, snake[i].x * 2 + xoffset, "  ");
    attroff(COLOR_PAIR(1));
  }

  attron(COLOR_PAIR(2));
  mvprintw(apple.y + yoffset, apple.x * 2 + xoffset, "  ");
  attroff(COLOR_PAIR(2));

  mvprintw(yoffset, xoffset + 3, "Score: %d", score);
}

void print_gameover() {
  mvprintw(screen_rows / 2, screen_cols / 2 - 5, "Game Over!");
  mvprintw(screen_rows / 2 + 1, screen_cols / 2 - 15,
           "Press 'n' to start a new game!");
}

void make_apple() {
  int touchingsnake = 0;

  do {
    touchingsnake = 0;
    apple.y = rand() % (ROWS - 1) + 1;
    apple.x = rand() % (COLS - 1) + 1;

    for (int i = 0; i < snake_length; i++)
      if (apple.x == snake[i].x && apple.y == snake[i].y)
        touchingsnake = 1;
  } while (touchingsnake);
}

void birth_snake() {
  free(snake);

  snake_length = 3;
  snake = malloc((snake_length + 1) * sizeof *snake);

  for (int i = 0; i <= snake_length; i++) {
    snake[i].y = COLS / 2;
    snake[i].x = 2 + i;
  }
}

void new_game() {
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

  direction_count = 0;
  direction_queue = realloc(direction_queue, sizeof(int));
  direction_queue[0] = RIGHT;

  snake_direction = RIGHT;
  score = 0;
  gameover = 0;
  paused = 0;
  birth_snake();
  make_apple();
}
