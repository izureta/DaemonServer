#include <stdio.h>
#include <unistd.h>

enum {
    BUF_SIZE = 1024,
};

int work(char *args[]) {
    if (execvp(args[0], args) < 0) {
        perror("execvp");
        return -1;
    }
}