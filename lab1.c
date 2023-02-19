#include "lab1.h"

void main() {
    FILE* file = fopen(OUTPUT_FILE_PATH, "w+");

    assert(file != NULL);

    fprintf(file, getTableForNumberSystem());
    fclose(file);
}


#pragma region common

char* getTableForNumberSystem() {
    char* finalBuffer = calloc(2048, sizeof(char));
    char numberSystems[] = { 10, 2, 16, NUMBER_SYSTEM };

    for (char decimal = 0; decimal < 26; decimal++) {
        char* buffer = calloc(2048, sizeof(char));

        for (char numberSystemIndex = 0; numberSystemIndex < 4; numberSystemIndex++) {
            char* cellBuffer = calloc(100, sizeof(char)); //allocate memory for cell buffer
            char* charNumber; //need for later allocate memory

            //convert decimal to another number system behavior
            switch(numberSystems[numberSystemIndex]) {
                case 10:
                    charNumber = calloc(numberLength(decimal), sizeof(char)); //allocate memory for decimal
                    sprintf(charNumber, "%lli", decimal); //format number
                    break;
                case 2:
                    charNumber = calloc(20, sizeof(char)); //allocate memory for hex
                    sprintf(charNumber, BINARY_PATTERN, BYTE_TO_BINARY(decimal)); //format decimal as binary
                    break;
                case 16:
                    charNumber = calloc(numberLength(decimal), sizeof(char)); //allocate memory for hex
                    sprintf(charNumber, "%x", decimal); //format decimal as hex
                    break;
                case NUMBER_SYSTEM:
                    charNumber = decimalToCharNumber(decimal); //convert decimal to char number
            }

            sprintf(cellBuffer, "%10s", charNumber); //align by right
            strcat(buffer, cellBuffer); //concat buffers

            // free(charNumber); ¯\_(ツ)_/¯ idk
            free(cellBuffer); //free cell buffer
        }

        strcat(buffer, "\n"); //endline
        strcat(finalBuffer, buffer); //concat buffers
        free(buffer); //free buffer
    }

    return finalBuffer;
}

#pragma endregion


#pragma region debug
void testNegativeDigit() {
    ASSERT_DIGIT(-0);
    ASSERT_DIGIT(-1);
}

void testDigits() {
    ASSERT_DIGIT(0);
    ASSERT_DIGIT(10);
    ASSERT_DIGIT(23);
    ASSERT_DIGIT(24);
    ASSERT_DIGIT(25);
}

void testCharDigits() {
    ASSERT_CHAR_DIGIT('0');
    ASSERT_CHAR_DIGIT('9');
    ASSERT_CHAR_DIGIT('A');
    ASSERT_CHAR_DIGIT(MAX_CHAR_DIGIT_IN_NUMBER_SYSTEM + 1);
}

void testInvalidCharDigits() {
    ASSERT_CHAR_DIGIT('-');
}

void testCompliance() {
    assert(numberLength(0) == 1);
    assert(numberLength(9) == 1);
    assert(numberLength(10) == 2);
    assert(numberLength(100) == 3);
    assert(numberLength(1000) == 4);
    assert(numberLength(10000) == 5);

    assert(digitToChar(1) == '1');
    assert(digitToChar(9) == '9');
    assert(digitToChar(10) == 'A');
    assert(digitToChar(NUMBER_SYSTEM) == MAX_CHAR_DIGIT_IN_NUMBER_SYSTEM);
}

void debugMain() {
    // testDigits();
    // testNegativeDigit();
    // testCharDigits();
    // testInvalidCharDigits();
    testCompliance();
}
#pragma endregion



#pragma region realisation
//copy string
char* copyString(char* string) {
    size_t size = strlen(string); //current string length
    char* result = calloc(size, sizeof(char)); //allocate memory for result string

    //copy string by char to char indexes
    for (size_t index = 0; index < size; index++) {
        result[index] = string[index];
    }

    //return result
    return result;
}

//check number is negative
Boolean isNegativeNumber(long long number) {
    // if number < 0 is negative number
    return number < 0 ? TRUE : FALSE;
}

//check number char is negative
Boolean isNegativeCharNumber(char* number) {
    //if first char in string is '-' is negative char number
    return number[0] == '-' ? TRUE : FALSE;
}

//need free number
char* absCharNumber(char* number) {
    //check is negative char number
    if (isNegativeCharNumber(number)) {
        size_t size = strlen(number); //current string length
        char* result = calloc(size - 1, sizeof(char)); //allocate memory for result string, -1 because we need remove first char (-)
        
        //copy string by char to char with 1 offset
        for (size_t index = 0; index < size; index++) {
            result[index] = number[index + 1];
        }

        return result;
    }

    //return copy because we need same behavior for any cases
    return copyString(number);
}

//need free string
char* reverseString(char* string) {
    size_t size = strlen(string); //current string length
    char* result = calloc(size, sizeof(char)); //allocate memory for result string
    
    //copy string by char to char with reverse index (size - index - 1)
    for (size_t index = 0; index < size; index++) {
        result[index] = string[size - index - 1];
    }

    return result;
}

//need free string
char* addMinus(char* string) {
    size_t size = strlen(string) + 1; //new string length, +1 for '-' char
    char* result = calloc(size, sizeof(char)); //allocate memory for result
    result[0] = '-'; //set first char as '-'
    
    //copt string by char to char with -1 offset
    for (size_t index = 1; index < size; index++) {
        result[index] = string[index - 1];
    }

    return result;
}

//get number length
size_t numberLength(long long number) {
    //log10(0) it`s infinity, return 1
    //+1 because log10 for numbers less 10 is 0
    return number == 0 ? 1 : log10(abs(number)) + 1;
}


//convert digit to char
char digitToChar(Digit digit) {
    ASSERT_DIGIT(digit); //asset if digit is not belong current numbet system

    return digit > 9 ? 'A' + (digit - 10) : '0' + digit;
}

//convert char to digit
Digit charToDigit(char value) {
    ASSERT_CHAR_DIGIT(value); //asset if char digit is not belong current numbet system

    if (value <= '9') {
        return value - '0';
    }

    return (value - 'A') + 10;
}

//convert decimal number to char number
char* decimalToCharNumber(Decimal decimal) {
    //if decimal is zero return zero
    if (decimal == 0) {
        return "0";
    }

    Decimal currentDecimal = abs(decimal); //current absolute decimal
    size_t size = numberLength(currentDecimal); //current number length
    char* buffer = calloc(size, sizeof(char)); //allocate memory for char number
    size_t index = 0;

    //fill buffer
    while (currentDecimal > 0) {
        buffer[index++] = digitToChar(currentDecimal % NUMBER_SYSTEM); //get current decimal in number system and convert at to digit
        currentDecimal /= NUMBER_SYSTEM; //divide the number by number system
    }

    //if decimal is negative
    if (isNegativeNumber(decimal)) {
        char* reversedString = reverseString(buffer); //reverse string, function return new allocated memory
        char* negativeCharNumberString = addMinus(reversedString); //add minus to reversed string, function return new allocated memory

        free(buffer); //free buffer, because allocated new
        free(reversedString); //free reversed string, because allocated new

        //return allocated negative char number string
        return negativeCharNumberString;
    }

    char* reversedString = reverseString(buffer); //reverse string, function return new allocated memory
    free(buffer); //free buffer, because allocated new

    //return reversed string
    return reversedString;
}

//wrapper for convert char number  to decimal function without size arguemnt
Decimal charNumberToDecimal(char* charNumber) {
    //pass string length argument
    return charNumberToDecimalWithSize(charNumber, strlen(charNumber));
}

//convert char number to decimal
Decimal charNumberToDecimalWithSize(char* charNumber, size_t length) {
    //main idea
    /*
        convert current char to digit and multiple to discharge
        for value 3GC (2124), size: 3
        C = (12) * (  1 ) = 12
        G = (16) * (24^1) = 384
        3 = (3)  * (24^2) = 1728
        result: 2124
    */

    Decimal decimal = 0; //result container

    Boolean isNegative = isNegativeCharNumber(charNumber); //check is negative
    char* currentCharNumber = absCharNumber(charNumber); //for simple abs char number in any cases
    size_t currentLength = isNegative ? length - 1 : length; //get current char nubmer length, if current length for negative value need -1

    //convert behavior
    for (size_t index = 0; index < currentLength; index++) {
        size_t reverseIndex = currentLength - index - 1; //reversed index

        Digit digit = charToDigit(currentCharNumber[reverseIndex]); //convert current char to digit

        decimal += digit * (index == 0 ? 1 : pow(NUMBER_SYSTEM, index)); //multiple to discharge
    }

    //free abs char number
    free(currentCharNumber);

    //if is negative return negative result else default
    return isNegative ? -decimal : decimal;
}

char* sum(char* charNumber1, char* charNumber2) {
    return decimalToCharNumber(charNumberToDecimal(charNumber1) + charNumberToDecimal(charNumber2));
}

char* subtract(char* charNumber1, char* charNumber2) {
    return decimalToCharNumber(charNumberToDecimal(charNumber1) - charNumberToDecimal(charNumber2));
}

//print digit in current system
void printDigit(Digit digit) {
    ASSERT_DIGIT(digit); //asset if digit not belongs to current number system

    printf("%c\n", digitToChar(digit)); //convert digit to char and print
}

//print decumal number in current system
void printDecimal(Decimal decimal) {
    char* charNumber = decimalToCharNumber(decimal); //convert decimal to char number
    char* walker = charNumber; //walker for while loop

    //walk on chars
    while (*walker) {
        printf("%c", *walker); //print char
        walker++; //next walk
    }

    free(charNumber); //free allocated 
    printf("\n"); //print next line
}
#pragma endregion
