#include <stdlib.h>
#include <stdio.h>

extern void parsefile(FILE *);
static char *parseargs(char **);

int main(int argc, char **argv)
{
	FILE *fp;
	char *filename;

	if ((filename = parseargs(argv)) == NULL)
		return EXIT_FAILURE;
	if ((fp = fopen(filename, "r")) == NULL) {
		fputs("tigy: could not open the file", stderr);
		return EXIT_FAILURE;
	}
	parsefile(fp);
	return EXIT_SUCCESS;
}

char *parseargs(char **argv)
{
	char *filename = NULL;

	while (*++argv != NULL) {
		if (filename == NULL)
			filename = *argv;
		else {
			fputs("tigy: too many input files", stderr);
			return NULL;
		}
	}
	if (filename == NULL) {
		fputs("tigy: no input file", stderr);
		return NULL;
	} else {
		return filename;
	}
}
