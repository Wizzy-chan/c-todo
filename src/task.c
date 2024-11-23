#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "task.h"
#include "sb.h"
#include "file.h"

/* Create a new task object with the given title and description, call task_free() when finished with the Task to free string pointers */
Task *task_create(const char *title, const char *description) {
  Task *t = malloc(sizeof *t);
  t->title = strdup(title);
  t->description = strdup(description);
  t->completed = false;
  return t;
}

/* Append a task to the end of a Tasks array */
void task_append(Tasks *tasks, Task *t) {
  if (tasks->count == tasks->size) {
    if (tasks->size == 0) {
      tasks->size = MIN_ARRAY_SIZE;
    } else {
      tasks->size *= 2;
    }
    tasks->items = realloc(tasks->items, tasks->size * sizeof(Task*));
  } 
  tasks->items[tasks->count] = t;
  tasks->count++;
}

/* Remove a task from a Tasks array by index */
void task_remove(Tasks *tasks, int i) {
  Task* t = tasks->items[i];
  task_free(t);
  tasks->count--;
  memmove(&tasks->items[i], &tasks->items[i+1], (tasks->count - i) * sizeof(Task));
}

/* Frees a task pointer and its string pointers */
void task_free(Task *t) {
  free(t->title);
  free(t->description);
  free(t);
}

/* Write all of the tasks in a Tasks array into a file */
void task_write_file(Tasks* tasks, const char *filepath) {
  String_Builder sb = {0};
  for (int i = 0; i < tasks->count; i++) {
    sb_append_cstr(&sb, tasks->items[i]->title);
    sb_append(&sb, ',');
    sb_append_cstr(&sb, tasks->items[i]->description);
    sb_append(&sb, ',');
    if (tasks->items[i]->completed) {
      sb_append_cstr(&sb, "true");
    } else {
      sb_append_cstr(&sb, "false");
    }
    sb_append(&sb, '\n');
  }
  sb_append_null(&sb);
  file_write_all(filepath, sb.string);
  free(sb.string); // We are done with this String_Builder, but the dynamic array is not freed automatically
}

void task_read_file(Tasks *tasks, const char *filepath) {
  char *content = file_read_all(filepath);
  if (content == NULL) {
    return;
  }
  String_View view = sv_from_str(content);
  while (view.count > 0) {
    String_View title_view = sv_slice_left(&view, ',');
    String_View description_view = sv_slice_left(&view, ',');
    String_View completed_view = sv_slice_left(&view, '\n');
    // TODO: Allocate less?
    // task_create() already does a strdup() so maybe we need to
    // reuse the same allocations or change how task_create works
    // maybe sv_to_str could accept a buffer to write to instead
    // of mallocing a new string to write to
    char *title = sv_to_str(&title_view);
    char *description = sv_to_str(&description_view);
    char *completed_str = sv_to_str(&completed_view);
    Task *t = task_create(title, description);
    if (strcmp(completed_str, "true") == 0) {
      t->completed = true;
    }
    task_append(tasks, t);
    free(title);
    free(description);
    free(completed_str);
  }
  free(content);
}
