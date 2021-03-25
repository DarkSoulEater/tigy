cmake_minimum_required(VERSION 3.17)
project(tigy C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-pedantic")

add_executable(
        ${PROJECT_NAME}
        src/main.c
        src/parser.c
        src/lexer.c
        src/token.h
        src/hash-table.c
        src/hash-table.h
        src/source-file.c
        src/source-file.h)
