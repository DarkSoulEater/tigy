//
// Created by Artyom Plevako on 31.03.2021.
//

#ifndef TIGY_TYPE_CHECK_H
#define TIGY_TYPE_CHECK_H

#include "front-end/syntax/source-file.h"
#include "util/stack.h"

extern struct stack *expression_types;

extern void init_type_check(void);
extern void clean_up_type_check(void);

extern void check_algebraic_operation(struct source_file *file, struct token *operation);
extern void check_equality_operation(struct source_file *file, struct token *operation);
extern void check_comparison_operation(struct source_file *file, struct token *operation);
extern void check_logical_operation(struct source_file *file, struct token *operation);
extern void check_assignment_operation(struct source_file *file, struct token *operation);

#endif //TIGY_TYPE_CHECK_H
