//
// Created by Artyom Plevako on 31.03.2021.
//

#include "front-end/semantics/types.h"
#include "front-end/semantics/type-check.h"

struct stack *expression_types;

void init_type_check(void)
{
	expression_types = stack_allocate();
}

void clean_up_type_check(void)
{
	stack_free(&expression_types);
}
