#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define MAX_CODE 1000000

typedef enum {
    OP_LA,
    OP_LV,
    OP_LC,
    OP_LI,
    OP_INT, 
    OP_DCT, 
    OP_J,
    OP_FJ,
    OP_HL,
    OP_ST,
    OP_CALL,
    OP_EP,
    OP_EF,
    OP_RC,
    OP_RI,
    OP_WRC,
    OP_WRI,
    OP_WLN,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEG,
    OP_CV,
    OP_EQ,
    OP_NE,
    OP_GT,
    OP_LT,
    OP_GE,
    OP_LE,
    OP_BP
} op_code_t;

typedef struct{
    op_code_t op;
    int p;
    int q;
} instruction_t;

instruction_t code[MAX_CODE];
int num_code = 0;

#endif