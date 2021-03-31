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

#endif //TIGY_GENERATE_H
