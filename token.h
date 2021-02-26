//
// Created by Artyom Plevako on 25.02.2021.
//

#ifndef TIGY_TOKEN_H
#define TIGY_TOKEN_H

#define TOKENLEN 20

struct token {
	enum { ID, KEYWORD, INT, FLOAT, STRING, UNKNOWN } name;
	char value[TOKENLEN];
};

#endif //TIGY_TOKEN_H
