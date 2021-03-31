//
// Created by Artyom Plevako on 31.03.2021.
//

#include "generate.h"

struct array *execution_sequence;
struct array *execution_operands;

void init_generator(void)
{
	execution_sequence = array_allocate();
	execution_operands = array_allocate();
}

void clean_up_generator(void)
{
	array_free(&execution_sequence);
	array_free(&execution_operands);
}
