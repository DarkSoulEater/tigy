//
// Created by Artyom Plevako on 30.03.2021.
//

#ifndef TIGY_NAMESPACE_H
#define TIGY_NAMESPACE_H

struct namespace *namespace_allocate(void);
void namespace_begin_scope(struct namespace *space);
void namespace_end_scope(struct namespace *space);
void namespace_insert(struct namespace *space, const char *key, void *value);
void *namespace_lookup(struct namespace *space, const char *key);
void *namespace_deep_lookup(struct namespace *space, const char *key);
void namespace_free(struct namespace **space);

#endif //TIGY_NAMESPACE_H
