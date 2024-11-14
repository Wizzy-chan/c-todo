#ifndef TASK_H__
#define TASK_H__
#include <stdbool.h>

typedef struct Task {
  char *title;
  char *description;
  bool completed;
} Task; 

typedef struct Tasks {
  Task **items;
  int count;
  int size;
} Tasks;

#define MIN_ARRAY_SIZE 5
#endif // TASK_H__

Task *task_create(const char *title, const char *description);
void task_append(Tasks *tasks, Task *t);
void task_remove(Tasks *tasks, int i);
void task_free(Task *t);
void task_write_file(Tasks* tasks, const char *filepath);
