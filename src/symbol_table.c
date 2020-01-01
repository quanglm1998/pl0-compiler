#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "symbol_table.h"
#include "errors.h"

const char KIND_TO_TEXT[][20] = {
    "variable", "constant", "array value", "proceduce"
};

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

int enter_symbol(char *id, kind_t kind) {
    init_symbol(id);
    main_table.symbol_stack[main_table.top].kind = kind;
    main_table.top++;
    return main_table.top - 1;
}

// add number of var to proceduce
void add_num_var(int pos, int num_var, int flag) {
    main_table.symbol_stack[pos].number_of_args = num_var;
    main_table.symbol_stack[pos].flag = flag;
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

void check(char *id, kind_t kind) {
    int pos = get_location(id);
    if (pos == -1) error_detail(id, " has not been declared!");
    if (main_table.symbol_stack[pos].kind != kind) {
        printf("\n\n===================\n");
        printf("\n%s is not a/an %s\n", id, KIND_TO_TEXT[kind]);
        exit(0);
    }
}

void check_var_or_const(char *id) {
    int pos = get_location(id);
    if (pos == -1) error_detail(id, " has not been declared!");
    if (main_table.symbol_stack[pos].kind == KIND_PROCEDURE) error_detail(id, " is a proceduce!");
    if (main_table.symbol_stack[pos].kind == KIND_ARRAY) error_detail(id, " is an array!");
}

