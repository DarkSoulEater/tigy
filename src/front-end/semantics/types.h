//
// Created by Artyom Plevako on 31.03.2021.
//

#ifndef TIGY_TYPES_H
#define TIGY_TYPES_H

#include "util/array.h"
#include "front-end/syntax/token.h"

struct type {
	enum type_kind {
		INT,
		FLOAT,
		STRING,
		ARRAY,
		RECORD,
	} kind;
	void *data;
};

struct record {
	struct array *fields;
};

struct field {
	char *identifier;
	struct type *type;
};

struct variable {
	struct type *type;
	void *data;
};

struct function {
	struct array *fields;
	struct type *return_type;
	struct scope *scope;
};

#endif //TIGY_TYPES_H
