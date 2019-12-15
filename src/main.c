#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "parser.h"
#include "errors.h"
#include "symbol_table.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Please add source file\n");
        return 0;
    }
    src_file = fopen(argv[1], "r");
    
    row = 1;
    col = 0;
    last_char = ' ';
    token = get_token();
    program();
    main_table.cur_level = 0;
    main_table.top = 0;
    fclose(src_file);
    return 0;
}
