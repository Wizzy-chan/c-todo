#include "task.h"
#include <string.h>
#include <stdlib.h>

/* Create a Task object */
Task create_task(const char* title, const char* description) {
    Task t = {0};
    t.title = strdup(title);
    t.description = strdup(description);
    return t;
}

/* Append a Task to a Tasks array */
void append_task(Tasks *tasks, Task t) {
    if (tasks->count == tasks->size) {
        if (tasks->size == 0) {
            tasks->size = MIN_ARRAY_SIZE;
        } else {
            tasks->size *= 2;
        }
        tasks->items = realloc(tasks->items, tasks->size * sizeof(Task));
    } 
    tasks->items[tasks->count] = t;
    tasks->count++;
}

/* Removes a Task from a Tasks array by index */
void remove_task(Tasks *tasks, int i) {
    Task* t = &tasks->items[i];
    free(t->title);
    free(t->description);
    tasks->count--;
    memmove(&tasks->items[i], &tasks->items[i+1], (tasks->count - i) * sizeof(Task));
}
