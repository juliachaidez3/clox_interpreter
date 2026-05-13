#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ,
    VAL_SMALL_STRING
} ValueType;

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)
#define AS_OBJ(value)     ((value).as.obj)

#define BOOL_VAL(value)   ((Value){VAL_BOOL,   {.boolean = value}})
#define NIL_VAL           ((Value){VAL_NIL,    {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)   ((Value){VAL_OBJ,    {.obj = (Obj*)object}})

#define SMALL_STRING_MAX 7

#define IS_SMALL_STRING(value) ((value).type == VAL_SMALL_STRING)

#define SMALL_STRING_VAL(text, len) makeSmallStringValue(text, len)

#define AS_SMALL_STRING(value) ((value).as.smallString.chars)
#define AS_SMALL_STRING_LENGTH(value) ((value).as.smallString.length)

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        Obj* obj;
        struct {
            uint8_t length;
            char chars[SMALL_STRING_MAX + 1];
        } smallString;
    } as;
} Value;

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);

void printValue(Value value);
bool valuesEqual(Value a, Value b);

Value makeSmallStringValue(const char* chars, int length);

#endif