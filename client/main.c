#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

int create_connection(char* node, char* service) {
    struct addrinfo *res = NULL;
    int gai_err;
    struct addrinfo hint = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };
    if ((gai_err = getaddrinfo(node, service, &hint, &res))) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(gai_err));
        return -1;
    }
    int sock = -1;
    for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
        sock = socket(ai->ai_family, ai->ai_socktype, 0);
        if (sock < 0) {
            perror("socket");
            continue;
        }
        if (connect(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            perror("connect");
            close(sock);
            sock = -1;
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    return sock;
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
    if (argc < 3) {
        printf("Usage: ./client SERVER_ADDRESS <command> [args...].\n");
        return 0;
    }
    char *divider = argv[1];
    for (divider = argv[1]; (*divider) != '\0'; ++divider) {
    }
    while ((*divider) != ':') {
        --divider;
    }
    *divider = '\0';
    char *ip = argv[1];
    char *port = divider + 1;
    int sock = create_connection(ip, port);
    if (strcmp(argv[2], "spawn") == 0) {
        int command_args = argc - 3;
        if (write(sock, &command_args, sizeof(command_args)) < 0) {
            perror("write");
            return -1;
        }
        for (int i = 3; i < argc; ++i) {        
            if (write(sock, argv[i], strlen(argv[i]) + 1) < 0) {
                perror("write");
                return -1;
            }
        }
        pid_t pid = fork();
        if (pid < 0) {
            close(sock);
            perror("fork");
            return -1;
        } else if (pid == 0) {
            char buf[BUF_SIZE];
            int read_bytes = 0;
            while ((read_bytes = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
                write(sock, buf, read_bytes);
            }
            shutdown(sock, SHUT_WR);
            exit(EXIT_SUCCESS);
        }
        char buf[BUF_SIZE];
        int read_bytes = 0;
        while ((read_bytes = read(sock, buf, BUF_SIZE)) > 0) {
            write(STDOUT_FILENO, buf, read_bytes);
        }
        kill(pid, SIGKILL);
        wait(NULL);
    }
}