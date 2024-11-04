#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define UP_ARROW 259
#define DOWN_ARROW 258
#define BUFFER_MAX_SIZE 256
#define MAX_TASKS 10

typedef enum Color {
    DEFAULT,
    HIGHLIGHTED,
} Color;

typedef enum State {
    QUIT,
    VIEW,
} State;

typedef struct Task
{
    char* title;
    char* description;
    bool completed;
} Task; 

Task tasks[MAX_TASKS] = {0}; // Global Task array
int task_count; // Number of tasks in the global array
State state; // Global state
int selected = 0; // Selected task
size_t rows, cols; // Size of the terminal

/* Display a message to the user, closes on input */
void display(const char* message) {
    printw("%s", message);
    getch();
} 

/* Prints a string to the screen as the provided color */
void print_color(const char* string, Color color) {
    attron(COLOR_PAIR(color));
        printw("%s", string);
    attroff(COLOR_PAIR(color));
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

/* Removes a task from the global tasks array */
void remove_task(int i) {
    Task* t = &tasks[i];
    free(t->title);
    free(t->description);
    for(int j = i; j < MAX_TASKS; j++) {
        tasks[j] = tasks[j+1];
    }
    task_count--;
}

/* Creates a window to prompt for the title and description of a new task */
void create_task(void) {
    Task t = {0};
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

    t.title = strdup(title_buf);
    t.description = strdup(desc_buf);
    t.completed = false;
    append_task(t);

    delwin(win);
}

/* Handles commands input by the user */
void cmd(void) {
    char buf[BUFFER_MAX_SIZE] = {0};
    WINDOW *win;

    win = newwin(1, cols, rows-1, 0);

    echo();
        wprintw(win, "> ");
        wgetnstr(win, buf, BUFFER_MAX_SIZE-1);
    noecho();

    delwin(win);

    if (strcmp(buf, "complete") == 0) {
        tasks[selected].completed = true;
    }
    if (strcmp(buf, "uncomplete") == 0) {
        tasks[selected].completed = false;
    }
    if (strcmp(buf, "delete") == 0) {
        remove_task(selected);
    }
}

/* Lists all the tasks, as well as a few options; highlights the selected option */
void list_tasks(void) {
    char* title_buf = malloc(cols+2);
    for(int i = 0; i < task_count; i++) {
        Task* t = &tasks[i];
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
        print_color(title_buf, title_color);
        if (i == selected) {
            printw("\t");
            print_color(t->description, DEFAULT);
            printw("\n");
        }                                   
    }
    free(title_buf);
    move(rows-2, 0);
    print_color("[+] New\n", (selected == task_count ? HIGHLIGHTED : DEFAULT));
    print_color("[<] Quit\n", (selected == task_count + 1 ? HIGHLIGHTED : DEFAULT));
}

int main(void) {
    state = VIEW;
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
        getmaxyx(stdscr, rows, cols);
        switch(state) {
            case VIEW:
                curs_set(0);
                list_tasks();
                int input = getch();
                curs_set(1);
                switch (input) {
                    case UP_ARROW:
                        if (selected > 0) {
                            selected--;
                        }
                        break;
                    case DOWN_ARROW:
                        if (selected < task_count + 1) {
                            selected++;
                        }
                        break;
                    case '\n':
                        if (selected < task_count) {
                            cmd();
                        }
                        else if (selected == task_count) {
                            create_task();
                        }
                        else if (selected == task_count+1) {
                            state = QUIT;
                        }
                        break;
                    case 'C':
                        if (selected < task_count) {
                            tasks[selected].completed = true;
                        }
                        break;

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