#ifndef TIGY_TYPE_CHECK_H
#define TIGY_TYPE_CHECK_H

#include "front-end/syntax/source-file.h"
#include "util/stack.h"

extern struct stack *expression_types;

extern struct type *string_type;
extern struct type *int_type;
extern struct type *float_type;

extern void init_type_check(void);
extern void clean_up_type_check(void);

extern void check_algebraic_operation(struct source_file *file, struct token operation);
extern void check_comparison_operation(struct source_file *file, struct token operation);
extern void check_logical_operation(struct source_file *file, struct token operation);
extern void check_assignment_operation(struct source_file *file, struct token operation);
extern void check_unary_minus(struct source_file *file, struct token operation);

#endif //TIGY_TYPE_CHECK_H
