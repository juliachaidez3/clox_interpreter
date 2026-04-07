#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "object.h"

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

typedef struct {
    Chunk* chunk;
    uint8_t* ip;

    Value* stack;
    int stackCapacity;
    Value* stackTop;

    Obj* objects;
} VM;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);

extern VM vm;

#endif