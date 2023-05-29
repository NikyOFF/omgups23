#ifndef UNIT_H
#define UNIT_H
#include "stdlib.h"

typedef struct UnitStruct {
    size_t power;
} Unit;

Unit* Unit_constructor(size_t power);

void Unit_deconstructor(Unit* unit);

#endif //UNIT_H
