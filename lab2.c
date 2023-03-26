#include "lab2.h"


void main() {
    printf("Main task\n");

    double result = 0;

    for (size_t n = 0; result < END_VALUE - EPS; n++) {
        double temp = 1.0 / pow(2, n);

        result += temp;
        printf("[%zu]: temp = %f (1 / 2**%zu), result = %f\n", n, temp, n, result);
    }

    task7();
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

void task7() {
    printf("\n\ntask7\n");

    size_t* arrayLength = malloc(sizeof(size_t)); //allocate memory for array length
    int* array = randomIntArray(arrayLength); //generate random int array

    long int sum = 0;
    size_t counter = 0;

    for (size_t index = 0; index < *arrayLength; index++) {
        int arrayItem = array[index];

        if (arrayItem > sum) {
            counter++;
            printf("[%i]: %i (sum = %li)\n", index, arrayItem, sum);
        }
        else {
            printf("[%i]: %i\n", index, arrayItem);
        }

        sum += arrayItem;
    }

    printf("\ncount: %i\n", counter);
}