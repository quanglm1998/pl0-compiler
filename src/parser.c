#include <string.h>
#include "scanner.h"
#include "errors.h"
#include "parser.h"
#include "symbol_table.h"
#include "instruction.h"

token_type_t token;

// returns 0 if it is a var or an array var
int term() {
    int flag = 0;
    flag |= factor();
    while (token == TIMES || token == SLASH || token == PERCENT) {
        flag = 1;
        token = get_token();
        factor();
    }
    return flag;
}

// returns 0 if it is a var or an array var
int expression() {
    int flag = 0;
    if (token == PLUS || token == MINUS) {
        token = get_token();
        flag = 1;
    }
    flag |= term();
    while (token == PLUS || token == MINUS) {
        flag = 1;
        token = get_token();
        term();
    }
    return flag;
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

// returns 0 if it is a var or an array var
int factor() {
    if (token == IDENT) {
        char old_id[MAX_IDENT_LEN + 1];
        memcpy(old_id, id, MAX_IDENT_LEN + 1);
        token = get_token();
        if (token == LBRACK) {
            token = get_token();
            expression();
            if (token != RBRACK) error("Expected ]");
            check(old_id, KIND_ARRAY);
            token = get_token();
            return 0;
        } else {
            check_var_or_const(old_id);
            int pos = get_location(old_id);
            if (main_table.symbol_stack[pos].kind == KIND_CONST) return 1;
            return 0;
        }
    } else if (token == NUMBER) {
        token = get_token();
    } else if (token == LPARENT) {
        token = get_token();
        expression();
        if (token != RPARENT) error("Expected )");
        token = get_token();
    } else error("Factor: syntax error");
    return 1;
}

void statement() {
    if (token == IDENT) {
        char old_id[MAX_IDENT_LEN + 1];
        memcpy(old_id, id, MAX_IDENT_LEN + 1);
        token = get_token();
        if (token == LBRACK) {
            token = get_token();
            expression();
            if (token != RBRACK) error("Expected ]");
            check(old_id, KIND_ARRAY);
            token = get_token();
        } else check(old_id, KIND_VAR);
        if (token != ASSIGN) error("Expected :=");
        token = get_token();
        expression();
    } else if (token == CALL) {
        token = get_token();
        if (token != IDENT) error("Expected an IDENT");
        check(id, KIND_PROCEDURE);
        int pos = get_location(id);
        int num_var = 0;
        token = get_token();
        if (token == LPARENT) {
            token = get_token();
            int f = expression();
            if (main_table.symbol_stack[pos].number_of_args < num_var + 1) error_detail(id, " has too many arguments!");
            int flag = ((main_table.symbol_stack[pos].flag >> num_var) & 1);
            if (flag && f) error("Reference argument must be a variable");
            num_var++;
            while (token == COMMA) {
                token = get_token();
                int f = expression();
                if (main_table.symbol_stack[pos].number_of_args < num_var + 1) error_detail(id, " has too many arguments!");
                int flag = ((main_table.symbol_stack[pos].flag >> num_var) & 1);
                if (flag && f) error("Reference argument must be a variable");
                num_var++;
            }
            if (token != RPARENT) error("Expected )");
            token = get_token();
        }
        if (main_table.symbol_stack[pos].number_of_args > num_var) error_detail(id, " has too few arguments!");
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
        check(id, KIND_VAR);
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

void block(int num_arg) {
    if (token == CONST) {
        token = get_token();
        while (1) {
            if (token != IDENT) error("Expected an IDENT");
            token = get_token();
            if (token != EQU) error("Expected =");
            token = get_token();
            if (token != NUMBER) error("Expected a NUMBER");
            int pos_in_table = enter_symbol(id, KIND_CONST);
            add_const_description(pos_in_table, num, num_arg);
            num_arg++;
            token = get_token();
            if (token != COMMA) break;
            token = get_token();
        }
        if (token != SEMICOLON) error("Expected ;");
        token = get_token();
    }
    int jump_id = add_instruction(OP_J, 0, 0);
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
                int pos_in_table = enter_symbol(id, KIND_ARRAY);
                add_var_description(pos_in_table, num_arg);
                num_arg += num;
                token = get_token();
            } else {
                int pos_in_table = enter_symbol(id, KIND_VAR);
                add_var_description(pos_in_table, num_arg);
                num_arg++;
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
        int pos = enter_symbol(id, KIND_PROCEDURE);
        main_table.symbol_stack[pos].pos_in_instruction = num_code;
        int num_var = 0;
        int flag = 0;
        enter_scope();
        token = get_token();
        if (token == LPARENT) {
            token = get_token();
            while (1) {
                int is_ref = 0;
                if (token == VAR) {
                    flag |= (1 << num_var);
                    token = get_token();
                    is_ref = 1;
                }
                if (token != IDENT) error("Expected an IDENT");
                int pos_in_table = enter_symbol(id, KIND_VAR);
                add_var_description(pos_in_table, num_var);
                main_table.symbol_stack[pos_in_table].is_ref = is_ref;
                num_var++;
                token = get_token();
                if (token != SEMICOLON) break;
                token = get_token();
            }
            if (token != RPARENT) error("Expected )");
            token = get_token();
        }
        if (token != SEMICOLON) error("Expected ;");
        add_num_var(pos, num_var, flag);
        token = get_token();
        block(num_var);
        if (token != SEMICOLON) error("Expected ;");
        exit_scope();
        token = get_token();
        add_instruction(OP_EP, 0, 0);
    }
    code[jump_id].q = num_code;
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
                block(0);
                if(token == PERIOD) {
                    printf("\n================\n");
                    printf("\nProgram parsed succesfully!\n");
                    add_instruction(OP_HL, 0, 0);
                } else error("Expected .");
            } else error("Expected ;");
        } else error("Expected an IDENT");
    } else error("Expected PROGRAM");
}