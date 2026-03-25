#include <stdio.h>

#include "compiler.h"
#include "scanner.h"

bool compile(const char* source, Chunk* chunk) {
    (void)chunk;

    initScanner(source);

    for (;;) {
        Token token = scanToken();
        printf("%4d ", token.line);
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
        if (token.type == TOKEN_ERROR) return false;
    }

    return true;
}