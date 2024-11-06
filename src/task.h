#include <stdbool.h>

typedef struct Task {
    char* title;
    char* description;
    bool completed;
} Task; 

typedef struct Tasks {
    Task* items;
    int count;
    int size;
} Tasks;

#define MIN_ARRAY_SIZE 5

Task create_task(const char* title, const char* description);
void append_task(Tasks *tasks, Task t);
void remove_task(Tasks *tasks, int i);