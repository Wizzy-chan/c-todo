#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define BACKSPACE 127
#define UP_ARROW 259
#define DOWN_ARROW 258
#define BUFFER_MAX_SIZE 256
#define MAX_TASKS 10

typedef enum ColorPair {
    DEFAULT,
    HIGHLIGHTED,
} ColorPair;

typedef enum State {
    QUIT,
    VIEW,
    CREATE_TITLE,
    CREATE_DESCRIPTION,
    COMMAND,
} State;

typedef struct Task
{
    char* title;
    char* description;
    bool completed;
    bool expanded;
} Task; 

Task tasks[MAX_TASKS] = {0}; // Global Task array
int task_count; // Number of tasks in the global array
State state; // Global state
int selected = 0; // Selected task

/* Display a message to the user, closes on input and restores to main view state */
void display(const char* message) {
    printw("%s", message);
    getch();
    state = VIEW;
} 

void print_color(const char* string, ColorPair color) {
    attron(COLOR_PAIR(color));
    printw("%s", string);
    attroff(COLOR_PAIR(color));
}

/* Prints a task to stdscr */
void print_task(Task* task, ColorPair title_color, ColorPair desc_color) {
    if (task->completed) {
        print_color("[-] ", title_color);
    } else {
        print_color("[ ] ", title_color);
    }
    print_color(task->title, title_color);
    printw("\n");
    if (task->expanded) {
        char* description = task->description;
        if (strcmp(description, "") == 0) {
            description = "[No description]";
        }
        print_color("\t", desc_color);
        print_color(description, desc_color);
        print_color("\n", desc_color);
    }
}

/* Creates an input prompt for the user, returns true when the input is finished (user presses enter) */
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
            if (*buf_size < BUFFER_MAX_SIZE-1 && input < 256) {
                buf[*buf_size] = input;
                (*buf_size)++;
            }
            break;
    }
    return false;
}

/* Adds a task to the global tasks array */
void append_task(Task t) {
    if (task_count == MAX_TASKS) {
        printw("Error: Maximum task count reached");
        return;
    } 
    tasks[task_count] = t;
    task_count++;

}

/* Handles commands input by the user */
void cmd(char* buffer) {
    if (strcmp(buffer, "exit") == 0) {
        state = QUIT;
    }
    if (strcmp(buffer, "create") == 0) {
        state = CREATE_TITLE;
    }
    if (strcmp(buffer, "complete") == 0) {
        if (selected < task_count) {
            tasks[selected].completed = true;
        }
        state = VIEW;
    }
    if (strcmp(buffer, "expand") == 0) {
        if (selected < task_count) {
            tasks[selected].expanded ^= true;
        }
        state = VIEW;
    }
}

void list_tasks(void) {
    for(int i = 0; i < task_count; i++) {
        ColorPair title_color = (i == selected ? HIGHLIGHTED : DEFAULT);
        print_task(&tasks[i], title_color, DEFAULT);
    }
    print_color("+", (selected == task_count ? HIGHLIGHTED : DEFAULT));
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
    start_color();
    init_pair(HIGHLIGHTED, COLOR_BLACK, COLOR_WHITE);

    int rows = getmaxy(stdscr);
    refresh();
    noecho();
    while (state != QUIT)
    {
        clear();
        move(0,0);
        switch(state) {
            case VIEW:
                curs_set(0);
                list_tasks();
                int input = getch();
                curs_set(1);
                switch (input) {
                    case UP_ARROW:
                        if (selected > 0) { selected--; }
                        break;
                    case DOWN_ARROW:
                        if (selected < task_count) { selected++; }
                        break;
                    case '\n':
                        state = COMMAND;
                        break;
                }
                break;
            case CREATE_TITLE: // Prompt for task title during task creation
                printw("Title: ");
                submitted = prompt(buffer, &buf_size);
                if (submitted) {
                    task.title = strdup(buffer);
                    memset(buffer, 0, 256);
                    buf_size = 0;
                    state = CREATE_DESCRIPTION;
                }
                break;
            case CREATE_DESCRIPTION: // Prompt for task description during task creation
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
            case COMMAND:
                move(0, 0);
                list_tasks();
                move(rows-1, 0);
                printw("> ");
                submitted = prompt(buffer, &buf_size);
                if (submitted) {
                    cmd(buffer);
                    memset(buffer, 0, 256);
                    buf_size = 0;
                }
                break;
            default:
                display("ERROR: INVALID STATE REACHED.");
                break;
        }
        refresh();
    }
    endwin();
    return 0;
}