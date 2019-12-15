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
} symbol_t;

#endif