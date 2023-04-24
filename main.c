#include "main.h"
#include <stdio.h>
#include <stdarg.h>


#pragma region unit
Unit* Unit_constructor(Power power) {
    Unit* unit = malloc(sizeof(Unit));

    unit->power = power;

    return unit;
}

Unit* Unit_createRandom(Power minPower, Power maxPower) {
    srand(time(0));

    Power fixedMinPower = minPower == 0 ? 1 : minPower;
    Power randomPower = (rand() % maxPower == 0 ? 1 : maxPower);

    Unit* unit = malloc(sizeof(Unit));
        

    unit->power = randomPower < fixedMinPower ? fixedMinPower : randomPower;

    return unit;
}
#pragma endregion

#pragma region game context
GameContext* GameContext_constructor(Scene scene) {
    assert(isValidScene(scene) == TRUE, "[GameContext::constructor] invalid scene "SCENE_FORMAT, scene);

    GameContext* gameContext = malloc(sizeof(GameContext));

    gameContext->version = GAME_VERSION;
    gameContext->state = ProcessGameState;
    gameContext->scene = scene;

    return gameContext;
}

void GameContext_setScene(GameContext *gameContext, Scene scene) {
    assert(isValidScene(scene) == TRUE, "[GameContext::setScene] invalid scene "SCENE_FORMAT, scene);

    gameContext->scene = scene;
}

//free after use
char* GameContext_debug_string() {
    char* debugString = calloc(2048, sizeof(char));

    sprintf(debugString, "scene: \"%s\" ("SCENE_FORMAT")", getSceneName(GAME_CONTEXT.scene), GAME_CONTEXT.scene);

    return debugString;
}

#pragma endregion

#pragma region utils
void slice(const char *str, char *result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
}

//free after use
char* timestamp() {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    char* raw = calloc(7, sizeof(char));

    slice(asctime(timeinfo), raw, 11, 19);

    return raw;
}

void debug(const char* format, ...) {
    if (DEBUG_MODE == FALSE) {
        return;
    }

    va_list args;
    char* currentTimestamp = timestamp();
    fprintf(stderr, "[%s] [Game]: ", currentTimestamp);
    free(currentTimestamp);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void warn(const char* format, ...) {
    if (DEBUG_MODE == FALSE) {
        return;
    }

    va_list args;
    char* currentTimestamp = timestamp();
    fprintf(stderr, "[%s] [Warn]: ", currentTimestamp);
    free(currentTimestamp);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void assert(Boolean condition, const char* format, ...) {
    if (condition == TRUE) {
        return;
    }

    va_list args;
    char* currentTimestamp = timestamp();
    fprintf(stderr, "[%s] [Error]: ", currentTimestamp);
    free(currentTimestamp);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");

    exitGame(ERROR_EXIT_REASON, TRUE);
}


char* getExitReasonName(ExitReason exitReason) {
    switch(exitReason) {
        case ERROR_EXIT_REASON:
            return "error";
        case GAME_STOPED_EXIT_REASON:
            return "game stoped";
        default:
            return "other";
    }
}


Boolean isValidScene(Scene scene) {
    switch(scene) {
        case MAIN_MENU_SCENE:
        case GAME_PROCESS_SCENE:
            return TRUE;
    }

    return FALSE;
}

char* getSceneName(Scene scene) {
    assert(isValidScene(scene) == TRUE, "[getSceneName] invalid scene "SCENE_FORMAT, scene);

    switch(scene) {
        case MAIN_MENU_SCENE:
        return "main menu";
        case GAME_PROCESS_SCENE:
            return "game process";
    }
}
#pragma endregion

#pragma region game
void saveGame() {
    debug("[Game::save]: saving...");

    FILE *file = fopen("game.save", "wa");


    fwrite(&GAME_CONTEXT.version, sizeof(size_t), 1, file);
    fwrite(&GAME_CONTEXT.scene, sizeof(Scene), 1, file);

    fclose(file);


    debug("[Game::save]: saved");
}

LoadGameResult loadGame() {
    debug("[Game::load]: start");

    FILE *file = fopen("game.save", "r");

    if (file == NULL) {
        warn("[Game::load]: File not exists");
        return FileNotExistsErrorLoadGameResult;
    }

    size_t version;

    fread(&version, sizeof(size_t), 1, file);

    if (version != GAME_VERSION) {
        warn("[Game::load]: Diffrent save file version");
        return DiffrentVersionErrorLoadGameResult;
    }

    fread(&GAME_CONTEXT.scene, sizeof(Scene), 1, file);

    debug("[Game::load]: loaded");
    return SuccessLoadGameResult;
}

void exitGame(ExitReason exitReason, Boolean shouldSaveGame) {
    if (shouldSaveGame == TRUE) {
        saveGame();
    }

    debug("[Game::exitGame]: exit game with reason \"%s\" ("EXIT_REASON_FORMAT")", getExitReasonName(exitReason), exitReason);

    exit(0);
}

void initializeGame() {
    debug("[Game::initializeGame]: start");
    GAME_CONTEXT = *GameContext_constructor(MAIN_MENU_SCENE);
    debug("[Game::initializeGame]: initialized");

    char* gameContextDebugString = GameContext_debug_string();
    debug("[Game::initializeGame]: game context:\n%s", gameContextDebugString);
    free(gameContextDebugString);
}

void setScene(Scene scene) {
    debug("[Game::setScene]: set new scene \"%s\" ("SCENE_FORMAT")", getSceneName(scene), scene);
    GameContext_setScene(&GAME_CONTEXT, scene);

}
#pragma endregion

void mainMenuLoop() {

    typedef unsigned char MainMenuState;
    #define WelcomeMainMenuState 0
    #define NewGameMainMenuState 1
    #define LoadGameMainMenuState 2

    MainMenuState state = WelcomeMainMenuState;
    unsigned char userAnswer = 0;

    while(TRUE) {
        switch (state) {
            case WelcomeMainMenuState:
                printf("Welcome to \"The Teams game\"!\n");

                while (TRUE) {
                    #define StartNewGameUserAnswer 1
                    #define LoadGameUserAnswer 2
                    #define ExitUserAnswer 3

                    if (userAnswer != 0) {
                        printf("Bad answer, try again...\n\n");
                        userAnswer = 0;
                    }

                    printf("1. Start new game\n");
                    printf("2. Load game\n");
                    printf("3. Exit\n");

                    printf("[User input]: ");
                    scanf("%u", &userAnswer);

                    if (userAnswer == StartNewGameUserAnswer) {
                        state = NewGameMainMenuState;
                    }
                    else if (userAnswer == LoadGameMainMenuState) {
                        state = LoadGameMainMenuState;
                    }
                    else if (userAnswer == ExitUserAnswer) {
                        exitGame(GAME_STOPED_EXIT_REASON, FALSE);
                    }
                    else {
                        warn("Unknown user answer");
                        continue;
                    }

                    userAnswer = 0;
                    break;
                }

                break;
            case NewGameMainMenuState:
                printf("New game!\n");
                exitGame(GAME_STOPED_EXIT_REASON, FALSE);
                break;
            case LoadGameMainMenuState:
                printf("Load game!\n");
                exitGame(GAME_STOPED_EXIT_REASON, FALSE);
        }
    }
}

void gameLoop() {

    while (GAME_CONTEXT.state != ExitGameState) {

        switch (GAME_CONTEXT.scene) {
            case MAIN_MENU_SCENE:
                mainMenuLoop();
        }

    }

}

void main() {
    initializeGame();
    gameLoop();
    exitGame(GAME_STOPED_EXIT_REASON, TRUE);
}