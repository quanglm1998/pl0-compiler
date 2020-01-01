#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "parser.h"
#include "errors.h"
#include "symbol_table.h"
#include "instruction.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Please add source file\n");
        return 0;
    }
    src_file = fopen(argv[1], "r");
    
    row = 1;
    col = 0;
    last_char = ' ';
    main_table.cur_level = 0;
    main_table.top = 0;
    token = get_token();
    program();
    fclose(src_file);
    for (int i = 0; i < num_code; i++) {
        printf("%d %s %d %d\n", i, ASM[code[i].op], code[i].p, code[i].q);
    }

    // for (int i = 0; i < main_table.top; i++) {
    //     printf("Var #%d:\n", i);
    //     printf("\tkind = %d\n", main_table.symbol_stack[i].kind);
    //     printf("\tlevel = %d\n", main_table.symbol_stack[i].level);
    //     printf("\tname = %s\n", main_table.symbol_stack[i].name);
    //     printf("\tnum_arg = %d\n", main_table.symbol_stack[i].number_of_args);
    //     printf("\tflag = %d\n", main_table.symbol_stack[i].flag);
    //     printf("\toffset = %d\n", main_table.symbol_stack[i].offset);
    //     printf("\tis_ref = %d\n", main_table.symbol_stack[i].is_ref);
    //     printf("\tpos_in_instruction = %d\n", main_table.symbol_stack[i].pos_in_instruction);
    //     printf("\tvalue = %d\n", main_table.symbol_stack[i].value);
    // }
    return 0;
}
