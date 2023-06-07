
#include "proxy.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

int wait_for_connections(int server_sock) {
    int sock;
    int child_count = 0;
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
            } else {
                ++child_count;
            }
        }
    }
    for (int i = 0; i < child_count; i++) {
        wait(NULL);
    }
    return 0;
}