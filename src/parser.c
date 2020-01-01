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
        token_type_t old_token = token;
        flag = 1;
        token = get_token();
        factor();
        if (old_token == TIMES) add_instruction(OP_MUL, 0, 0);
        else add_instruction(OP_DIV, 0, 0);
    }
    return flag;
}

// returns 0 if it is a var or an array var
int expression() {
    int flag = 0;
    int is_minus = 0;
    if (token == PLUS || token == MINUS) {
        token = get_token();
        flag = 1;
        is_minus = (token == MINUS);
    }
    flag |= term();
    if (is_minus) add_instruction(OP_NEG, 0, 0);
    while (token == PLUS || token == MINUS) {
        int is_minus = (token == MINUS);
        flag = 1;
        token = get_token();
        term();
        if (is_minus) add_instruction(OP_SUB, 0, 0);
        else add_instruction(OP_ADD, 0, 0);
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
            token_type_t old_token = token;
            token = get_token();
            expression();
            if (old_token == EQU) add_instruction(OP_EQ, 0, 0);
            else if (old_token == NEQ) add_instruction(OP_NE, 0, 0);
            else if (old_token == LSS) add_instruction(OP_LT, 0, 0);
            else if (old_token == LEQ) add_instruction(OP_LE, 0, 0);
            else if (old_token == GTR) add_instruction(OP_GT, 0, 0);
            else add_instruction(OP_GE, 0, 0);
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
            int pos = get_location(old_id);
            add_instruction(OP_LA, main_table.cur_level - main_table.symbol_stack[pos].level, main_table.symbol_stack[pos].offset);
            token = get_token();
            expression();
            if (token != RBRACK) error("Expected ]");
            check(old_id, KIND_ARRAY);
            add_instruction(OP_ADD, 0, 0);
            add_instruction(OP_LI, 0, 0);
            token = get_token();
            return 0;
        } else {
            check_var_or_const(old_id);
            int pos = get_location(old_id);
            add_instruction(OP_LV, main_table.cur_level - main_table.symbol_stack[pos].level, main_table.symbol_stack[pos].offset);
            if (main_table.symbol_stack[pos].is_ref) add_instruction(OP_LI, 0, 0);
            if (main_table.symbol_stack[pos].kind == KIND_CONST) return 1;
            return 0;
        }
    } else if (token == NUMBER) {
        add_instruction(OP_LC, 0, num);
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
        int ins_id = add_instruction(OP_LA, 0, 0);
        int p = 0, q = 0;
        if (token == LBRACK) {
            token = get_token();
            expression();
            if (token != RBRACK) error("Expected ]");
            check(old_id, KIND_ARRAY);
            add_instruction(OP_ADD, 0, 0);
            token = get_token();
        } else check(old_id, KIND_VAR);
        int pos_in_table = get_location(old_id);
        code[ins_id].p = main_table.cur_level - main_table.symbol_stack[pos_in_table].level;
        code[ins_id].q = main_table.symbol_stack[pos_in_table].offset;
        if (main_table.symbol_stack[pos_in_table].is_ref) {
            code[ins_id].op = OP_LV;
        }

        if (token != ASSIGN) error("Expected :=");
        token = get_token();
        expression();
        add_instruction(OP_ST, 0, 0);
    } else if (token == CALL) {
        token = get_token();
        if (token != IDENT) error("Expected an IDENT");
        check(id, KIND_PROCEDURE);
        int pos_old_id = get_location(id);
        add_instruction(OP_ADD, 0, 0);
        int pos = get_location(id);
        int num_var = 0;
        token = get_token();
        if (token == LPARENT) {
            token = get_token();
            int old_num_code = num_code;
            int f = expression();
            if (main_table.symbol_stack[pos].number_of_args < num_var + 1) error_detail(id, " has too many arguments!");
            int flag = ((main_table.symbol_stack[pos].flag >> num_var) & 1);
            if (flag && f) error("Reference argument must be a variable");
            if (flag) {
                num_code = old_num_code;
                int pos_in_table = get_location(id);
                add_instruction(OP_LA, main_table.cur_level - main_table.symbol_stack[pos_in_table].level, main_table.symbol_stack[pos_in_table].offset);
            }
            num_var++;
            while (token == COMMA) {
                token = get_token();
                old_num_code = num_code;
                int f = expression();
                if (main_table.symbol_stack[pos].number_of_args < num_var + 1) error_detail(id, " has too many arguments!");
                int flag = ((main_table.symbol_stack[pos].flag >> num_var) & 1);
                if (flag && f) error("Reference argument must be a variable");
                if (flag) {
                    num_code = old_num_code;
                    int pos_in_table = get_location(id);
                    add_instruction(OP_LA, main_table.cur_level - main_table.symbol_stack[pos_in_table].level, main_table.symbol_stack[pos_in_table].offset);
                }
                num_var++;
            }
            if (token != RPARENT) error("Expected )");
            token = get_token();
        }
        if (main_table.symbol_stack[pos].number_of_args > num_var) error_detail(id, " has too few arguments!");
        add_instruction(OP_DCT, 0, 4 + num_var);
        add_instruction(OP_CALL, main_table.cur_level - main_table.symbol_stack[pos_old_id].level + 1, main_table.symbol_stack[pos_old_id].pos_in_instruction);
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
        int ins_id_l1 = add_instruction(OP_FJ, 0, 0);
        if (token != THEN) error("Expected THEN");
        token = get_token();
        statement();
        if (token == ELSE) {
            int ins_id_l2 = add_instruction(OP_J, 0, 0);
            code[ins_id_l1].q = num_code;
            token = get_token();
            statement();
            code[ins_id_l2].q = num_code;
        } else {
            code[ins_id_l1].q = num_code;
        }
    } else if (token == WHILE) {
        int l1 = num_code;
        token = get_token();
        condition();
        int ins_id = add_instruction(OP_FJ, 0, 0);
        if (token != DO) error("Expected DO");
        token = get_token();
        statement();
        add_instruction(OP_J, 0, l1);
        code[ins_id].q = num_code;
    } else if (token == FOR) {
        token = get_token();
        if (token != IDENT) error("Expected an IDENT");
        check(id, KIND_VAR);
        int pos_in_table = get_location(id);
        if (main_table.symbol_stack[pos_in_table].is_ref) {
            add_instruction(OP_LV, main_table.cur_level - main_table.symbol_stack[pos_in_table].level, main_table.symbol_stack[pos_in_table].offset);
        } else {
            add_instruction(OP_LA, main_table.cur_level - main_table.symbol_stack[pos_in_table].level, main_table.symbol_stack[pos_in_table].offset);
        }
        add_instruction(OP_CV, 0, 0);
        token = get_token();
        if (token != ASSIGN) error("Expected :=");
        token = get_token();
        expression();
        int l1 = add_instruction(OP_ST, 0, 0);
        add_instruction(OP_CV, 0, 0);
        add_instruction(OP_LI, 0, 0);
        if (token != TO) error("Expected TO");
        token = get_token();
        expression();
        add_instruction(OP_LE, 0, 0);
        int ins_id = add_instruction(OP_FJ, 0, 0);
        if (token != DO) error("Expected DO");
        token = get_token();
        statement();
        add_instruction(OP_CV, 0, 0);
        add_instruction(OP_CV, 0, 0);
        add_instruction(OP_LI, 0, 0);
        add_instruction(OP_LC, 0, 1);
        add_instruction(OP_ADD, 0, 0);
        add_instruction(OP_ST, 0, 0);
        add_instruction(OP_J, 0, l1);
        code[ins_id].q = num_code;
        add_instruction(OP_DCT, 0, 1);
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
                add_var_description(pos_in_table, num_arg, num);
                num_arg += num;
                token = get_token();
            } else {
                int pos_in_table = enter_symbol(id, KIND_VAR);
                add_var_description(pos_in_table, num_arg, 1);
                num_arg++;
            }
            if (token != COMMA) break;
            token = get_token();
        }
        if (token != SEMICOLON) error("Expected ;");
        token = get_token();
    }
    int jump_id = add_instruction(OP_J, 0, 0);
    while (token == PROCEDURE) {
        token = get_token();
        if (token != IDENT) error("Expected an IDENT");
        int pos = enter_symbol(id, KIND_PROCEDURE);
        main_table.symbol_stack[pos].pos_in_instruction = num_code;
        int num_var = 0;
        int flag = 0;
        enter_scope();
        add_instruction(OP_INT, 0, 4);
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
                add_var_description(pos_in_table, num_var, 1);
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
                add_instruction(OP_INT, 0, 4);
                block(0);
                if(token == PERIOD) {
                    printf("\n================\n");
                    printf("\nProgram parsed succesfully!\n");

                    for (int i = 0; i < main_table.top; i++) {
                        symbol_t now = main_table.symbol_stack[i];
                        if (now.kind != KIND_VAR) continue;
                        add_instruction(OP_LV, now.level, now.offset);
                        add_instruction(OP_WRI, 0, 0);
                        add_instruction(OP_WLN, 0, 0);
                    }
                    add_instruction(OP_HLT, 0, 0);
                } else error("Expected .");
            } else error("Expected ;");
        } else error("Expected an IDENT");
    } else error("Expected PROGRAM");
}