cmake_minimum_required(VERSION 3.16.2)

project(Daemon)

set(SERVER_SOURCE_FILES
    server/main.c
    server/server_initializer.h
    server/server_initializer.c
    server/waiter.h
    server/waiter.c
    server/proxy.h
    server/proxy.c
    server/worker.h
    server/worker.c   
)

set(CLIENT_SOURCE_FILES
    client/main.c 
)

add_executable(server ${SERVER_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})

