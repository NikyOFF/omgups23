#include "team.h"

Team* Team_constructor(char* name, size_t unitsLength) {
    Team* team = calloc(2, sizeof(Team));

    char* nameCopy = calloc(strlen(name), sizeof(char));
    strcpy(nameCopy, name);

    team->name = nameCopy;
    team->unitsLength = unitsLength;
    team->units = calloc(unitsLength, sizeof(Unit*));
    team->flagsOfDeadUnits = 0;
    team->flagOfDefeat = 0;

    for (size_t index = 0; index < unitsLength; index++) {
        team->flagOfDefeat = team->flagOfDefeat | 1 << index;
        team->units[index] = Unit_constructor(1);
    }

    return team;
}

void Team_deconstructor(Team* team) {
    for (size_t index = 0; index < team->unitsLength; index++) {
        Unit_deconstructor(team->units[index]);
    }

    free(team->name);
    free(team->units);
    free(team);
}

bool Team_isDefeat(Team* team) {
    return team->flagsOfDeadUnits == team->flagOfDefeat;
}