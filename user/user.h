#include <stdlib.h>
#include <memory.h>
#include "../binary/binary.h"

#ifndef USER_H
#define USER_H
typedef struct UserStruct {
    unsigned long long loginSize;
    char* login;
    unsigned long long passwordSize;
    char* password;
    unsigned long long wins;
    unsigned long long defeats;
} User;

User* User_constructor(
    unsigned long long loginSize,
    char* login,
    unsigned long long passwordSize,
    char* password,
    unsigned long long wins,
    unsigned long long defeats
);

void User_deconstructor(User* user);


void Binary_writeUser(Binary* binary, User* user);
User* Binary_readUser(Binary* binary);
#endif //USER_H
