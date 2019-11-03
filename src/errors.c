#include <stdlib.h>

#include "stdio.h"
#include "errors.h"

void error (const char msg[]) {
    printf("\n================\n");
    printf("\n\n%s", msg);
    printf(" at row %d, column %d", row, col);
    printf("\n");
    exit(0);
}