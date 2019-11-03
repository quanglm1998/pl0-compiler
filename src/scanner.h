#include <stdio.h>

#define MAX_IDENT_LEN 10
#define NUM_KEYWORDS 15
#define MAX_NUMBER 100000000

typedef enum {//Các loại Token được sử dụng trong PL/0
    NONE=0, IDENT, NUMBER,
    BEGIN, CALL, CONST, DO,  ELSE, END, FOR, IF, ODD,
    PROCEDURE, PROGRAM, THEN, TO, VAR, WHILE,
    PLUS, MINUS, TIMES, SLASH, EQU, NEQ, LSS, LEQ, GTR, GEQ, LPARENT, RPARENT, LBRACK, RBRACK, PERIOD, COMMA, SEMICOLON,  ASSIGN, PERCENT
} TokenType;

TokenType token;    //Token nhận dạng được
int       num;      //Từ vựng khi Token là NUMBER
char id[MAX_IDENT_LEN + 1]; //Từ vựng khi Token là IDENT
int id_len;
FILE* src_file;
char last_char;

TokenType get_token();