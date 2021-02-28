//
// Created by Artyom Plevako on 25.02.2021.
//

#ifndef TIGY_TOKEN_H
#define TIGY_TOKEN_H

#define TOKLEN 20

struct token {
        enum { ID, KEYWORD, INT, FLOAT, STR, PUNCTUATOR, EMPTY } name;
        char value[TOKLEN];
        int  line, col;
};

#endif //TIGY_TOKEN_H
