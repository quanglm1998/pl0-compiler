#include <stdlib.h>

#include "stdio.h"
#include "errors.h"

void error(const char msg[]) {
    printf("\n================\n");
    printf("\n\n%s", msg);
    printf(" at row %d, column %d", row, col);
    printf("\n");
    exit(0);
}

void error_detail(const char id[], const char msg[]) {
    printf("\n================\n");
    printf("\n\nAt row %d, column %d: ", row, col);
    printf("%s%s\n", id, msg);
    exit(0);
}