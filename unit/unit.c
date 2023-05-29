#include "unit.h"

Unit* Unit_constructor(size_t power) {
    Unit* unit = calloc(1, sizeof(Unit));

    unit->power = power;

    return unit;
}

void Unit_deconstructor(Unit* unit) {
    free(unit);
}