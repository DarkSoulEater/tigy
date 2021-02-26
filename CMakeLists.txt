cmake_minimum_required(VERSION 3.17)
project(tigy C)

set(CMAKE_C_STANDARD 90)

add_executable(tigy main.c parser.c lexer.c token.h)
