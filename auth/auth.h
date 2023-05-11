#include "../user/user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../binary/binary.h"
#include <windows.h>

int initializeAuthService();

int login(char* login, char* password, struct UserStruct* outUser);

int saveUserToFile(struct UserStruct* user, pthread_mutex_t* mutex);

#define USER_FILE_NOT_FOUND 1
#define USER_FILE_FOUND 0
int loadUserFromFile(char* userLogin, struct UserStruct* outUser, pthread_mutex_t* mutex);
