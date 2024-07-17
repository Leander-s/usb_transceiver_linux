cmake_minimum_required(VERSION 3.10)

project(slave)

set(SOURCES
    src/main.c
    src/connection_slave.c
)

set(HEADERS
    src/config.h
    src/connection_slave.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
