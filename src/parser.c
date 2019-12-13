#include "scanner.h"
#include "errors.h"
#include "parser.h"

TokenType token;

void term() {
    factor();
    while (token == TIMES || token == SLASH || token == PERCENT) {
        token = get_token();
        factor();
    }
}

void expression() {
    if (token == PLUS || token == MINUS) token = get_token();
    term();
    while (token == PLUS || token == MINUS) {
        token = get_token();
        term();
    }
}

void condition() {
    if (token == ODD) {
        token = get_token();
        expression();
    } else {
        expression();
        if (token == EQU || token == NEQ || token == LSS || token == LEQ || token == GTR || token == GEQ) {
            token = get_token();
            expression();
        } else error("Condition: syntax error");
    }
}

void factor() {
    if (token == IDENT) {
        token = get_token();
        if (token == LBRACK) {
            token = get_token();
            expression();
            if (token != RBRACK) error("Expected ]");
            token = get_token();
        }
    } else if (token == NUMBER) {
        token = get_token();
    } else if (token == LPARENT) {
        token = get_token();
        expression();
        if (token != RPARENT) error("Expected )");
        token = get_token();
    } else error("Factor: syntax error");
}

void statement() {
    if (token == IDENT) {
        token = get_token();
        if (token == LBRACK) {
            token = get_token();
            expression();
            if (token != RBRACK) error("Expected ]");
            token = get_token();
        }
        if (token != ASSIGN) error("Expected :=");
        token = get_token();
        expression();
    } else if (token == CALL) {
        token = get_token();
        if (token != IDENT) error("Expected an IDENT");
        token = get_token();
        if (token == LPARENT) {
            token = get_token();
            expression();
            while (token == COMMA) {
                token = get_token();
                expression();
            }
            if (token != RPARENT) error("Expected )");
            token = get_token();
        }
    } else if (token == BEGIN) {
        token = get_token();
        statement();
        while (token == SEMICOLON) {
            token = get_token();
            statement();
        }
        if (token != END) error("Expected END");
        token = get_token();
    } else if (token == IF) {
        token = get_token();
        condition();
        if (token != THEN) error("Expected THEN");
        token = get_token();
        statement();
        if (token == ELSE) {
            token = get_token();
            statement();
        }
    } else if (token == WHILE) {
        token = get_token();
        condition();
        if (token != DO) error("Expected DO");
        token = get_token();
        statement();
    } else if (token == FOR) {
        token = get_token();
        if (token != IDENT) error("Expected an IDENT");
        token = get_token();
        if (token != ASSIGN) error("Expected :=");
        token = get_token();
        expression();
        if (token != TO) error("Expected TO");
        token = get_token();
        expression();
        if (token != DO) error("Expected DO");
        token = get_token();
        statement();
    }
}

void block() {
    if (token == CONST) {
        token = get_token();
        while (1) {
            if (token != IDENT) error("Expected an IDENT");
            token = get_token();
            if (token != EQU) error("Expected =");
            token = get_token();
            if (token != NUMBER) error("Expected a NUMBER");
            token = get_token();
            if (token != COMMA) break;
            token = get_token();
        }
        if (token != SEMICOLON) error("Expected ;");
        token = get_token();
    }
    if (token == VAR) {
        token = get_token();
        while (1) {
            if (token != IDENT) error("Expected an IDENT");
            token = get_token();
            if (token == LBRACK) {
                token = get_token();
                if (token != NUMBER) error("Expected a NUMBER");
                token = get_token();
                if (token != RBRACK) error("Expected ]");
                token = get_token();
            }
            if (token != COMMA) break;
            token = get_token();
        }
        if (token != SEMICOLON) error("Expected ;");
        token = get_token();
    }
    while (token == PROCEDURE) {
        token = get_token();
        if (token != IDENT) error("Expected an IDENT");
        token = get_token();
        if (token == LPARENT) {
            token = get_token();
            while (1) {
                if (token == VAR) token = get_token();
                if (token != IDENT) error("Expected an IDENT");
                token = get_token();
                if (token != SEMICOLON) break;
                token = get_token();
            }
            if (token != RPARENT) error("Expected )");
            token = get_token();
        }
        if (token != SEMICOLON) error("Expected ;");
        token = get_token();
        block();
        if (token != SEMICOLON) error("Expected ;");
        token = get_token();
    }
    if (token != BEGIN) error("Expected BEGIN");
    token = get_token();
    statement();
    while (token == SEMICOLON) {
        token = get_token();
        statement();
    }
    if (token != END) error("Expected END");
    token = get_token();
}

void program() {
    if (token == PROGRAM) {
        token = get_token();
        if (token == IDENT) {
            token = get_token();
            if (token == SEMICOLON) {
                token = get_token();
                block();
                if(token == PERIOD) {
                    printf("\n================\n");
                    printf("\nProgram parsed succesfully!\n");
                } else error("Expected .");
            } else error("Expected ;");
        } else error("Expected an IDENT");
    } else error("Expected PROGRAM");
}