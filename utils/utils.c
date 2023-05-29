#include "utils.h"


void scanString(char* buffer, int maxCount) {
    fgets(buffer, maxCount, stdin);
    buffer[strlen(buffer) - 1] = '\0';
}


//#region char numbers
unsigned char charToDigit(char value) {
    ASSERT_CHAR_DIGIT(value);

    return value - '0';
}

bool isNegativeCharNumber(const char* number) {
    return number[0] == '-';
}

bool isCharNumber(char* number) {
    char* currentChar = number;

    if (number[0] == '-') {
        currentChar++;
    }

    while(*currentChar) {
        if (!(IS_CHAR_DIGIT(*currentChar))) {
            return false;
        }

        currentChar++;
    }

    return true;
}

char* absCharNumber(const char* number) {
    size_t size = strlen(number);
    char* result;

    if (isNegativeCharNumber(number)) {
        result = calloc(size - 1, sizeof(char));

        for (size_t index = 0; index < size; index++) {
            result[index] = number[index + 1];
        }

        return result;
    }
    else {
        result = calloc(size, sizeof(char));
        strcpy(result, number);
    }

    return result;
}

long long charNumberToDecimal(const char* charNumber) {
    return charNumberToDecimalWithSize(charNumber, strlen(charNumber));
}

long long charNumberToDecimalWithSize(const char* charNumber, size_t length) {
    long long decimal = 0;

    bool isNegative = isNegativeCharNumber(charNumber);
    char* currentCharNumber = absCharNumber(charNumber);
    size_t currentLength = isNegative ? length - 1 : length;

    for (size_t index = 0; index < currentLength; index++) {
        size_t reverseIndex = currentLength - index - 1;

        unsigned char digit = charToDigit(currentCharNumber[reverseIndex]);

        decimal += digit * (index == 0 ? 1 : pow(10, index));
    }

    free(currentCharNumber);

    return isNegative ? -decimal : decimal;
}
//#endregion


//#region print
void printfWithRGBColor(
        const RGB* backgroundColor,
        const RGB* foregroundColor,
        const char* format,
        ...
) {
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    if (backgroundColor == NULL) {
        printf(
                "\x1b[38;2;%u;%u;%um",
                foregroundColor->red,
                foregroundColor->green,
                foregroundColor->blue
        );
    }
    else if (foregroundColor == NULL) {
        printf(
                "\x1b[48;2;%u;%u;%um",
                backgroundColor->red,
                backgroundColor->green,
                backgroundColor->blue
        );
    }
    else if (backgroundColor != NULL && foregroundColor != NULL) {
        printf(
                "\x1b[48;2;%u;%u;%um\x1b[38;2;%u;%u;%um",
                backgroundColor->red,
                backgroundColor->green,
                backgroundColor->blue,
                foregroundColor->red,
                foregroundColor->green,
                foregroundColor->blue
        );
    }

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    SetConsoleTextAttribute(consoleOutput, 7);
}

void printfWithColor(const WORD wAttributes, const char* format, ...) {
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(consoleOutput, wAttributes);
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    SetConsoleTextAttribute(consoleOutput, 7);
}

void printTeams(
        Team** teams,
        size_t numberOfTeams,
        size_t numberOfUnitsInTeams,
        size_t currentTeamIndex,
        size_t selectedUnit,
        size_t enemyTeamIndex,
        size_t unitToAttack
) {

    for (size_t teamIndex = 0; teamIndex < numberOfTeams; teamIndex++) {
        Team* team = teams[teamIndex];

        for (size_t unitIndex = 0; unitIndex < numberOfUnitsInTeams; unitIndex++) {
            Unit* unit = team->units[unitIndex];

            if (Team_isDefeat(team)) {
                printfWithRGBColor(
                        &DEFEAT_TEAM_UNIT_COLOR,
                        &WHITE_COLOR,
                        " [%llu] ",
                        unit->power
                );

                continue;
            }

            if (currentTeamIndex != -1 && currentTeamIndex == teamIndex) {
                printfWithRGBColor(
                        selectedUnit != -1 && selectedUnit == unitIndex ? &SELECTED_TEAM_UNIT_COLOR : &TEAM_UNIT_COLOR,
                        &WHITE_COLOR,
                        " [%llu] ",
                        unit->power
                );

                continue;
            }

            if (enemyTeamIndex != -1 && enemyTeamIndex == teamIndex) {
                printfWithRGBColor(
                        unitToAttack != -1 && unitToAttack == unitIndex ? &SELECTED_ENEMY_TEAM_UNIT_COLOR : &ENEMY_TEAM_UNIT_COLOR,
                        &WHITE_COLOR,
                        " [%llu] ",
                        unit->power
                );

                continue;
            }

            printf(" [%llu] ", unit->power);

        }

        if (currentTeamIndex != -1 && currentTeamIndex == teamIndex) {
            printfWithColor(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | 0x80, "   (%llu) Team \"%s\" \n\n", teamIndex, team->name);
        }
        else {
            printf("   (%llu) Team \"%s\" \n\n", teamIndex, team->name);
        }
    }

    printf("----------------------------------------------------------------------------------------------------------------------------\n\n");

}

void printSelectMenu(char** items, size_t itemsSize, size_t selectedIndex) {

    for (size_t index = 0; index < itemsSize; index++) {
        char* item = items[index];

        printf("%zu.", index+1);

        if (index == selectedIndex) {
            printfWithColor(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | 0x80, " %s ", item);
        }
        else {
            printf(" %s ", item);
        }

        printf("\n");
    }
}

void printSelectMenu2(SelectMenuOptions options, size_t selectedIndex) {
    for (size_t index = 0; index < options.itemsSize; index++) {
        char* item = options.items[index];

        printf("%zu.", index+1);

        if (index == selectedIndex) {

            if (options.lockedItems[index] == true) {
                printfWithRGBColor(
                        &options.selectedLockedBackgroundColor,
                        &options.selectedLockedColor,
                        " %s ",
                        item
                );
            }
            else {
                printfWithRGBColor(
                        &options.selectedBackgroundColor,
                        &options.selectedColor,
                        " %s ",
                        item
                );
            }
        }
        else if (options.lockedItems[index] == true) {
            printfWithRGBColor(
                    &options.lockedBackgroundColor,
                    &options.lockedColor,
                    " %s ",
                    item
            );
        }
        else {
            printf(" %s ", item);
        }

        printf("\n");
    }

}
//#endreigon

int attackUnit(Team* team, size_t unitIndex, Team* targetTeam, size_t targetIndex) {
    Unit* unit = team->units[unitIndex];
    Unit* target = targetTeam->units[targetIndex];

    if (unit->power == 0) {
        return 1;
    }

    if (target->power == 0) {
        return 2;
    }

    size_t attackPower = (rand() % unit->power) + 1;
    size_t protectionPower = (rand() % target->power) + 1;

    if (attackPower < protectionPower) {
        unit->power -= attackPower;
        target->power += attackPower;
    }
    else if (protectionPower < attackPower) {
        target->power -= protectionPower;
        unit->power += protectionPower;
    }
    else {
        size_t side = rand() % 2;

        if (side == 0) {
            unit->power -= attackPower;
            target->power += attackPower;
        }
        else {
            target->power -= protectionPower;
            unit->power += protectionPower;
        }
    }

    if (unit->power == 0) {
        team->flagsOfDeadUnits = team->flagsOfDeadUnits | 1 << unitIndex;
    }

    if (target->power == 0) {
        targetTeam->flagsOfDeadUnits = targetTeam->flagsOfDeadUnits | 1 << targetIndex;
    }

    return 0;
}
