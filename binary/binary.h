#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>

#ifndef BINARY_H
#define BINARY_H
typedef struct BinaryStruct {
    char* buffer;
    size_t bufferSize;
    size_t writeIndex;
    size_t readIndex;
} Binary;

Binary* Binary_constructor(size_t bufferSize);
void Binary_deconstructor(Binary* binary);

size_t Binary_clear(Binary* binary);
size_t Binary_getSize(Binary* binary);

size_t Binary_getCapacity(Binary* binary);
size_t Binary_ensureCapacity(Binary* binary, size_t size);
size_t Binary_maybeEnsureCapacity(Binary* binary, size_t writeSize);

void Binary_write(Binary* binary, void* value, size_t size);
void Binary_read(Binary* binary, size_t size, void* outValue);

void Binary_writeBool(Binary* binary, bool value);
void Binary_readBool(Binary* binary, bool* outValue);

void Binary_writeInt(Binary* binary, int value);
void Binary_readInt(Binary* binary, int* outValue);

void Binary_writeSizeT(Binary* binary, size_t value);
void Binary_readSizeT(Binary* binary, size_t* outValue);

void Binary_writeString(Binary* binary, char* value);
char* Binary_readString(Binary* binary, size_t* outSize);

#endif //BINARY_H
