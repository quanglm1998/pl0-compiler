#include "scanner.h"
#include "errors.h"
#include "parser.h"

TokenType token;

void term() {
    factor();
    while (token == TIMES || token == SLASH) {
        token = get_token();
        factor();
    }
}

void expression() {
    if (token == PLUS || token == MINUS) {
        token = get_token();
    }
    term();
    while (token == PLUS || token == MINUS) {
        token = get_token();
        term();
    }
}

void factor() {
    if (token == IDENT) {
        token = get_token();
        if (token == LBRACK) {
            token = get_token();
            expression();
            if (token == RBRACK) {
                token = get_token();
            } else {
                error("Expected ]");
            }
        }
        return;
    }

    if (token == NUMBER) {
        token = get_token();
        return;
    }

    if (token == LPARENT) {
        token = get_token();
        expression();
        if (token == RPARENT) {
            token = get_token();
        } else {
            error("Expected )");
        }
        return;
    }

    error("Factor: syntax error");

}

void condition() {
    expression();
    if (token == EQU || token == NEQ || token == LSS ||   
        token == LEQ || token == GTR || token == GEQ) {
        token = get_token();
        expression();
    } else {
        error("Condition: syntax error");
    }
}

void statement() {
    if (token == IDENT) {
        token = get_token();
        if (token == LBRACK) {
            token = get_token();
            expression();
            if (token == RBRACK) token = get_token();
            else error("Expected ]");
        }
        if (token == ASSIGN) {
            token = get_token();
            expression();
        } else error("Expected := ");
        return;
    }

    if (token == CALL) {
        token = get_token();
        if (token == IDENT) {
            token = get_token();
            if (token == LPARENT) {
                token = get_token();
                expression();
                while (token == COMMA) {
                    token = get_token();
                    expression();
                }
                if (token == RPARENT) {
                    token = get_token();
                } else {
                    error("Expected )");
                }
            }
        } else {
            error("Call: expected an IDENT");
        }
        return;
    }

    if (token == BEGIN) {
        token = get_token();
        statement();
        while (token == SEMICOLON) {
            token = get_token();
            statement();
        }
        if (token != END) {
            error("Expected END");
        }
        token = get_token();
        return;
    }

    if (token == IF) {
        token = get_token();
        condition();
        if (token != THEN) {
            error("Expected THEN");
        }
        token = get_token();
        statement();
        if (token == ELSE) {
            token = get_token();
            statement();
        }
        return;
    }

    if (token == WHILE) {
        token = get_token();
        condition();
        if (token == DO) {
            token = get_token();
            statement();
        } else {
            error("Expected DO");
        }
        return;
    }

    if (token == FOR) {
        token = get_token();
        if (token != IDENT) {
            error("Expected an IDENT");
        }
        token = get_token();
        if (token != ASSIGN) {
            error("Expected ASSIGN");
        }
        token = get_token();
        expression();
        if (token != TO) {
            error("Expected TO");
        }
        token = get_token();
        expression();
        if (token != DO) {
            error("Expected DO");
        }
        token = get_token();
        statement();
        return;
    }
}

void do_block_type1() {
    if (token != IDENT) {
        error("Expected an IDENT");
    }
    token = get_token();
    if (token != EQU) {
        error("Expected =");
    }
    token = get_token();
    if (token != NUMBER) {
        error("Expected NUMBER");
    }
    token = get_token();
}

void do_block_type2() {
    if (token != IDENT) {
        error("Expected an IDENT");
    }
    token = get_token();
    if (token == LBRACK) {
        token = get_token();
        if (token != NUMBER) {
            error("Expected NUMBER");
        }
        token = get_token();
        if (token != RBRACK) {
            error("Expected ]");
        }
        token = get_token();
    }
}

void do_block_type3() {
    if (token == VAR) {
        token = get_token();
    }
    if (token != IDENT) {
        error("Expected an IDENT");
    }
    token = get_token();
}

void block() {
    if (token == CONST) {
        token = get_token();
        do_block_type1();
        while (token == COMMA) {
            token = get_token();
            do_block_type1();
        }
        if (token != SEMICOLON) {
            error("Expected ;");
        }
        token = get_token();
    }

    if (token == VAR) {
        token = get_token();
        do_block_type2();
        while (token == COMMA) {
            token = get_token();
            do_block_type2();
        }
        if (token != SEMICOLON) {
            error("Expected ;");
        }
        token = get_token();
    }

    while(token == PROCEDURE) {
        token = get_token();
        if (token != IDENT) {
            error("Expected an IDENT");
        }
        token = get_token();
        if (token == LPARENT) {
            token = get_token();
            do_block_type3();
            while (token == SEMICOLON) {
                token = get_token();
                do_block_type3();
            }
            if (token != RPARENT) {
                error("Expected )");
            }
            token = get_token();
        }

        if (token != SEMICOLON) {
            error("Expected ;");
        }
        token = get_token();
        block();
        if (token != SEMICOLON) {
            error("Expected ;");
        }
        token = get_token();
    }

    if (token == BEGIN) {
        token = get_token();
        statement();
        while (token == SEMICOLON) {
            token = get_token();
            statement();
        }
        if (token != END) {
            error("Expected END");
        }
        token = get_token();
        return;
    }
    error("Block: syntax error");
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
                } else error("expected .");

            } else error("Expected ;");

        } else error("Expected an IDENT");

    } else error("Expected PROGRAM");
}