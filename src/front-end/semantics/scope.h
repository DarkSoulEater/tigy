#ifndef TIGY_SCOPE_H
#define TIGY_SCOPE_H

struct execution_unit {
	enum unit_kind {
		OPERAND,
		OPERATION,
	} kind;
	void *data;
};

struct scope {
	struct hash_table *type_namespace, *variable_namespace, *function_namespace;
	struct array *execution_sequence;
	struct scope *parent;
};

struct scope *scope_allocate(void);
void scope_free(struct scope **scope);

void scope_insert_type(struct scope *scope, const char *key, void *value);
void *scope_lookup_type(struct scope *scope, const char *key);
void *scope_deep_lookup_type(struct scope *scope, const char *key);

void scope_insert_variable(struct scope *scope, const char *key, void *value);
void *scope_lookup_variable(struct scope *scope, const char *key);
void *scope_deep_lookup_variable(struct scope *scope, const char *key);

void scope_insert_function(struct scope *scope, const char *key, void *value);
void *scope_lookup_function(struct scope *scope, const char *key);
void *scope_deep_lookup_function(struct scope *scope, const char *key);

#endif //TIGY_SCOPE_H
