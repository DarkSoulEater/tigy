#include <stdlib.h>
#include <stdio.h>

extern void  parsefile(FILE *);
static char *parseargs(char **);

#include "token.h"
extern struct token gettoken(FILE* fp);

#include "hashtable.h"
#include <string.h>

int main(int argc, char **argv)
{
        struct hashtable tb;
        table_create(&tb);
        table_insert(&tb, "var");
        table_insert(&tb, "int");
        table_insert(&tb, "let");
        table_insert(&tb, "le");
        table_insert(&tb, "lt");
        //table_insert(&tb, "et");
        table_insert(&tb, "led");
        table_insert(&tb, "ledd");
        table_insert(&tb, "ledwd");
        printf("%i\n", table_find(&tb, "int"));
        printf("%i\n", table_find(&tb, "i"));
        printf("%i\n", table_find(&tb, "let"));
        table_remove(&tb, "let");
        printf("%i\n", table_find(&tb, "let"));

	FILE *fp;
	char *filename;

	if ((filename = parseargs(argv)) == NULL)
		return EXIT_FAILURE;
	if ((fp = fopen(filename, "r")) == NULL) {
		fputs("tigy: could not open the file", stderr);
		return EXIT_FAILURE;
	}
	//parsefile(fp);
	struct token t;
        while ((t = gettoken(fp)).name != EMPTY) {
                switch (t.name) {
                        case ID: {
                                printf("ID %s\n", t.value);
                                break;
                        }

                        case KEYWORD: {
                                printf("KEYWORD %s\n", t.value);
                                break;
                        }

                        case INT: {
                                printf("INT_CONST %s\n", t.value);
                                break;
                        }

                        case STR: {
                                printf("STR_CONST %s\n", t.value);
                                break;
                        }

                        case PUNCTUATOR: {
                                printf("PUNCT %s\n", t.value);
                                break;
                        }
                }
        }
	fclose(fp);
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
	} else
		return filename;
}
