//
// Created by Artyom Plevako on 31.03.2021.
//

#ifndef TIGY_GENERATE_H
#define TIGY_GENERATE_H

#include "../util/array.h"

extern struct array *execution_sequence;
extern struct array *execution_operands;

extern void init_generator(void);
extern void clean_up_generator(void);

extern void add(void);
extern void sub(void);
extern void mult(void);
extern void div(void);

extern void less(void);
extern void lesseq(void);
extern void greater(void);
extern void greatereq(void);
extern void equal(void);
extern void not_equal(void);

extern void logical_or(void);
extern void logical_and(void);
extern void logical_not(void);

extern void assigment(void);
extern void pop(void);

#endif //TIGY_GENERATE_H
