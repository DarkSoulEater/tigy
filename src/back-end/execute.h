//
// Created by Artyom Plevako on 31.03.2021.
//

#ifndef TIGY_EXECUTE_H
#define TIGY_EXECUTE_H

#include "front-end/semantics/program.h"

extern struct stack *execution_buffer;

extern void execute_scope(struct scope *scope);
extern void execute_program(struct program *program);

#endif //TIGY_EXECUTE_H
