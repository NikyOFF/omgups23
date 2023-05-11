#include "auth.h"



int initializeAuthService() {
    CreateDirectory("players", NULL);
    return 0;
}

int login(char* login, char* password, struct UserStruct* outUser) {

    return 0;
}

int saveUserToFile(struct UserStruct* user, pthread_mutex_t* mutex) {
    char filename[40];
    strcat(filename, "players/");
    strcat(filename, user->login);

    Binary binary = *Binary_constructor(512);
    Binary_writeUser(&binary, user);

    if (mutex != NULL) {
        pthread_mutex_lock(mutex);
    }

    FILE *file = fopen(filename, "w");
    fwrite(binary.buffer, Binary_getSize(&binary), 1, file);
    fclose(file);

    if (mutex != NULL) {
        pthread_mutex_unlock(mutex);
    }

    return 0;
}

int loadUserFromFile(char* userLogin, struct UserStruct* outUser, pthread_mutex_t* mutex) {
    size_t userLoginSize = strlen(userLogin);
    char* filename = calloc(1, sizeof("players/") + userLoginSize);
    strcat(filename, "players/");
    strcat(filename, userLogin);

    if (mutex != NULL) {
        pthread_mutex_lock(mutex);
    }

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        if (mutex != NULL) {
            pthread_mutex_unlock(mutex);
        }

        return USER_FILE_NOT_FOUND;
    }

    fseek(file, 0, SEEK_END);
    size_t fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    Binary binary = *Binary_constructor(fsize + 1);
    fread(binary.buffer, fsize, 1, file);
    fclose(file);

    if (mutex != NULL) {
        pthread_mutex_unlock(mutex);
    }

    *outUser = *Binary_readUser(&binary);

    return USER_FILE_FOUND;
}