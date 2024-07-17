cmake_minimum_required(VERSION 3.10)

project(slave)

set(SOURCES
    src/main.c
)

set(HEADERS
    src/config.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
