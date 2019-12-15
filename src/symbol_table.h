#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "symbol.h"

#define MAX_NUMBER_OF_VARS 100000

typedef struct {
    symbol_t symbol_stack[MAX_NUMBER_OF_VARS];
    int cur_level;
    int top;
} symbol_table_t;

symbol_table_t main_table;

void init_symbol(char *id);
int get_location(char *id);
int enter_var(char *id);
int enter_const(char *id);
int enter_array(char *id);
int enter_prod(char *id);
void enter_scope();
void exit_scope();
void add_num_var(int pos, int num_var, int flag);
void check_array(char *id);
void check_var(char *id);
void check_var_or_const(char *id);
void check_prod(char *id);

#endif
