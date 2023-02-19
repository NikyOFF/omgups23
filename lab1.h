#define NUMBER_SYSTEM 24 //29 % 15 + 10, NUMBER_SYSTEM only for 10+ number systems
#define MAX_DIGIT_IN_NUMBER_SYSTEM 25
#define MIN_DIGIT_IN_NUMBER_SYSTEM -25
#define MAX_CHAR_DIGIT_IN_NUMBER_SYSTEM ('A' + (NUMBER_SYSTEM - 10))
#define DIGIT_FORMAT "%d"
#define DECIMAL_FORMAT "%lli"
#define ASSERT_DIGIT(value) assert(value >= 0 && value < MAX_DIGIT_IN_NUMBER_SYSTEM)
#define ASSERT_CHAR_DIGIT(value) assert( (value >= '0' && value <= '9') || (value >= 'A' && value <= MAX_CHAR_DIGIT_IN_NUMBER_SYSTEM) )
#define TRUE 1
#define FALSE 0

#define DEBUG_MODE TRUE
#define OUTPUT_FILE_PATH "./output.txt"


#define BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(i) \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0')



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>


typedef char Boolean;
typedef long long Decimal;
typedef unsigned char Digit;

void debugMain();

char* copyString(char* string);
Boolean isNegativeNumber(long long number);
Boolean isNegativeCharNumber(char* number);
char* absCharNumber(char* number);
char* reverseString(char* string);
char* addMinus(char* string);
size_t numberLength(long long number);


char digitToChar(Digit value);
Digit charToDigit(char value);
char* decimalToCharNumber(Decimal decimal);
Decimal charNumberToDecimal(char* charNumber);
Decimal charNumberToDecimalWithSize(char* charNumber, size_t length);
char* sum(char* charNumber1, char* charNumber2);
char* subtract(char* charNumber1, char* charNumber2);
void printDigit(Digit digit);
void printDecimal(Decimal number);

char* getTableForNumberSystem();
