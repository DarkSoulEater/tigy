//
// Created by Artyom Plevako on 22.03.2021.
//

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "source-file.h"

struct source_file *open_source_file(const char *filename)
{
	assert(filename != NULL);
	FILE *data = fopen(filename, "r");
	if (data == NULL) {
		fprintf(stderr, "tigy: could not open %s\n", filename);
		return NULL;
	}
	struct source_file *file = malloc(sizeof(struct source_file));
	file->name = malloc(sizeof *filename * (strlen(filename) + 1));
	strcpy(file->name, filename);
	file->data = data;
	file->line = 1, file->column = 1;
	file->is_correct = true;
	return file;
}

void close_source_file(struct source_file **file)
{
	assert(file != NULL && *file != NULL);
	if (fclose((*file)->data) == EOF) {
		fprintf(stderr, "tigy: could not close %s\n", (*file)->name);
		free((*file)->name);
		free(*file);
		return;
	}
	free((*file)->name);
	free(*file);
	*file = NULL;
}

void print_error(struct source_file *file, int line, int column, const char *format, ...)
{
	assert(file != NULL && format != NULL);
	file->is_correct = false;
	va_list message;
	va_start(message, format);
	fprintf(stderr, "tigy:%s:%d:%d: error: ", file->name, line, column);
	vfprintf(stderr, format, message);
	putc('\n', stderr);
	va_end(message);
}
