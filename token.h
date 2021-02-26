//
// Created by Artyom Plevako on 25.02.2021.
//

#ifndef TIGY_TOKEN_H
#define TIGY_TOKEN_H

#define TOKENLEN 20

struct token {
        enum { ID, KEYWORD, INT, FLOAT, STRING, PUNCTUATOR, EMPTY } name;
        char value[TOKENLEN];
};

struct token get_token(FILE *fp);

#endif //TIGY_TOKEN_H
