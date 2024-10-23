#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define BACKSPACE 127

typedef struct Task
{
    char* title;
    char* description;
    bool completed;
} Task; 

Task tasks[10] = {0};
int task_count;
bool running;

void print_task(Task* task) {
    printw("Task:\n\tTitle: %s\n\tDescription: %s\n\tCompleted: %d\n", task->title, task->description, task->completed);
}

void append_task(Task t) {
    if (task_count == 10) {
        printw("Error: Maximum task count reached");
        return;
    } 
    tasks[task_count] = t;
    task_count++;

}

void create_task() {
    char buffer[256] = {0};
    int buf_size = 0;
    int step = 0;
    Task t = {0};
    t.completed = false;
    char confirmation[256];
    while (step < 3) {
        clear();
        switch(step) {
            case 0:
                printw("Title: %s", buffer);
                break;
            case 1:
                printw("Description: %s", buffer);
                break;
            case 2:
                printw("Confirm? (y/n): %s", buffer);
        }
        int input = getch();
        switch (input) {
            case BACKSPACE:
                if (buf_size > 0) {
                    buf_size--;
                    buffer[buf_size] = 0;
                }
                break;
            
            case '\n':
                switch (step) {
                    case 0:
                        t.title = strdup(buffer);
                        break;
                    case 1:
                        t.description = strdup(buffer);
                        break;
                    case 2:
                        strncpy(confirmation, buffer, buf_size+1);
                        break;
                }
                memset(buffer, 0, 255);
                buf_size = 0;
                step++;
                break;

            default:
                if (buf_size < 255) {
                    buffer[buf_size] = input;
                    buf_size++;
                }
                break;
        }
        refresh();
    }
    if (strcmp(confirmation, "y") == 0) {
        append_task(t);
    }
}

void process_string(char* buffer) {
    if (strcmp(buffer, "exit") == 0) {
        running = false;
    }
    if (strcmp(buffer, "create") == 0) {
        create_task();
    }
}


int main(int argc, char* argv[]) {
    char buffer[256] = {0};
    int buf_size = 0;

    running = true;
    initscr();
    raw();
    keypad(stdscr, true);
    cbreak();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    refresh();
    noecho();
    while (running)
    {
        clear();
        move(0,0);
        for(int i = 0; i < task_count; i++) {
            print_task(&tasks[i]);
        }
        move(rows-1, 0);
        printw("> %s", buffer);
        int input = getch();
        switch (input)
        {
            case BACKSPACE:
                if (buf_size > 0) {
                    buf_size--;
                    buffer[buf_size] = 0;
                }
                break;

            case '\n':
                process_string(buffer);
                memset(buffer, 0, 256);
                buf_size = 0;
                break;
            
            default:
                if (buf_size < 255) {
                    buffer[buf_size] = input;
                    buf_size++;
                }
                break;
        }
        refresh();
    }
    endwin();
    return 0;
}