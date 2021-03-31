#include <stdio.h>
#include <stdlib.h>

#include "front-end/syntax/source-file.h"
#include "front-end/semantics/program.h"
#include "back-end/execute.h"

static const char *parse_args(const char **argv)
{
	const char *filename = NULL;
	while (*++argv != NULL) {
		if (filename == NULL)
			filename = *argv;
		else {
			fputs("tigy: too many input files\n", stderr);
			return NULL;
		}
	}
	if (filename == NULL) {
		fputs("tigy: no input file\n", stderr);
		return NULL;
	} else
		return filename;
}

int main(int argc, const char **argv)
{
	struct source_file *file;
	const char *filename = parse_args(argv);
	if (filename == NULL || (file = open_source_file(filename)) == NULL)
		return EXIT_FAILURE;
	struct program *program = parse_source_file(file);
	if (file->is_correct)
		execute_program(program);
	else
		return EXIT_FAILURE;
	program_free(&program);
	close_source_file(&file);
	if (file != NULL)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
