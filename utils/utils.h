#ifndef UTILS_H
#define UTILS_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "math.h"
#include <assert.h>
#include <windows.h>

#include "../globals.h"
#include "../color/color.h"
#include "../unit/unit.h"
#include "../team/team.h"

void scanString(char* buffer, int maxCount);

//#region char numbers
#define IS_CHAR_DIGIT(value) value >= '0' && value <= '9'
#define ASSERT_CHAR_DIGIT(value) assert(value >= '0' && value <= '9')

unsigned char charToDigit(char value);
bool isNegativeCharNumber(const char* number);
bool isCharNumber(char* number);
char* absCharNumber(const char* number);
long long charNumberToDecimal(const char* charNumber);
long long charNumberToDecimalWithSize(const char* charNumber, size_t length);

size_t getNumberOfDigits(double value);
//#endregion

//#region print
void printfWithRGBColor(
        const RGB* backgroundColor,
        const RGB* foregroundColor,
        const char* format,
        ...
);

void printfWithColor(const WORD wAttributes, const char* format, ...);

void printTeams(
        Team** teams,
        size_t numberOfTeams,
        size_t numberOfUnitsInTeams,
        size_t currentTeamIndex,
        size_t selectedUnit,
        size_t enemyTeamIndex,
        size_t unitToAttack
);

void printSelectMenu(char** items, size_t itemsSize, size_t selectedIndex);

typedef struct SelectMenuOptionsStruct {
    size_t itemsSize;
    char** items;
    bool* lockedItems;

    RGB selectedColor;
    RGB selectedBackgroundColor;

    RGB lockedColor;
    RGB lockedBackgroundColor;

    RGB selectedLockedColor;
    RGB selectedLockedBackgroundColor;
} SelectMenuOptions;

void printSelectMenu2(SelectMenuOptions options, size_t selectedIndex);

#define INITIALIZE_SELECT_MENU(_itemsSize, _selectedIndex, ...) \
size_t _menuItemsSize = _itemsSize;\
size_t _menuSelectedIndex = _selectedIndex;              \
char* _menuItems[] = {__VA_ARGS__}

#define SELECT_MENU_BEHAVIOR() \
printSelectMenu(_menuItems, _menuItemsSize, _menuSelectedIndex);\
MOVE_CURSOR_UP(_menuItemsSize);\
char _menuInput = getch();\
if (_menuInput == 'w' || _menuInput == 72) {\
_menuSelectedIndex = _menuSelectedIndex == 0 ? _menuItemsSize - 1 : (_menuSelectedIndex - 1) % _menuItemsSize;\
}\
else if (_menuInput == 's' || _menuInput == 80) {\
_menuSelectedIndex = (_menuSelectedIndex + 1) % _menuItemsSize;\
}\
else if (_menuInput == '\n' || _menuInput == 13 || _menuInput == 100) \


#define SELECT_MENU_2_BEHAVIOR(_options) \
printSelectMenu2(_options, _menuSelectedIndex); \
MOVE_CURSOR_UP(_menuItemsSize);\
char _menuInput = getch();\
if (_menuInput == 'w' || _menuInput == 72) {\
_menuSelectedIndex = _menuSelectedIndex == 0 ? _menuItemsSize - 1 : (_menuSelectedIndex - 1) % _menuItemsSize;\
}\
else if (_menuInput == 's' || _menuInput == 80) {\
_menuSelectedIndex = (_menuSelectedIndex + 1) % _menuItemsSize;\
}\
else if (_menuInput == '\n' || _menuInput == 13 || _menuInput == 100)\

//#endregion


int attackUnit(Team* team, size_t unitIndex, Team* targetTeam, size_t targetIndex);


#endif //UTILS_H
