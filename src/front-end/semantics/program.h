//
// Created by Artyom Plevako on 30.03.2021.
//

#ifndef TIGY_PROGRAM_H
#define TIGY_PROGRAM_H

struct program *program_allocate(void);
void program_free(struct program **program);

void program_begin_scope(struct program *program);
void program_end_scope(struct program *program);
struct scope *program_current_scope(struct program *program);

#endif //TIGY_PROGRAM_H
