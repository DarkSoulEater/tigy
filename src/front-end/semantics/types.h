//
// Created by Artyom Plevako on 31.03.2021.
//

#ifndef TIGY_TYPES_H
#define TIGY_TYPES_H

#include "util/array.h"

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

struct variable {
	struct type *type;
	void *data;
};

struct function {
	struct array *fields;
	struct scope *scope;
};

#endif //TIGY_TYPES_H
