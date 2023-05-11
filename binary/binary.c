#include "binary.h"

Binary* Binary_constructor(size_t bufferSize) {
    Binary* binary = calloc(1, sizeof(Binary));

    binary->buffer = calloc(1, bufferSize);
    binary->bufferSize = bufferSize;
    binary->readIndex = 0;
    binary->writeIndex = 0;

    return binary;
}

void Binary_deconstructor(Binary* binary) {
    free(binary->buffer);
    free(binary);
}

size_t Binary_clear(Binary* binary) {
    free(binary->buffer);
    binary->buffer = calloc(1, binary->bufferSize);
    binary->writeIndex = 0;
    binary->readIndex = 0;
}

size_t Binary_getSize(Binary* binary) {
    return binary->writeIndex;
}

size_t Binary_getCapacity(Binary* binary) {
    return binary->bufferSize - binary->writeIndex;
}

size_t Binary_ensureCapacity(Binary* binary, size_t size) {
    if (size <= binary->bufferSize) {
        return binary->bufferSize;
    }

    char* buffer = calloc(1, size);
    memcpy(buffer, binary->buffer, binary->writeIndex);

    binary->buffer = buffer;
    binary->bufferSize = size;

    return size;
}

size_t Binary_maybeEnsureCapacity(Binary* binary, size_t writeSize) {
    return Binary_ensureCapacity(binary, Binary_getCapacity(binary) + writeSize);
}

void Binary_write(Binary* binary, void* value, size_t size) {
    Binary_maybeEnsureCapacity(binary, size);
    memcpy(&binary->buffer[binary->writeIndex], value, size);
    binary->writeIndex += size;
}

void Binary_read(Binary* binary, size_t size, void* outValue) {
    memcpy(outValue, &binary->buffer[binary->readIndex], size);
    binary->readIndex += size;
}

void Binary_writeBool(Binary* binary, bool value) {
    Binary_write(binary, &value, sizeof(bool));
}

void Binary_readBool(Binary* binary, bool* outValue) {
    Binary_read(binary, sizeof(bool), outValue);
}

void Binary_writeInt(Binary* binary, int value) {
    Binary_write(binary, &value, sizeof(int));
}

void Binary_readInt(Binary* binary, int* outValue) {
    Binary_read(binary, sizeof(int), outValue);
}

void Binary_writeSizeT(Binary* binary, size_t value) {
    Binary_write(binary, &value, sizeof(size_t));
}

void Binary_readSizeT(Binary* binary, size_t* outValue) {
    Binary_read(binary, sizeof(size_t), outValue);
}

void Binary_writeString(Binary* binary, char* value) {
    size_t valueSize = strlen(value) + 1;
    Binary_writeSizeT(binary, valueSize);
    Binary_write(binary, value, valueSize);
}

char* Binary_readString(Binary* binary, size_t* outSize) {
    Binary_readSizeT(binary, outSize);

    char* outString = calloc(1, *outSize);
    Binary_read(binary, *outSize, outString);

    return outString;
}
