cmake_minimum_required(VERSION 3.10)

project(master)

set(SOURCES
    src/main.c
    src/connection_master.c
)

set(HEADERS
    src/config.h
    src/connection_master.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
