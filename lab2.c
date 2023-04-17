/*
YAKOVLEV NIKOLAI, 22M, INDEX: 30
VARIANTS: 14, 6
*/

#include "lab2.h"


void main() {
    mainTask();
    task6();
}

int* randomIntArray(__OUT__ size_t* length) {
    srand(time(0)); //set random seed
    *length = FIXED_RANDOM_ARRAY_LENGTH_ENABLED ? FIXED_RANDOM_ARRAY_LENGTH : rand() % MAX_RANDOM_ARRAY_LENGTH; //set length for array
    int* array = calloc(*length, sizeof(int)); //allocate memory for array

    //fill array
    for (size_t index = 0; index < *length; index++) {
        int value = rand() % MAX_RANDOM_ARRAY_VALUE;

        array[index] = NEGATIVE_VALUES ? rand() % 2 == 1 ? value : -value : value;
    }

    return array;
}

void mainTask() {
    printf("Main task (variant 14)\n");
    //task1

    const double EXPECTED_RESULT = 1.08232;
    double result = 0;

    for (size_t n = 1; result < EXPECTED_RESULT - EPS; n++) {
        double temp = 1.0 / pow(n, 4);

        result += temp;
    }

    printf("current result  = %f\n", result);
    printf("expected result = %f\n", EXPECTED_RESULT);
    printf("error rate      = %.2f%%", (EXPECTED_RESULT - result) * 100);

    //task2
    /*
    * NOT IMPLEMENTED
    * idk math :D
    */
}

void task6() {
    printf("\n\nSubtask (variant 6)\n");

    size_t* arrayLength = malloc(sizeof(size_t)); //allocate memory for array length
    int* array = randomIntArray(arrayLength); //generate random int array

    size_t counter = 0; //define counter for count local mins

    for (size_t index = 0; index < *arrayLength; index++) {
        int arrayItem = array[index]; //get current array item by index

        //check loop start, end and common cases
        if (
            (index == 0 && arrayItem < array[index + 1]) ||
            (index == (*arrayLength) - 1 && arrayItem < array[index - 1]) ||
            (arrayItem < array[index - 1] && arrayItem < array[index + 1])
        ) {
            counter++;
            printf("%i - min\n", arrayItem);
            continue;
        }

        printf("%i\n", arrayItem);
    }

    printf("\ncount: %i\n", counter);
}