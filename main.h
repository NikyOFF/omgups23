#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

//helpers
#define __OUT__
#define __IN__

//boolean
typedef unsigned char Boolean;
#define TRUE 1
#define FALSE 0

typedef unsigned char ExitReason;
#define EXIT_REASON_FORMAT "%u"
#define ERROR_EXIT_REASON 0
#define GAME_STOPED_EXIT_REASON 1

typedef unsigned long long Power;
#define POWER_FORMAT "%llu"

typedef unsigned char GameState;
#define ProcessGameState 0
#define ExitGameState 1

typedef unsigned long long Scene;
#define SCENE_FORMAT "%llu"
#define MAIN_MENU_SCENE 0
#define GAME_PROCESS_SCENE 1





typedef struct UnitStuct {
    Power power;
} Unit;

typedef struct GameContextStruct {
    size_t version;
    GameState state;

    Scene scene;
} GameContext;


#define DEBUG_MODE TRUE
#define GAME_VERSION 1

GameContext GAME_CONTEXT;

void slice(const char *str, char *result, size_t start, size_t end);
char* timestamp();


void saveGame();

typedef unsigned char LoadGameResult;
#define FileNotExistsErrorLoadGameResult 0
#define DiffrentVersionErrorLoadGameResult 1
#define SuccessLoadGameResult 2
LoadGameResult loadGame();
void exitGame(ExitReason exitReason, Boolean shouldSaveGame);
void initializeGame();
void setScene(Scene scene);


void debug(const char* format, ...);
void warn(const char* format, ...);
void assert(Boolean condition, const char* format, ...);

char* getExitReasonName(ExitReason exitReason);

Boolean isValidScene(Scene scene);
char* getSceneName(Scene scene);


GameContext* GameContext_constructor(Scene scene);
void GameContext_setScene(GameContext *gameContext, Scene scene);
char* GameContext_debug_string();
