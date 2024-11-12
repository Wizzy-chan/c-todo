#include <ncurses.h>
#include <string.h>
#include "task.h"
#include "client.h"

#define BUFFER_MAX_SIZE 256

/* Print a string to stdscr with the provided color */
void client_print_color(const char *string, Color color) {
  attron(COLOR_PAIR(color));
    printw("%s", string);
  attroff(COLOR_PAIR(color));
}

/* Prints a string to stdscr, uses the DEFAULT color if highlight is false, and the HIGHLIGHT color if highlight is true */
void client_print_highlight(const char *string, bool highlight) {
  if (highlight) {
    attron(COLOR_PAIR(HIGHLIGHTED));
  }
  printw("%s", string);
  attroff(COLOR_PAIR(HIGHLIGHTED));
}

/* Prints a list of all the tasks in the provided Tasks array to stdscr. Highlights the task with the selected index */
void client_display_tasks(Tasks *tasks, int selected) {
  size_t cols = getmaxx(stdscr);
  char *title_buf = malloc(cols+2);
  for(int i = 0; i < tasks->count; i++) {
    Task* t = tasks->items[i];
    Color title_color;
    if (selected == i) { 
      title_color = HIGHLIGHTED;
    } else {
      title_color = DEFAULT;
    }
    memset(title_buf, 0, cols+2);
    if (t->completed) {
      strcat(title_buf, "[=] ");
    } else {
      strcat(title_buf, "[ ] ");
    }
    if (strlen(t->title) > cols-5) {
      strncat(title_buf, t->title, cols-8);
      strcat(title_buf, "...");
    } else {
      strcat(title_buf, t->title);
    }
    strcat(title_buf, "\n");
    client_print_color(title_buf, title_color);
    if (i == selected) {
      printw("\t");
      client_print_color(t->description, DEFAULT);
      printw("\n");
    }                                   
  }
  free(title_buf);
}

/* Prints a list of options to stdscr. Highlights the selected option */
void client_display_options(int selected) {
  size_t rows = getmaxy(stdscr);
  move(rows-2, 0);
  client_print_highlight("[+] New\n", selected == 0);
  client_print_highlight("[<] Quit\n", selected == 1);
}

/* Creates a window containing a prompt to create a new task. Returns a pointer to a new task with the input title and description */
Task *client_prompt_task() {
  int rows, cols;

  getmaxyx(stdscr, rows, cols);

  WINDOW *win;
  char title_buf[BUFFER_MAX_SIZE] = {0};
  char desc_buf[BUFFER_MAX_SIZE] = {0};
  win = newwin(2, cols, rows-2, 0);

  echo();
    wprintw(win, "Title: ");
    wgetnstr(win, title_buf, BUFFER_MAX_SIZE-1);
    wprintw(win, "Description: ");
    wgetnstr(win, desc_buf, BUFFER_MAX_SIZE-1);
  noecho();

  delwin(win);

  Task *t = task_create(title_buf, desc_buf);
  return t;
}

/* Creates a window with a prompt for the user to enter a command, returns a pointer to a null terminated string with the user's input */
char *client_prompt_cmd() {
  int rows, cols;

  getmaxyx(stdscr, rows, cols);

  WINDOW *win;
  char buf[BUFFER_MAX_SIZE] = {0};
  win = newwin(1, cols, rows-1, 0);
  
  echo();
    wprintw(win, "> ");
    wgetnstr(win, buf, BUFFER_MAX_SIZE-1);
  noecho();
  
  return strdup(buf);
}
