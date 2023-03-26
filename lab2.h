#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


//helpers
#define __OUT__
#define __IN__

//boolean
typedef unsigned char Boolean;
#define TRUE 1
#define FALSE 0

//random array options
#define FIXED_RANDOM_ARRAY_LENGTH_ENABLED 0
#define FIXED_RANDOM_ARRAY_LENGTH 10
#define MAX_RANDOM_ARRAY_LENGTH 100
#define MAX_RANDOM_ARRAY_VALUE 10
#define NEGATIVE_VALUES FALSE


//constants
#define EPS 0.01
#define END_VALUE 2.0

//utils
double* randomDoubleArray(__OUT__ size_t* length);

//lab
void task7();