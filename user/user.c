#include "user.h"

struct UserStruct* User_constructor(
        unsigned long long loginSize,
        char* login,
        unsigned long long passwordSize,
        char* password,
        unsigned long long wins,
        unsigned long long defeats
) {
    struct UserStruct* user = calloc(1, sizeof(struct UserStruct));

    user->loginSize = loginSize;
    user->login = login;
    user->passwordSize = passwordSize;
    user->password = password;
    user->wins = wins;
    user->defeats = defeats;

    return user;
}

void User_deconstructor(struct UserStruct* user) {
    free(user);
}


void Binary_writeUser(Binary* binary, User* user) {
    Binary_writeString(binary, user->login);
    Binary_writeString(binary, user->password);
    Binary_writeSizeT(binary, user->wins);
    Binary_writeSizeT(binary, user->defeats);
}

User* Binary_readUser(Binary* binary) {
    struct UserStruct* user = calloc(1, sizeof(struct UserStruct));

    user->login = Binary_readString(binary, &user->loginSize);
    user->password = Binary_readString(binary, &user->passwordSize);
    Binary_readSizeT(binary, &user->wins);
    Binary_readSizeT(binary, &user->defeats);

    return user;
}