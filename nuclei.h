#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "lisp.h"

enum TokenType_
{
    LPAREN,
    RPAREN,
    QUOTE,
    STRING,
    VAR,
    NUMBER,
    WHILE,
    IF,
    SET,
    PRINT,
    CAR,
    CDR,
    CONS,
    PLUS,
    LENGTH,
    LESS,
    GREATER,
    EQUAL,
    NILL,
    EOF_TOKEN,
    ERROR
};

char *tokenNames[] = {
    "LPAREN", "RPAREN",
    "QUOTE", "STRING",
    "VAR", "NUMBER",
    "WHILE", "IF",
    "SET", "PRINT",
    "CAR", "CDR", "CONS",
    "PLUS", "LENGTH",
    "LESS", "GREATER", "EQUAL", "NIL",
    "EOF_TOKEN", "ERROR"};

#define NUM_KEYWORDS 13
char *keywords[] = {
    "WHILE", "IF",
    "SET", "PRINT",
    "CAR", "CDR", "CONS",
    "PLUS", "LENGTH",
    "LESS", "GREATER", "EQUAL",
    "NIL"};

struct Token_
{
    enum TokenType_ type;
    char *lexme;
    int value;
};

typedef struct Token_ Token;
typedef enum TokenType_ TokenType;

//Lexer functions
Token parseString(FILE *file);
Token parseNumber(FILE *file, char c);
Token parseVarKeyword(FILE *file, char c);
Token extractToken(FILE *input);
Token getToken();
void ungetToken(Token t);



//Parser/Interpreter functions
void parse_program(char *filename);
void parse_instructs();
void parse_instruct();
void parse_func();
void parse_retfunc();
void parse_listfunc();
void parse_intfunc();
void parse_boolfunc();
void parse_iofunc();
void parse_set();
void parse_print();
void parse_if();
void parse_loop();
void parse_list();
void parse_var();
lisp *parse_literal();
lisp *parse_obj_list();
int main(int argc, char *argv[]);

//Testing functions
void test_token();
void test_parse_program_1();
void test_parse_program_2();
void test_parse_program_3();

#endif
