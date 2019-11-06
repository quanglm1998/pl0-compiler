#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "errors.h"

const TokenType KEYWORDS[NUM_KEYWORDS] = {BEGIN, END, IF, THEN, WHILE, DO, CALL, ODD, TO, CONST, VAR, PROCEDURE, PROGRAM, ELSE, FOR};

char TOKEN_TO_TEXT[][20] = {
    "NONE", "IDENT", "NUMBER",
    "BEGIN", "CALL", "CONST", "DO",  "ELSE", "END", "FOR", "IF", "ODD",
    "PROCEDURE", "PROGRAM", "THEN", "TO", "VAR", "WHILE",
    "PLUS", "MINUS", "TIMES", "SLASH", "EQU", "NEQ", "LSS", "LEQ", "GTR", "GEQ", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "PERIOD", "COMMA", "SEMICOLON",  "ASSIGN", "PERCENT"
};

void get_next_char() {
    last_char = fgetc(src_file);
    col++;
    if (last_char == '\n') {
        row++;
        col = 0;
    }
    if (last_char != EOF) putchar(last_char);
    if (last_char != EOF && isalpha(last_char)) last_char = toupper(last_char);
}

int is_non_sense(char u) {
    return u == ' ' || u == '\t' || u == '\n' || u == '\r';
}

int get_keyword_id(char *keyword) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(keyword, TOKEN_TO_TEXT[KEYWORDS[i]]) == 0) return i;
    }
    return -1; // -1 means ident
}

TokenType get_token() {
    while (last_char != EOF && is_non_sense(last_char)) get_next_char();
    if (last_char == EOF) return NONE;

    if (isalpha(last_char)) {
        id_len = 0;
        while (last_char != EOF && (isalpha(last_char) || isdigit(last_char))) {
            if (id_len < MAX_IDENT_LEN) id[id_len] = last_char;
            id_len++;
            get_next_char();
        }
        id[id_len] = 0;
        int key_word_id = get_keyword_id(id);
        if (key_word_id >= 0) return KEYWORDS[key_word_id];
        return IDENT;
    }
    if (isdigit(last_char)) {
        num = 0;
        while (last_char != EOF && isdigit(last_char)) {
            num = num * 10 + (last_char - '0');
            if (num >= MAX_NUMBER) {
                error("The number is too big");
                return NONE;
            }
            get_next_char();
        }
        return NUMBER;
    }
    if (last_char == '(') {
        get_next_char();

        if (last_char == '*') {
            get_next_char();
            int found = 0;
            while (1) {
                if (last_char == EOF) break;
                if (last_char != '*' && last_char != ')') {
                    get_next_char();
                    found = 0;
                    continue;
                }
                if (last_char == '*') {
                    get_next_char();
                    found = 1;
                    continue;
                }
                get_next_char();
                if (found == 1) {
                    found = 2;
                    break;
                }
                found = 0;
            }
            if (found < 2) error("Expected end of comment *)");
            return get_token();
        }
        return LPARENT;
    }

    if (last_char == ':') {
        get_next_char();
        if (last_char != '=') {
            error("Expected = after :");
            return NONE;
        }
        get_next_char();
        return ASSIGN;
    }

    if (last_char == '<') {
        get_next_char();
        if (last_char == '=') return (get_next_char(), LEQ);
        if (last_char == '>') return (get_next_char(), NEQ);
        return LSS;
    }

    if (last_char == '>') {
        get_next_char();
        if (last_char == '=') return (get_next_char(), GEQ);
        return GTR;
    }
    if (last_char == '+') return (get_next_char(), PLUS);
    if (last_char == '-') return (get_next_char(), MINUS);
    if (last_char == '*') return (get_next_char(), TIMES);
    if (last_char == '/') return (get_next_char(), SLASH);
    if (last_char == '%') return (get_next_char(), PERCENT);
    if (last_char == '=') return (get_next_char(), EQU);
    if (last_char == ')') return (get_next_char(), RPARENT);
    if (last_char == '[') return (get_next_char(), LBRACK);
    if (last_char == ']') return (get_next_char(), RBRACK);
    if (last_char == ';') return (get_next_char(), SEMICOLON);
    if (last_char == ',') return (get_next_char(), COMMA);
    if (last_char == '.') return (get_next_char(), PERIOD);
    error("Weird character");
    return NONE;
}