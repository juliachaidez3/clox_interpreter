#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initValueArray(&chunk->constants);

  chunk->lineCount = 0;
  chunk->lineCapacity = 0;
  chunk->lines = NULL;
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(LineRun, chunk->lines, chunk->lineCapacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  chunk->count++;

  if (chunk->lineCount > 0 &&
      chunk->lines[chunk->lineCount - 1].line == line) {
    chunk->lines[chunk->lineCount - 1].count++;
      } else {
        if (chunk->lineCapacity < chunk->lineCount + 1) {
          int oldCapacity = chunk->lineCapacity;
          chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
          chunk->lines = GROW_ARRAY(LineRun, chunk->lines,
                                    oldCapacity, chunk->lineCapacity);
        }

        chunk->lines[chunk->lineCount].line = line;
        chunk->lines[chunk->lineCount].count = 1;
        chunk->lineCount++;
      }
}

int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

int getLine(Chunk* chunk, int instruction) {
  int current = 0;

  for (int i = 0; i < chunk->lineCount; i++) {
    current += chunk->lines[i].count;
    if (instruction < current) {
      return chunk->lines[i].line;
    }
  }

  return -1;
}