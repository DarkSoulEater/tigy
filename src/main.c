#include <stdio.h>
#include <stdlib.h>

#include "source-file.h"

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
	parse_source_file(file);
	if (!file->is_correct)
		return EXIT_FAILURE;
	close_source_file(&file);
	if (file != NULL)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
