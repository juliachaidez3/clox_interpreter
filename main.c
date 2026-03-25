#include <stdio.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(void) {
  Chunk chunk;
  initChunk(&chunk);

  int c1 = addConstant(&chunk, 1.2);
  int c2 = addConstant(&chunk, 3.4);

  writeChunk(&chunk, OP_CONSTANT, 10);
  writeChunk(&chunk, (uint8_t)c1, 10);

  writeChunk(&chunk, OP_CONSTANT, 10);
  writeChunk(&chunk, (uint8_t)c2, 10);

  writeChunk(&chunk, OP_RETURN, 11);

  printf("code byte count: %d\n", chunk.count);
  printf("line run count: %d\n", chunk.lineCount);

  printLineRuns(&chunk);
  disassembleChunk(&chunk, "test chunk");

  freeChunk(&chunk);
  return 0;
}