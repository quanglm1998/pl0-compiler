#include <string.h>
#include "symbol.h"
#include "symbol_table.h"
#include "errors.h"

// return -1 if ident has not declared yet
int get_location(char *id) {
    for (int i = main_table.top - 1; i >= 0; i--) {
        if (strcmp(id, main_table.symbol_stack[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

void init_symbol(char *id) {
    int pos = get_location(id);
    if (pos != -1 && main_table.symbol_stack[pos].level == main_table.cur_level) {
        error_detail(id, " <-- has been already declared in this scope!");
    }
    memcpy(main_table.symbol_stack[main_table.top].name, id, MAX_IDENT_LEN + 1);
    main_table.symbol_stack[main_table.top].level = main_table.cur_level;
}

int enter_var(char *id) {
    init_symbol(id);
    main_table.symbol_stack[main_table.top].kind = KIND_VAR;
    main_table.top++;
    return main_table.top - 1;
}

int enter_const(char *id) {
    init_symbol(id);
    main_table.symbol_stack[main_table.top].kind = KIND_CONST;
    main_table.top++;
    return main_table.top - 1;
}

int enter_array(char *id) {
    init_symbol(id);
    main_table.symbol_stack[main_table.top].kind = KIND_ARRAY;
    main_table.top++;
    return main_table.top - 1;
}

int enter_prod(char *id) {
    init_symbol(id);
    main_table.symbol_stack[main_table.top].kind = KIND_PROCEDURE;
    main_table.top++;
    return main_table.top - 1;
}

void add_num_var(int pos, int num_var) {
    main_table.symbol_stack[pos].number_of_args = num_var;
}

void enter_scope() {
    main_table.cur_level++;
}

void exit_scope() {
    while (main_table.top > 0 && main_table.symbol_stack[main_table.top - 1].level == main_table.cur_level) {
        main_table.top--;
    }
    main_table.cur_level--;
}

void check_var(char *id) {
    int pos = get_location(id);
    if (pos == -1) error_detail(id, " has not been declared!");
    if (main_table.symbol_stack[pos].kind != KIND_VAR) error_detail(id, " is not a variable!");
}

void check_array(char *id) {
    int pos = get_location(id);
    if (pos == -1) error_detail(id, " has not been declared!");
    if (main_table.symbol_stack[pos].kind != KIND_ARRAY) error_detail(id, " is not an array!");
}

void check_prod(char *id) {
    int pos = get_location(id);
    if (pos == -1) error_detail(id, " has not been declared!");
    if (main_table.symbol_stack[pos].kind != KIND_PROCEDURE) error_detail(id, " is not a proceduce!");
}

void check_var_or_const(char *id) {
    int pos = get_location(id);
    if (pos == -1) error_detail(id, " has not been declared!");
    if (main_table.symbol_stack[pos].kind == KIND_PROCEDURE) error_detail(id, " is a proceduce!");
    if (main_table.symbol_stack[pos].kind == KIND_ARRAY) error_detail(id, " is an array!");
}

