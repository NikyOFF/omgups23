#include "lab3.h"


int* copyArray(int* array, size_t length) {
    int* newArray = calloc(length, sizeof(int));

    for (size_t index = 0; index < length; index++) {
        newArray[index] = array[index];
    }

    return newArray;
}

int* selectionSort(int* array, size_t length) {
    int* sortedArray = copyArray(array, length);

    for (size_t i = 0; i < length - 1; i++) {
        size_t minIndex = i;

        for (size_t j = i + 1; j < length; j++) {
            if (sortedArray[minIndex] > sortedArray[j]) {
                minIndex = j;
            }
        }

        if (minIndex != i) {
            int temp = sortedArray[minIndex];
            sortedArray[minIndex] = sortedArray[i];
            sortedArray[i] = sortedArray[minIndex];
        }
    }

    return sortedArray;
}


void main() {
    size_t* arrayLength = malloc(sizeof(size_t)); //allocate memory for array length
    int* array = randomIntArray(arrayLength); //generate random int array

    // print unsorted array
    if (PRINT_UNSORTED_ARRAY) {
        printf("unsorted array:\n");
        for (size_t index = 0; index < *arrayLength; index++) {
            printf("[%i]: %i\n", index, array[index]);
        }
    }

    #pragma region start timer
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    #pragma endregion

    //sort array
    //merge sort: 334, 601, 425, 397, 401
    //selection sort: 1140, 1426, 1156, 1328, 1416
    int* sortedArray;

    switch(SORT_TIME) {
        case SORT_TYPE_MERGE:
            sortedArray = mergeSort(array, *arrayLength);
            break;
        case SORT_TYPE_SELECTION:
            sortedArray = selectionSort(array, *arrayLength);
            break;
        default:
            printf("Unsupported support type\n\n");
            exit(1);
            break;
    }

    #pragma region stop timer
    gettimeofday(&stop, NULL);
    
    if (PRINT_SORT_TIME) {
        printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec); 
    }
    #pragma region start timer


    //print sorted array
    if (PRINT_SORTED_ARRAY) {
        printf("\n\nsorted array:\n");
        for (size_t index = 0; index < *arrayLength; index++) {
            printf("[%i]: %i\n", index, sortedArray[index]);
        }
    }

    free(arrayLength); //free allocated memory for arrayLength
    free(array); //free allocated memory for array
    free(sortedArray); //free allocated memory for sortedArray


    //main task
    printf("\n\n");

    #define STRING_ARRAY_LENGTH 6

    char* stringArray[] = {
        "avtor",
        "tovar",
        "otvar",
        "apelsin",
        "spaniel",
        "toavr"
    };

    char** sortedStringArray = calloc(STRING_ARRAY_LENGTH, sizeof(stringArray));

    for (size_t index = 0; index < STRING_ARRAY_LENGTH; index++) {
        char* stringArrayItem = stringArray[index];
        size_t stringLength = getStringLength(stringArrayItem);

        sortedStringArray[index] = mergeSortForString(stringArrayItem, stringLength);
    }

    Boolean* checkedStrings = calloc(STRING_ARRAY_LENGTH, sizeof(Boolean));

    for (size_t index = 0; index < STRING_ARRAY_LENGTH; index++) {
        if (checkedStrings[index] == TRUE) {
            continue;
        }

        char* currentString = sortedStringArray[index];
        Boolean finded = FALSE;

        for (size_t internalIndex = index + 1; internalIndex < STRING_ARRAY_LENGTH; internalIndex++) {
            char* nextString = sortedStringArray[internalIndex];

            if (strcmp(currentString, nextString) == 0) {
                if (!finded) {
                    printf("%s", stringArray[index]);
                }

                printf(" -- %s", stringArray[internalIndex]);

                finded = TRUE;
                checkedStrings[internalIndex] = TRUE;
            }
        }

        if (finded) {
            printf(";\n");
        }
    }

    free(sortedStringArray);
}

#pragma region implementations
//initialize sub arrays helpter function
SubArrays* initializeSubArrays(size_t subArray1Length, size_t subArray2Length) {
    SubArrays *subArrays = malloc(sizeof(SubArrays)); //allocate memory for sturct
    subArrays->array1 = calloc(subArray1Length, sizeof(int)); //allocate memory for array1
    subArrays->array2 = calloc(subArray2Length, sizeof(int)); //allocate memory for array2
    subArrays->array1Length = subArray1Length; //set array1 length
    subArrays->array2Length = subArray2Length; //set array2 length

    return subArrays;
}

//free sub array helper function
void freeSubArrays(SubArrays* subArrays) {
    free(subArrays->array1); //free allocated memory for array1
    free(subArrays->array2); //free allocated memory for array2
    free(subArrays); //free allocated memory for subarrays struct
}

//split array to subarrays
SubArrays* splitArray(int* array, size_t length) {
    size_t subArray1Length = length / 2; //get array length for first array
    size_t subArray2Length = length - subArray1Length; //get array length for second array

    if (DEBUG) {
        printf("l1: %i, l2: %i\n\n", subArray1Length, subArray2Length);
    }

    //initialize sub arrays struct
    SubArrays *result = initializeSubArrays(subArray1Length, subArray2Length);

    if (DEBUG) {
        for (size_t index = 0; index < length; index++) {
            printf("split value [%i]: %i\n", index, array[index]);
        }
    }

    //indexes for fill subarrays
    size_t index1 = 0;
    size_t index2 = 0;

    //ful subarrays
    for (size_t index = 0; index < length; index++) {
        if (index < subArray1Length) {
            result->array1[index1] = array[index];
            index1++;
            continue;
        }

        result->array2[index2] = array[index];
        index2++;
    }

    return result;
}

//merge sort implementations
int* mergeSort(int* array, size_t length) {
    //check array length, if is equal to 1 - not need to sort
    if (length == 1) {
        return array;
    }

    //check array length, if is equal to 2 - maybe we need swap values
    if (length == 2) {
        //check values
        if (array[0] > array[1]) {
            //swap array values
            int value = array[0];
            array[0] = array[1];
            array[1] = value;
        }

        return array;
    }

    //if array length > 2 split array
    SubArrays* subArrays = splitArray(array, length);

    if (DEBUG) {
        for (size_t index = 0; index < subArrays->array1Length; index++) {
            printf("[1-%i]: %i\n", index, subArrays->array1[index]);
        }

        for (size_t index = 0; index < subArrays->array2Length; index++) {
            printf("[2-%i]: %i\n", index, subArrays->array2[index]);
        }

        printf("\n\n");
    }

    int* resultArray = calloc(length, sizeof(int)); //allocate memory for new array

    int* array1 = mergeSort(subArrays->array1, subArrays->array1Length); //recursive merge sort for first splited array
    int* array2 = mergeSort(subArrays->array2, subArrays->array2Length); //recursive merge sort for second splited array

    size_t array1Index = 0; //array1 index for check
    size_t array2Index = 0; //array2 index for check

    //sort arrays
    for (size_t index = 0; index < length; index++) {
        if (array1Index == subArrays->array1Length) {
            resultArray[index] = array2[array2Index];
            array2Index++;
            continue;
        }

        if (array2Index == subArrays->array2Length) {
            resultArray[index] = array1[array1Index];
            array1Index++;
            continue;
        }

        if (array1[array1Index] < array2[array2Index]) {
            resultArray[index] = array1[array1Index];
            array1Index++;
            continue;
        }

        if (array2[array2Index] < array1[array1Index]) {
            resultArray[index] = array2[array2Index];
            array2Index++;
            continue;
        }

        resultArray[index] = array1[array1Index];
        index++;
        resultArray[index] = array2[array2Index];

        array1Index++;
        array2Index++;
    }

    freeSubArrays(subArrays); //free allocated memory for sub arrays

    return resultArray;
}

char* mergeSortForString(char* string, size_t length) {
    int* intArray = calloc(length, sizeof(int)); //allocate memory for int array

    for (size_t index = 0; index < length; index++) {
        intArray[index] = 0 + string[index];
    }

    int* sortedIntArray = mergeSort(intArray, length);

    char* resultString = calloc(length, sizeof(char));
    resultString[length - 1] = '\n';

    for (size_t index = 0; index < length; index++) {
        resultString[index] = (char)(sortedIntArray[index]);
    }

    return resultString;
}

//generate random int array
int* randomIntArray(__OUT__ size_t* length) {
    srand(time(0)); //set random seed
    *length = FIXED_RANDOM_ARRAY_LENGTH_ENABLED ? FIXED_RANDOM_ARRAY_LENGTH : rand() % MAX_RANDOM_ARRAY_LENGTH; //set length for array
    int* array = calloc(*length, sizeof(int)); //allocate memory for array

    //fill array
    for (size_t index = 0; index < *length; index++) {
        array[index] = rand() % MAX_RANDOM_ARRAY_VALUE;
    }

    return array;
}

//get string length
size_t getStringLength(char* string) {
    size_t result = 0;

    for (; string[result] != '\0'; ++result);

    return result;
}
#pragma endregion
