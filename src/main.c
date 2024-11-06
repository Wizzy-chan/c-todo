#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "task.h"

#define UP_ARROW 259
#define DOWN_ARROW 258
#define BUFFER_MAX_SIZE 256

typedef enum Color {
    DEFAULT,
    HIGHLIGHTED,
} Color;

typedef enum State {
    QUIT,
    VIEW,
} State;

typedef struct Term {
    size_t rows, cols;
} Term;

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

/* Creates a window to prompt for the title and description of a new task and then adds it to a Tasks array */
void cmd_task(Tasks *tasks, Term *term) {
    WINDOW *win;
    char title_buf[BUFFER_MAX_SIZE] = {0};
    char desc_buf[BUFFER_MAX_SIZE] = {0};
    win = newwin(2, term->cols, term->rows-2, 0);

    echo();
        wprintw(win, "Title: ");
        wgetnstr(win, title_buf, BUFFER_MAX_SIZE-1);
        wprintw(win, "Description: ");
        wgetnstr(win, desc_buf, BUFFER_MAX_SIZE-1);
    noecho();

    Task t = create_task(title_buf, desc_buf);
    append_task(tasks, t);

    delwin(win);
}

/* Prompts the user to input a command, and then executes it on the selected Task */
void cmd(Tasks *tasks, int selected, Term *term) {
    char buf[BUFFER_MAX_SIZE] = {0};
    WINDOW *win;

    win = newwin(1, term->cols, term->rows-1, 0);

    echo();
        wprintw(win, "> ");
        wgetnstr(win, buf, BUFFER_MAX_SIZE-1);
    noecho();

    delwin(win);

    if (strcmp(buf, "complete") == 0) {
        tasks->items[selected].completed = true;
    }
    if (strcmp(buf, "uncomplete") == 0) {
        tasks->items[selected].completed = false;
    }
    if (strcmp(buf, "delete") == 0) {
        remove_task(tasks, selected);
    }
}

/* Lists all tasks into a given window, using the size of the terminal as reference;
   Highlights the selected option */
void list_tasks(Tasks *tasks, int selected, WINDOW *win, Term *term) {
    char* title_buf = malloc(term->cols+2);
    for(int i = 0; i < tasks->count; i++) {
        Task* t = &tasks->items[i];
        Color title_color;
        if (selected == i) { 
            title_color = HIGHLIGHTED;
        } else {
            title_color = DEFAULT;
        }
        memset(title_buf, 0, win->_maxy+2);
        if (t->completed) {
            strcat(title_buf, "[=] ");
        } else {
            strcat(title_buf, "[ ] ");
        }
        if (strlen(t->title) > term->cols-5) {
            strncat(title_buf, t->title, term->cols-8);
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
    wmove(win, term->rows-2, 0);
    print_color("[+] New\n", (selected == tasks->count ? HIGHLIGHTED : DEFAULT));
    print_color("[<] Quit\n", (selected == tasks->count + 1 ? HIGHLIGHTED : DEFAULT));
}

int main(void) {
    Tasks tasks = {0};
    Term term = {0};
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
        getmaxyx(stdscr, term.rows, term.cols);
        switch(state) {
            case VIEW:
                curs_set(0);
                list_tasks(&tasks, selected, stdscr, &term);
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
                            cmd(&tasks, selected, &term);
                        }
                        else if (selected == tasks.count) {
                            cmd_task(&tasks, &term);
                        }
                        else if (selected == tasks.count+1) {
                            state = QUIT;
                        }
                        break;
                    case 'C':
                        if (selected < tasks.count) {
                            tasks.items[selected].completed = true;
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