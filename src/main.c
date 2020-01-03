#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "parser.h"
#include "errors.h"
#include "symbol_table.h"
#include "instruction.h"

int NUM_ARGS[] = {
    2, 2, 1, 0, 1, 1,
    1, 1, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0
};

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
        printf("%s", ASM[code[i].op]);
        if (NUM_ARGS[code[i].op] == 1) {
            printf(" %d", code[i].q);
        } else if (NUM_ARGS[code[i].op] == 2) {
            printf(" %d %d", code[i].p, code[i].q);
        }
        printf("\n");
    }
    FILE* output = fopen("asm", "w");
    for (int i = 0; i < num_code; i++) {
        fprintf(output, "%s", ASM[code[i].op]);
        if (NUM_ARGS[code[i].op] == 1) {
            fprintf(output, " %d", code[i].q);
        } else if (NUM_ARGS[code[i].op] == 2) {
            fprintf(output, " %d %d", code[i].p, code[i].q);
        }
        fprintf(output, "\n");
    }
    fclose(output);
    return 0;
}
