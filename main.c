#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define BACKSPACE 127
#define BUFFER_MAX_SIZE 256

typedef enum State {
    QUIT,
    VIEW,
    CREATE_TITLE,
    CREATE_DESCRIPTION,
} State;

typedef struct Task
{
    char* title;
    char* description;
    bool completed;
    bool expanded;
} Task; 

Task tasks[10] = {0};
int task_count;
State state;

void print_task(Task* task) {
    if (task->completed) {
        printw("[-] ");
    } else {
        printw("[ ] ");
    }
    printw("%s\n", task->title);
    if (task->expanded) {
        printw("\t%s\n", task->description);
    }
}

bool prompt(char* buf, int* buf_size) {
    printw("%s", buf);
    int input = getch();
    switch (input)
    {
        case BACKSPACE:
            if (*buf_size > 0) {
                (*buf_size)--;
                buf[*buf_size] = 0;
            }
            break;

        case '\n':
            return true;
        
        default:
            if (*buf_size < BUFFER_MAX_SIZE-1) {
                buf[*buf_size] = input;
                (*buf_size)++;
            }
            break;
    }
    return false;
}

void append_task(Task t) {
    if (task_count == 10) {
        printw("Error: Maximum task count reached");
        return;
    } 
    tasks[task_count] = t;
    task_count++;

}

void cmd(char* buffer) {
    if (strcmp(buffer, "exit") == 0) {
        state = QUIT;
    }
    if (strcmp(buffer, "create") == 0) {
        state = CREATE_TITLE;
    }
    if (strncmp(buffer, "complete ", 9) == 0) {
        // TODO: replace atoi with an error checking conversion
        int index = atoi(buffer+9);
        if (index < task_count) {
            tasks[index].completed = true;
        }
    }
    if (strncmp(buffer, "expand ", 7) == 0) {
        // TODO: replace atoi with an error checking conversion
        int index = atoi(buffer+7);
        if (index < task_count) {
            tasks[index].expanded ^= true;
        }
    }
}

int main(void) {
    char buffer[BUFFER_MAX_SIZE] = {0};
    int buf_size = 0;
    Task task = {0};
    bool submitted;

    state = VIEW;
    initscr();
    raw();
    keypad(stdscr, true);
    cbreak();

    int rows = getmaxy(stdscr);
    refresh();
    noecho();
    while (state != QUIT)
    {
        clear();
        switch(state) {
            case VIEW:
                move(0,0);
                for(int i = 0; i < task_count; i++) {
                    print_task(&tasks[i]);
                }
                move(rows-1, 0);
                printw("> ");
                submitted = prompt(buffer, &buf_size);
                if (submitted) {
                    cmd(buffer);
                    memset(buffer, 0, 256);
                    buf_size = 0;
                }
                break;
            case CREATE_TITLE:
                move(0,0);
                printw("Title: ");
                submitted = prompt(buffer, &buf_size);
                if (submitted) {
                    task.title = strdup(buffer);
                    memset(buffer, 0, 256);
                    buf_size = 0;
                    state = CREATE_DESCRIPTION;
                }
                break;
            case CREATE_DESCRIPTION:
                move(0,0);
                printw("Description: ");
                submitted = prompt(buffer, &buf_size);
                if (submitted) {
                    task.description = strdup(buffer);
                    memset(buffer, 0, 256);
                    buf_size = 0;
                    append_task(task);
                    state = VIEW;
                }
                break;
            default:
                break;
        }
        refresh();
    }
    endwin();
    return 0;
}