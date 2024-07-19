#include "config.h"

typedef struct {
    int connection;
    char* readBuffer;
    char* sendBuffer;
} slave;

int initConnection(const char* path);
slave* createSlave();
void destroySlave(slave* s);
int receiveData(slave* s);
void handleData(slave* s);
void runSlave(slave* s);
