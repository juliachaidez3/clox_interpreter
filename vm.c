#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "value.h"
#include "vm.h"

VM vm;

static void resetStack(void) {
    vm.stackTop = vm.stack;
}

void initVM(void) {
    vm.stackCapacity = 8;
    vm.stack = GROW_ARRAY(Value, NULL, 0, vm.stackCapacity);
    vm.stackTop = vm.stack;
    vm.chunk = NULL;
    vm.ip = NULL;
}

void freeVM(void) {
    FREE_ARRAY(Value, vm.stack, vm.stackCapacity);
    vm.stack = NULL;
    vm.stackTop = NULL;
    vm.stackCapacity = 0;
}

InterpretResult interpret(const char* source) {
    Chunk chunk;
    initChunk(&chunk);

    bool ok = compile(source, &chunk);

    freeChunk(&chunk);
    resetStack();

    if (!ok) return INTERPRET_COMPILE_ERROR;
    return INTERPRET_OK;
}