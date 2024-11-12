#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "task.h"
#include "client.h"

#define UP_ARROW 259
#define DOWN_ARROW 258
#define BUFFER_MAX_SIZE 256

typedef enum State {
  QUIT,
  VIEW,
} State;

void process_cmd(Tasks *tasks, int selected, const char *cmd) {
  if (strcmp(cmd, "complete") == 0) {
    tasks->items[selected]->completed = true;
  }
  if (strcmp(cmd, "uncomplete") == 0) {
    tasks->items[selected]->completed = false;
  }
  if (strcmp(cmd, "delete") == 0) {
    task_remove(tasks, selected);
  }
}

int main(void) {
  Tasks tasks = {0};
  State state = VIEW;
  int selected = 0;
  initscr();
  raw();
  keypad(stdscr, true);
  cbreak();
  start_color();
  init_pair(HIGHLIGHTED, COLOR_BLACK, COLOR_WHITE);
  
  refresh();
  noecho();
  while (state != QUIT)
  {
    clear();
    move(0,0);
    switch(state) {
      case VIEW:
        curs_set(0);
        client_display_tasks(&tasks, selected);
        client_display_options(selected - tasks.count);
        int input = getch();
        curs_set(1);
        switch (input) {
          case UP_ARROW:
            if (selected > 0) {
              selected--;
            }
            break;
          case DOWN_ARROW:
            if (selected < tasks.count + 1) {
              selected++;
            }
            break;
          case '\n':
            if (selected < tasks.count) {
              char *cmd = client_prompt_cmd();
	      process_cmd(&tasks, selected, cmd);
	      free(cmd);
            }
            else if (selected == tasks.count) {
              Task *t = client_prompt_task();
	      task_append(&tasks, t);
            }
            else if (selected == tasks.count+1) {
              state = QUIT;
            }
            break;
          case 'C':
            if (selected < tasks.count) {
              tasks.items[selected]->completed = true;
            }
            break;

        }
        break;
    default:
      assert(0 && "Invalid state reached");
        break;
    }
    refresh();
  }
  endwin();
  return 0;
}
