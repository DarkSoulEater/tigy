//
// Created by Artyom Plevako on 27.02.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include "diagnostics.h"

void error(const char *msg, const char *token, int line, int col)
{
	fprintf(stderr, "tigy:%d:%d: error: %s '%s'\n", line, col, msg, token);
	exit(EXIT_FAILURE);
}
