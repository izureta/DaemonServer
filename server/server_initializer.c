#include "waiter.h"
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int create_listener(const char *node, const char *service) {
    int gai_err;
    struct addrinfo hint = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE,
    };
    struct addrinfo *info_res = NULL;
    if ((gai_err = getaddrinfo(node, service, &hint, &info_res))) {
        return -1;
    }
    int sock = -1;
    for (struct addrinfo *ai = info_res; ai; ai = ai->ai_next) {
        sock = socket(ai->ai_family, ai->ai_socktype, 0);
        if (sock < 0) {
            perror("socket");
            continue;
        }
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            perror("bind");
            close(sock);
            sock = -1;
            continue;
        }
        if (listen(sock, SOMAXCONN) < 0) {
            perror("listen");
            close(sock);
            sock = -1;
            continue;
        }
        break;
    }
    freeaddrinfo(info_res);
    return sock;
}

int start_server(char *port) {
    int server_sock = create_listener("::1", port);
    if (server_sock < 0) {
        return server_sock;
    }
    wait_for_connections(server_sock);
}

