#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

//helpers
#define __OUT__
#define __IN__

//boolean
typedef unsigned char Boolean;
#define TRUE 1
#define FALSE 0

#define SORT_TYPE_MERGE 0
#define SORT_TYPE_SELECTION 1

//lab options
#define DEBUG FALSE
#define PRINT_UNSORTED_ARRAY TRUE
#define PRINT_SORTED_ARRAY TRUE
#define PRINT_SORT_TIME TRUE

#define SORT_TIME SORT_TYPE_MERGE

#define VARIANT 15
Boolean SORT_ASCENDING = VARIANT & 0x1;
Boolean SORT_FROM_THE_END = VARIANT & 0x01;


//random array options
#define MAX_RANDOM_ARRAY_VALUE 100

#define MAX_RANDOM_ARRAY_LENGTH 100
#define FIXED_RANDOM_ARRAY_LENGTH_ENABLED TRUE
#define FIXED_RANDOM_ARRAY_LENGTH 100

typedef struct SubArraysStruct {
    int* array1;
    int* array2;
    size_t array1Length;
    size_t array2Length;
} SubArrays;

//utils
SubArrays* initializeSubArrays(size_t subArray1Length, size_t subArray2Length);
void freeSubArrays(SubArrays* subArrays);

SubArrays* splitArray(int* array, size_t length);

int* copyArray(int* array, size_t length);
int* randomIntArray(__OUT__ size_t* length);

size_t getStringLength(char* string);

//sort
int* mergeSort(int* array, size_t length);
int* selectionSort(int* array, size_t length);

char* mergeSortForString(char* array, size_t length);
