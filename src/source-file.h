//
// Created by Artyom Plevako on 22.03.2021.
//

#ifndef TIGY_SOURCE_FILE_H
#define TIGY_SOURCE_FILE_H

#include <stdbool.h>
#include <stdio.h>

struct source_file {
	char *name;
	FILE *data;
	int line, column;
	bool is_correct;
};

extern struct source_file *open_source_file(const char *filename);
extern void close_source_file(struct source_file **file);

extern void parse_source_file(struct source_file *file);
extern struct token get_token(struct source_file *file);

extern void print_error(struct source_file *location, int line, int column, const char *format, ...);

#endif //TIGY_SOURCE_FILE_H
