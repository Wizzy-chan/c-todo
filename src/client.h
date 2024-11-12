#ifndef UI_H__
#define UI_H__
#include <stdlib.h>
#include <ncurses.h>
#include "task.h"

typedef enum Color {
  DEFAULT,
  HIGHLIGHTED,
} Color;

#endif // UI_H__

void client_print_color(const char *string, Color color);
void client_print_highlight(const char *string, bool highlight);
void client_display_tasks(Tasks *tasks, int selected);
void client_display_options(int selected);
Task *client_prompt_task();
char *client_prompt_cmd();
