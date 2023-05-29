#ifndef TEAM_H
#define TEAM_H

#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "../unit/unit.h"

typedef struct TeamStruct {
    char* name;
    size_t unitsLength;
    Unit** units;
    size_t flagsOfDeadUnits;
    size_t flagOfDefeat;
} Team;

Team* Team_constructor(char* name, size_t unitsLength);

void Team_deconstructor(Team* team);

bool Team_isDefeat(Team* team);

#endif //TEAM_H
