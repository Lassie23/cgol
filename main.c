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
            if (board[x * bx + y] == 1) {
                mvprintw(y, x*2, "██");
            }
        }
    }
    refresh();
}

int count_live_neighbours(int board[bx * by], int x, int y) {
    return \
           board[((x - 1) % bx) * bx + ((y - 1) % by)] \
         + board[  x            * bx + ((y - 1) % by)] \
         + board[((x + 1) % bx) * bx + ((y - 1) % by)] \
         + board[((x - 1) % bx) * bx +   y           ] \
         + board[((x + 1) % bx) * bx +   y           ] \
         + board[((x - 1) % bx) * bx + ((y + 1) % by)] \
         + board[  x            * bx + ((y + 1) % by)] \
         + board[((x + 1) % bx) * bx + ((y + 1) % by)];
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
            board[x * bx + y] = rand() % 3 % 2;
        }
    }
    int counts[bx * by];
    setlocale(LC_ALL, "");
    initscr();
    draw(board);
    delay(delay_time);
    for (;;) {
        // count neighbors
        for (int x = 0; x < bx; x++) {
            for (int y = 0; y < by; y++) {
                counts[x * bx + y] = count_live_neighbours(board, x, y);
            }
        }
        // update the board from counts
        for (int x = 0; x < bx; x++) {
            for (int y = 0; y < by; y++) {
                if (board[x * bx + y]) {
                    if (!(counts[x * bx + y] == 2 || counts[x * bx + y] == 3)) {
                        board[x * bx + y] = 0;
                    }
                } else {
                    if (counts[x * bx + y] == 3) {
                        board[x * bx + y] = 1;
                    }
                }
            }
        }
        draw(board);
        delay(delay_time);
    }
}
