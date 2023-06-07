#include "worker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int sock_global_copy = 0;

void sigchld_handler(int signo) {
    (void)signo;
    shutdown(sock_global_copy, SHUT_RDWR);
}

enum {
    BUF_SIZE = 1024,
};

int start_proxy(int sock) {
    sock_global_copy = sock;
    struct sigaction sigchld_sa = {
        .sa_handler = sigchld_handler,
        .sa_flags = SA_RESTART,
    };
    sigaction(SIGCHLD, &sigchld_sa, NULL);
    int arg_count = 0;
    if (read(sock, &arg_count, sizeof(arg_count)) < 0) {
        perror("read");
        return -1;
    }
    char arg_buf[BUF_SIZE] = {0};
    char *args[BUF_SIZE] = {NULL};
    char *current_arg = arg_buf;
    int last_arg = 0;
    int read_bytes = 0;
    while (arg_count > 0 && (read_bytes = read(sock, current_arg, BUF_SIZE)) > 0) {
        while (arg_count > 0) {
            args[last_arg] = current_arg;
            ++last_arg;
            while (*current_arg) {
                ++current_arg;
            }
            ++current_arg;
            --arg_count;
        }
    }
    if (read_bytes < 0) {
        perror("read");
        return -1;
    }
    args[last_arg] = NULL;
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return -1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        close(sock);
        perror("fork");
        return -1;
    } else if (pid == 0) {
        close(pipefd[1]);
        dup2(sock, STDOUT_FILENO);
        dup2(pipefd[0], STDIN_FILENO);
        if (work(args) < 0) {
            fprintf(stderr, "Worker failed\n");
        }
        close(pipefd[0]);
        close(sock);
        _exit(EXIT_SUCCESS);
    }
    close(pipefd[0]);
    char buf[BUF_SIZE] = {0};
    while ((read_bytes = read(sock, buf, BUF_SIZE)) > 0) {
        if (write(pipefd[1], buf, read_bytes) < 0) {
            perror("write");
            return -1;
        }
    }
    close(pipefd[1]);
    wait(NULL);
    return 0;
}