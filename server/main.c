#include "server_initializer.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int server_pid = -1;

void stop_server_handler(int signo) {
    (void)signo;
    kill(0, SIGTERM);
    printf("\n");
    exit(0);
}

void set_signal_handler() {
    struct sigaction sigint_sa = {
        .sa_handler = stop_server_handler,
        .sa_flags = SA_RESTART,
    };
    struct sigaction sigterm_sa = {
        .sa_handler = stop_server_handler,
        .sa_flags = SA_RESTART,
    };
    sigaction(SIGINT, &sigint_sa, NULL);
    sigaction(SIGTERM, &sigterm_sa, NULL);
}

void wait_for_signals() {
    sigset_t mask;
    if (sigemptyset(&mask) < 0) {
        perror("sigemptyset");
        return;
    }
    if (sigsuspend(&mask) < 0) {
        perror("sigsuspend");
        return;
    }
}

enum {
    BUF_SIZE = 1024,
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./server PORT.\n");
        return 0;
    }
    daemon(0, 0);
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        if (start_server(argv[1]) < 0) {
            printf("Server start failed. Exiting the program.\n");
            return -1;
        }
        _exit(EXIT_SUCCESS);
    }
    set_signal_handler();
    server_pid = pid;
    char buf[BUF_SIZE] = {0};
    wait_for_signals();
    kill(0, SIGTERM);
    wait(NULL);
    exit(0);
}