#include "specific.h"
#include "nuclei.h"
#include <ctype.h>
#include <stdlib.h>
#define DEBUG 0

#if DEBUG
#define FUNC_BEG printf("%s: %d\n", __func__, __LINE__);
#define FUNC_END \
    printf("END %s: %d\n\n", __func__, __LINE__);
#else
#define FUNC_BEG
#define FUNC_END
#endif

//----LEXER----
Token parseString(FILE *file)
{

    Token token;
    char curr_len = 8;
    char *str = malloc(sizeof(char) * 8);
    int i = 0;
    char c = fgetc(file);

    while (c != '\"')
    {
        if (i == curr_len)
        {
            curr_len *= 2;
            str = realloc(str, sizeof(char) * curr_len);
        }
        str[i] = c;
        i++;
        c = fgetc(file);
    }

    str[i] = '\0';
    token.lexme = str;
    token.type = STRING;
    return token;
}

Token parseNumber(FILE *file, char c)
{

    Token token;
    token.value = 0;
    bool isNegative = false;
    int i = 0;

    if (c == '-')
    {
        isNegative = true;
        c = fgetc(file);
    }

    while (isdigit(c))
    {
        token.value = token.value * 10 + (c - '0');
        i++;
        c = fgetc(file);
    }
    ungetc(c, file);
    token.value = isNegative ? -token.value : token.value;
    token.lexme = NULL;
    token.type = NUMBER;

    return token;
}

Token parseVarKeyword(FILE *file, char c)
{

    Token token;
    char *var = malloc(sizeof(char) * 20);
    int i = 0;
    while (isalpha(c))
    {
        var[i] = c;
        i++;
        c = fgetc(file);
    }
    ungetc(c, file);

    var[i] = '\0';
    token.lexme = var;

    for (int i = 0; i < NUM_KEYWORDS; i++)
    {
        if (strcmp(var, keywords[i]) == 0)
        {
            token.type = WHILE + i;
            return token;
        }
    }
    if (strlen(var) > 1) // variable name should be of length 1
        token.type = ERROR;
    else
        token.type = VAR;

    return token;
}

Token extractToken(FILE *file)
{

    Token token;
    char c;
    // skip the whitespace
    while (isspace(c = fgetc(file)))
        ;

    if (c == '#') // skip comment
    {
        while (c != '\n')
            c = fgetc(file);

        return extractToken(file);
    }

    if (c == EOF)
    {
        token.lexme = NULL;
        token.type = EOF_TOKEN;
        return token;
    }

    switch (c)
    {
    case '(':
        token.lexme = "(";
        token.type = LPAREN;
        break;
    case ')':
        token.lexme = ")";
        token.type = RPAREN;
        break;
    case '\'':
        token.lexme = "'";
        token.type = QUOTE;
        break;
    case '\"':
        token = parseString(file);
        break;
    case '-':
        token = parseNumber(file, c);
        break;
    default:
        // parse number/variable/instruction
        if (isalnum(c))
        {
            if (isdigit(c))
                token = parseNumber(file, c);
            else
                token = parseVarKeyword(file, c);
        }
        else
        {
            token.lexme = NULL;
            token.type = ERROR;
        }
    }

    return token;
}
//----END OF LEXER----


//----PARSER/INTERPRETER----

/* The stack below manages a stack of size 3
to get and unget tokens*/

Token token_stack[3];
int curr_token_index = 0;
FILE *program_file;

Token getToken()
{
    if (curr_token_index == 0)
    {
        Token t = extractToken(program_file);
#if DEBUG
        printf("Token: %s %s\n", tokenNames[t.type], t.lexme);
#endif
        return t;
    }
    else
    {
        Token t = token_stack[--curr_token_index];
#if DEBUG
        printf("Token: %s %s\n", tokenNames[t.type], t.lexme);
#endif
        return t;
    }
}

void ungetToken(Token token)
{

    assert(curr_token_index < 3);
    token_stack[curr_token_index++] = token;
}


// parser entry point
void parse_program(char *filename)
{
    FUNC_BEG
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Error: File not found %s\n", filename);
        exit(1);
    }
    program_file = file;

    //<PROG>     ::= "(" <INSTRCTS>
    Token token = getToken();
    if (token.type != LPAREN)
        on_error("Expected ( at the start of the program");

    parse_instructs();

    token = getToken();

    if (token.type != EOF_TOKEN)
        on_error("Expected EOF at the end of the program");

    fclose(file);
    FUNC_END
}

//<INSTRCTS> ::= <INSTRCT> <INSTRCTS> | ")"
void parse_instructs()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type == RPAREN)
        return;
    else
        ungetToken(token);

    parse_instruct();
    parse_instructs();
    FUNC_END
}

//<INSTRCT>  ::= "(" <FUNC> ")"
void parse_instruct()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type != LPAREN)
        on_error("Expected ( at the start of an instruction");

    parse_func();

    token = getToken();
    if (token.type != RPAREN)
        on_error("Expected ) at the end of an instruction");
    FUNC_END
}

//<FUNC>     ::= <RETFUNC> | <IOFUNC> | <IF> | <LOOP>
void parse_func()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type == WHILE)
    {
        ungetToken(token);
        parse_loop();
    }
    else if (token.type == IF)
    {
        ungetToken(token);
        parse_if();
    }
    else if (token.type == PRINT || token.type == SET)
    {
        ungetToken(token);
        parse_iofunc();
    }
    else
    {
        ungetToken(token);
        parse_retfunc();
    }
    FUNC_END
}

//<RETFUNC>  ::= <LISTFUNC> | <INTFUNC> | <BOOLFUNC>
void parse_retfunc()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type == CAR || token.type == CDR || token.type == CONS)
    {
        ungetToken(token);
        parse_listfunc();
    }
    else if (token.type == PLUS || token.type == LENGTH)
    {
        ungetToken(token);
        parse_intfunc();
    }
    else if (token.type == LESS || token.type == EQUAL || token.type == GREATER)
    {
        ungetToken(token);
        parse_boolfunc();
    }
    else
        on_error("Expected a function that returns a value");
    FUNC_END
}

//<LISTFUNC> ::= "CAR" <LIST> | "CDR" <LIST> | "CONS" <LIST> <LIST>
void parse_listfunc()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type == CAR)
    {
        parse_list();
    }
    else if (token.type == CDR)
    {
        parse_list();
    }
    else if (token.type == CONS)
    {
        parse_list();
        parse_list();
    }
    else
        on_error("Expected a list function");
    FUNC_END
}

//<INTFUNC>  ::= "PLUS" <LIST> | "LENGTH" <LIST>
void parse_intfunc()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type == PLUS)
    {
        parse_list();
        parse_list();
    }
    else if (token.type == LENGTH)
    {
        parse_list();
    }
    else
        on_error("Expected an integer function");
    FUNC_END
}

//<BOOLFUNC> ::= "LESS" <LIST> <LIST> | "GREATER" <LIST> <LIST> | "EQUAL" <LIST> <LIST>
void parse_boolfunc()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type == LESS)
    {
        parse_list();
        parse_list();
    }
    else if (token.type == GREATER)
    {
        parse_list();
        parse_list();
    }
    else if (token.type == EQUAL)
    {
        parse_list();
        parse_list();
    }
    else
        on_error("Expected a boolean function");
    FUNC_END
}

//<IOFUNC>   ::= <SET> | <PRINT>
void parse_iofunc()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type == SET)
    {
        ungetToken(token);
        parse_set();
    }
    else if (token.type == PRINT)
    {
        ungetToken(token);
        parse_print();
    }
    else
        on_error("Expected an IO function");
    FUNC_END
}

//<SET>      ::= "SET" <VAR> <LIST>
void parse_set()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type != SET)
        on_error("Expected SET");

    token = getToken();
    if (token.type != VAR)
        on_error("Expected a variable after SET");

    parse_list();
    FUNC_END
}

//<PRINT>    ::= "PRINT" <LIST>
void parse_print()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type != PRINT)
        on_error("Expected PRINT");

    token = getToken();
    if (token.type == STRING)
    {
    }
    else
    {
        ungetToken(token);
        parse_list();
    }
    FUNC_END
}

//<IF>       ::= "IF" "(" <BOOLFUNC> ")" "(" <INSTRCTS> "(" <INSTRCTS>
void parse_if()
{
    FUNC_BEG

    Token t = getToken();
    if (t.type != IF)
        on_error("Expected IF");

    t = getToken();
    if (t.type != LPAREN)
        on_error("Expected ( after IF");

    parse_boolfunc();

    t = getToken();
    if (t.type != RPAREN)
        on_error("Expected ) after boolfunc");

    t = getToken();
    if (t.type != LPAREN)
        on_error("Expected ( before for if true block");

    parse_instructs();

    t = getToken();
    if (t.type != LPAREN)
        on_error("Expected ( before if false block");

    parse_instructs();
    FUNC_END
}

//<LOOP>     ::= "WHILE""(" <BOOLFUNC> ")" "(" <INSTRCTS>
void parse_loop()
{
    FUNC_BEG

    Token t = getToken();
    if (t.type != WHILE)
        on_error("Expected WHILE");

    t = getToken();
    if (t.type != LPAREN)
        on_error("Expected ( after WHILE");

    parse_boolfunc();

    t = getToken();
    if (t.type != RPAREN)
        on_error("Expected ) after boolfunc");

    t = getToken();
    if (t.type != LPAREN)
        on_error("Expected ( before loop block");

    parse_instructs();
    FUNC_END
}

//<LIST>     ::= <VAR> | <LITERAL> | "NIL" | "(" <RETFUNC> ")"
void parse_list()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type == VAR)
    {
        ungetToken(token);
        parse_var();
    }
    else if (token.type == NILL)
    {
    }
    else if (token.type == LPAREN)
    {
        parse_retfunc();
        token = getToken();
        if (token.type != RPAREN)
            on_error("Expected ) after retfunc");
    }
    else if (token.type == QUOTE)
    {
        lisp *list = parse_literal();
        token = getToken();
        if (token.type != QUOTE)
        {
            on_error("Expected ' after literal");
        }
        
        char arr[512];
        lisp_tostring(list, arr);
        lisp_free(&list);
        (void)arr;
    }
    else
    {
        on_error("Error parsing LITERAL");
    }
    FUNC_END
}

//<VAR>      ::= [A-Z]
void parse_var()
{
    FUNC_BEG
    Token token = getToken();
    if (token.type != VAR)
        on_error("Expected a variable");
    FUNC_END
}

//<LITERAL>  ::= "("  <OBJ_LIST>  ")" | NUMBER
lisp *parse_literal()
{
    FUNC_BEG

    Token token = getToken();
    if (token.type == LPAREN)
    {
        lisp * list = parse_obj_list();

        token = getToken();
        if (token.type != RPAREN)
            on_error("Expected ) after list");
        
        return  list;
    }
    else if (token.type == NUMBER)
    {
        // printf("Number: %d\n", token.value);
        return lisp_atom(token.value);
    }
    else{
        on_error("Expected a literal");
    }

    return NIL;
    FUNC_END
}

//<OBJ_LIST> ::= {<literal>} | "NIL"
lisp *parse_obj_list()
{
    FUNC_BEG
    Token token = getToken();
    
    lisp *list = NIL;
    lisp *cur = NIL;
    while(token.type == LPAREN || token.type == NUMBER)
    {
        ungetToken(token);
        lisp *tmp = parse_literal();

        if(list == NIL)
        {
            list = lisp_cons(tmp, NIL);
            cur = list;
        }
        else
        {
            cur->cdr = lisp_cons(tmp, NIL);
            cur = cur->cdr;
        }
        token = getToken();
    }


    ungetToken(token);
    return list;
    FUNC_END
}

int main(int argc, char *argv[])
{
    FUNC_BEG
    if (argc != 2)
    {
        printf("Error: Invalid number of arguments\n");
        exit(1);
    }

    parse_program(argv[1]);
    puts("Parsed OK");
    FUNC_END
}

void test_token()
{
    FUNC_BEG

    Token token;

    
    token = getToken();
    assert(token.type == LPAREN);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == RPAREN);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == QUOTE);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == STRING);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == VAR);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == NUMBER);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == WHILE);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == IF);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == SET);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == PRINT);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == CAR);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == CDR);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == CONS);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == PLUS);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == LENGTH);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == LESS);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == GREATER);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == EQUAL);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == NILL);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == EOF_TOKEN);
    ungetToken(token);

    
    token = getToken();
    assert(token.type == ERROR);
    ungetToken(token);

    FUNC_END
}

void test_parse_program_1() 
{
    FILE *file;
    char *filename = "test.txt";
    file = fopen(filename,"r");
    assert(file != NULL);
    fclose(file);
}

void test_parse_program_2() 
{
    FILE *file;
    char *filename = "invalid.txt";
    file = fopen(filename,"r");
    assert(file == NULL);
}

void test_parse_program_3() 
{
    char *filename = "test.txt";
    parse_program(filename);
    Token token = getToken();
    assert(token.type == EOF_TOKEN);


}