cmake_minimum_required(VERSION 3.17)
project(tigy C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-pedantic")

include_directories(src)

add_executable(
        ${PROJECT_NAME}
        src/main.c
        src/front-end/syntax/parser.c
        src/front-end/syntax/lexer.c
        src/front-end/syntax/token.h
        src/front-end/syntax/source-file.c
        src/front-end/syntax/source-file.h
        src/front-end/semantics/program.c
        src/front-end/semantics/program.h
        src/front-end/semantics/types.h
        src/front-end/semantics/type-check.c
        src/front-end/semantics/type-check.h
        src/front-end/semantics/scope.c
        src/front-end/semantics/scope.h
        src/back-end/generate.c
        src/back-end/generate.h
        src/back-end/execute.c
        src/back-end/execute.h
        src/util/hash-table.c
        src/util/hash-table.h
        src/util/stack.c
        src/util/stack.h
        src/util/array.c
        src/util/array.h)
