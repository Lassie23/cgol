#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

int bx, by;
int delay_time = 250;

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
           board[((y - 1) % by) * bx + ((x - 1) % bx)] \
         + board[((y - 1) % by) * bx +   x           ] \
         + board[((y - 1) % by) * bx + ((x + 1) % bx)] \
         + board[  y            * bx + ((x - 1) % bx)] \
         + board[  y            * bx + ((x + 1) % bx)] \
         + board[((y + 1) % by) * bx + ((x - 1) % bx)] \
         + board[((y + 1) % by) * bx +   x           ] \
         + board[((y + 1) % by) * bx + ((x + 1) % bx)];
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        return 1;
    }
    int isNumber = 1;
    int z = 0;
    while (z<strlen(argv[1]) && isNumber == 1) {
        if (argv[1][z] < "0"[0] || argv[1][z] > "9"[0]) {
            isNumber = 0;
        }
        z++;
    }
    z = 0;
    while (z<strlen(argv[2]) && isNumber == 1) {
        if (argv[2][z] < "0"[0] || argv[2][z] > "9"[0]) {
            isNumber = 0;
        }
        z++;
    }
    if (argc == 4) {
        z = 0;
        while (z<strlen(argv[2]) && isNumber == 1) {
            if (argv[2][z] < "0"[0] || argv[2][z] > "9"[0]) {
                isNumber = 0;
            }
            z++;
        }
    }
    if (isNumber == 0) {
        return 1;
    }
    bx = atoi(argv[1]);
    by = atoi(argv[2]);
    if (argc == 4) {
        delay_time = atoi(argv[3]);
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
                        if (!(counts[y * bx + x] == 2 || counts[y * bx + x] == 3)) {
                            board[y * bx + x] = 0;
                        }
                    } else {
                        if (counts[y * bx + x] == 3) {
                            board[y * bx + x] = 1;
                        }
                    }
                }
            }
            draw(board);
            delay(delay_time);
            if (once) {
                running = 0;
                once = 0;
            }
        }
        refresh();
        switch (ch) {
            case KEY_MOUSE:
                refresh();
                if (getmouse(&event) == OK) {
                    refresh();
                    if (event.bstate & BUTTON1_PRESSED) {
                        board[event.y * bx + event.x/2] = !board[event.y * bx + event.x/2];
                        draw(board);
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
