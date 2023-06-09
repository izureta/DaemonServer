
#include "proxy.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

void wait_for_child() {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);
}

int wait_for_connections(int server_sock) {
    struct sigaction sigchld_sa = {
        .sa_handler = wait_for_child,
        .sa_flags = SA_RESTART | SA_NOCLDSTOP,
    };
    sigaction(SIGCHLD, &sigchld_sa, NULL);
    int sock;
    while (1) { 
        if ((sock = accept(server_sock, NULL, NULL)) < 0) {
            perror("accept");
        } else {
            pid_t pid = fork();
            if (pid < 0) {
                close(sock);
                perror("fork");
                continue;
            } else if (pid == 0) {
                if (start_proxy(sock) < 0) {
                    fprintf(stderr, "Proxy failed\n");
                }
                close(sock);
                _exit(EXIT_SUCCESS);
            }
        }
    }
    int status;
    while (waitpid(-1, &status, 0) >= 0);
    return 0;
}