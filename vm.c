#include <stdio.h>

#include "compiler.h"
#include "memory.h"
#include "value.h"
#include "vm.h"

static VM vm;

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

static void push(Value value) {
  int count = (int)(vm.stackTop - vm.stack);

  if (count >= vm.stackCapacity) {
    int oldCapacity = vm.stackCapacity;
    vm.stackCapacity = GROW_CAPACITY(oldCapacity);
    vm.stack = GROW_ARRAY(Value, vm.stack, oldCapacity, vm.stackCapacity);
    vm.stackTop = vm.stack + count;
  }

  *vm.stackTop = value;
  vm.stackTop++;
}

static Value pop(void) {
  vm.stackTop--;
  return *vm.stackTop;
}

static InterpretResult run(void) {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)                 \
  do {                                \
    double b = pop();                 \
    double a = pop();                 \
    push(a op b);                     \
  } while (false)

  for (;;) {
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }

      case OP_RETURN: {
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      }

      case OP_NEGATE:
        vm.stackTop[-1] = -vm.stackTop[-1];
        break;

      case OP_ADD:
        BINARY_OP(+);
        break;

      case OP_SUBTRACT:
        BINARY_OP(-);
        break;

      case OP_MULTIPLY:
        BINARY_OP(*);
        break;

      case OP_DIVIDE:
        BINARY_OP(/);
        break;
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    resetStack();
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  freeChunk(&chunk);
  resetStack();
  return result;
}