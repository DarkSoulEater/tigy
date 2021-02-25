//
// Created by Artyom Plevako on 25.02.2021.
//

#ifndef TIGY_TOKEN_H
#define TIGY_TOKEN_H

struct token {
	enum { ID, KEYWORD, INT, FLOAT, STRING, UNKNOWN } name;
	char *value;
};

#endif //TIGY_TOKEN_H
