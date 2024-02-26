#include <stdio.h>
#include <stdlib.h>

#include "monkey/lexer.h"
#include "monkey/token.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s file\n", argv[0]);
        exit(1);
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", argv[1]);
        exit(1);
    }

    Token* token = token_init();
    while (lexer_next(file, token)) {
        token_print(token);
    }
    token_free(token);

    return 0;
}
