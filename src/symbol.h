#ifndef SYMBOL_H
#define SYMBOL_H

#include "scanner.h"

typedef enum {
    KIND_VAR = 0, KIND_CONST, KIND_ARRAY, KIND_PROCEDURE
} kind_t;

typedef struct {
    kind_t kind;
    int level;
    char name[MAX_IDENT_LEN + 1];
    int number_of_args; // for proceduce
    int flag; // for procedue, 0 for value, 1 for reference
    int offset; // for var and array var, offset in value stack
    int is_ref; // for var and array var, 1 if it is ref
    int pos_in_instruction; // for proc
} symbol_t;

#endif