//
// Created by Artyom Plevako on 25.02.2021.
//

#ifndef TIGY_TOKEN_H
#define TIGY_TOKEN_H

#define IDENTIFIER_LENGTH 32
#define STRING_CONSTANT_LENGTH 128

struct token {
        enum token_type {
        	NONE,
        	IDENTIFIER,
		INTEGER_CONSTANT,
		STRING_CONSTANT,
		FLOAT_CONSTANT,
        	ARRAY_KEYWORD,
        	BREAK_KEYWORD,
        	DO_KEYWORD,
        	ELSE_KEYWORD,
        	END_KEYWORD,
        	FOR_KEYWORD,
        	FUNCTION_KEYWORD,
        	IF_KEYWORD,
        	IN_KEYWORD,
        	LET_KEYWORD,
        	NIL_KEYWORD,
        	OF_KEYWORD,
        	THEN_KEYWORD,
        	TO_KEYWORD,
        	TYPE_KEYWORD,
        	VAR_KEYWORD,
        	WHILE_KEYWORD,
        	COMMA,
        	COLON,
        	SEMICOLON,
        	LEFT_PARENTHESIS,
        	RIGHT_PARENTHESIS,
        	LEFT_BRACKET,
        	RIGHT_BRACKET,
        	LEFT_BRACE,
        	RIGHT_BRACE,
        	DOT,
        	PLUS,
        	MINUS,
        	ASTERISK,
        	SLASH,
        	EQUAL,
        	NOT_EQUAL,
        	LESS,
        	LESS_EQUAL,
        	GREATER,
        	GREATER_EQUAL,
        	AND,
        	OR,
        	ASSIGNMENT,
        } name;
        union {
        	char identifier[IDENTIFIER_LENGTH];
		int integer_constant;
		float float_constant;
		char string_constant[STRING_CONSTANT_LENGTH];
        } value;
        int line, column;
};

#endif //TIGY_TOKEN_H
