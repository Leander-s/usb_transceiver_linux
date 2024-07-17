#include "config.h"

typedef struct {
  int connection;
  char *readBuffer;
  char *sendBuffer;
} master;

int initConnection(const char *path);
master *createMaster();
void destroyMaster(master *m);
int sendToSlave(master *m);
int requestFromSlave(master *m, int request);
