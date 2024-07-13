#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>
#include <getopt.h>
#include <ctype.h>

int bx = 50, by = 50;
int delay_time = 100;
int survival[] = {2, 3, 0, 0, 0, 0, 0, 0, 0, 0};
int s_size = 2;
int birth[] = {3, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int b_size = 1;

void delay(int milli_seconds) {
    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds * 1000) { ;
    }
}

void draw(int board[bx * by]) {
    clear();
    for (int x = 0; x < bx; x++) {
        for (int y = 0; y < by; y++) {
            if (board[y * bx + x] == 1) {
                mvprintw(y, x*2, "██");
            }
        }
    }
    refresh();
}

int count_live_neighbours(int board[bx * by], int x, int y) {
    return \
           board[((y+by - 1) % by) * bx + ((x+bx - 1) % bx)] \
         + board[((y+by - 1) % by) * bx +   x              ] \
         + board[((y+by - 1) % by) * bx + ((x+bx + 1) % bx)] \
         + board[  y               * bx + ((x+bx - 1) % bx)] \
         + board[  y               * bx + ((x+bx + 1) % bx)] \
         + board[((y+by + 1) % by) * bx + ((x+bx - 1) % bx)] \
         + board[((y+by + 1) % by) * bx +   x              ] \
         + board[((y+by + 1) % by) * bx + ((x+bx + 1) % bx)];
}

int isNumber(char *s) {
    int is = 1;
    int z = 0;
    while (z<strlen(s) && is == 1) {
        if (s[z] < "0"[0] || s[z] > "9"[0]) {
            is = 0;
        }
        z++;
    }
    return is;
}

int isinarray(int val, int *arr, size_t size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == val) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int c;
    while ((c = getopt(argc, argv, "x:y:d:s:b:")) != -1) {
        switch (c) {
            case 'x':
                if (isNumber(optarg)) {
                    bx = atoi(optarg);
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 'y':
                if (isNumber(optarg)) {
                    by = atoi(optarg);
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 'd':
                if (isNumber(optarg)) {
                    delay_time = atoi(optarg);
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 's':
                if (isNumber(optarg)) {
                    s_size = strlen(optarg);
                    for (int z = 0; z < s_size; z++) {
                        survival[z] = optarg[z] - '0';
                    }
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case 'b':
                if (isNumber(optarg)) {
                    b_size = strlen(optarg);
                    for (int z = 0; z < b_size; z++) {
                        birth[z] = optarg[z] - '0';
                    }
                } else {
                    fprintf (stderr, "Option -%c must be a number.\n", optopt);
                    return 1;
                }
                break;
            case '?':
                if (optopt == 'x' || optopt == 'y' || optopt == 'd') {
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint (optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort ();
        }
    }
    srand(time(0));
    int board[bx * by];
    for (int x = 0; x < bx; x++) {
        for (int y = 0; y < by; y++) {
            board[y * bx + x] = rand() % 3 % 2;
        }
    }
    int counts[bx * by];
    setlocale(LC_ALL, "");
    MEVENT event;
    initscr();
    raw();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    noecho();
    clear();
    cbreak();
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    curs_set(0);
    draw(board);
    delay(delay_time);
    int ch;
    int running = 1;
    int once = 0;
    while ((ch = getch()) != 'q') {
        if (running) {
            // count neighbors
            for (int x = 0; x < bx; x++) {
                for (int y = 0; y < by; y++) {
                    counts[y * bx + x] = count_live_neighbours(board, x, y);
                }
            }
            // update the board from counts
            for (int x = 0; x < bx; x++) {
                for (int y = 0; y < by; y++) {
                    if (board[y * bx + x]) {
                        if (!isinarray(counts[y * bx + x], survival, s_size)) {
                            board[y * bx + x] = 0;
                        }
                    } else {
                        if (isinarray(counts[y * bx + x], birth, b_size)) {
                            board[y * bx + x] = 1;
                        }
                    }
                }
            }
            draw(board);
            if (once) {
                running = 0;
                once = 0;
            } else {
                delay(delay_time);
            }
        }
        switch (ch) {
            case KEY_MOUSE:
                refresh();
                if (getmouse(&event) == OK) {
                    if (event.bstate & BUTTON1_PRESSED) {
                        if (event.x/2 < bx && event.y < by) {
                            board[event.y * bx + event.x/2] = !board[event.y * bx + event.x/2];
                            draw(board);
                        }
                    }
                }
                break;
            case ' ':
                running = !running;
                break;
            case 'r':
                for (int x = 0; x < bx; x++) {
                    for (int y = 0; y < by; y++) {
                        board[y * bx + x] = rand() % 3 % 2;
                    }
                }
                draw(board);
                break;
            case 'c':
                for (int x = 0; x < bx; x++) {
                    for (int y = 0; y < by; y++) {
                        board[y * bx + x] = 0;
                    }
                }
                draw(board);
                break;
            case 'i':
                for (int x = 0; x < bx; x++) {
                    for (int y = 0; y < by; y++) {
                        board[y * bx + x] = !board[y * bx + x];
                    }
                }
                draw(board);
                break;
            case 's':
                running = 1;
                once = 1;
                break;
        }
    }
    endwin();
}
