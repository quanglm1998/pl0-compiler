#include "instruction.h"
#include "symbol_table.h"

int num_code = 0;

const char* ASM[NUM_OP] = {
    "LA", "LV", "LC", "LI", "INT", "DCT",
    "J", "FJ", "HLT", "ST", "CALL", "EP", "EF",
    "RC", "RI", "WRC", "WRI", "WLN",
    "ADD", "SUB", "MUL", "DIV", "NEG", "CV",
    "EQ", "NE", "GT", "LT", "GE", "LE"
};

int add_instruction(op_code_t op, int p, int q) {
    code[num_code].op = op;
    code[num_code].p = p;
    code[num_code].q = q;
    return num_code++;
}

void add_const_description(int pos_in_table, int num, int num_arg) {
    main_table.symbol_stack[pos_in_table].offset = num_arg + 4;
    add_instruction(OP_LC, 0, num);
}

void add_var_description(int pos_in_table, int num_arg, int num) {
    main_table.symbol_stack[pos_in_table].offset = num_arg + 4;
    main_table.symbol_stack[pos_in_table].is_ref = 0;
    add_instruction(OP_INT, 0, num);
}
