#include "connection_master.h"

int main() {
  master* m = createMaster();
  requestFromSlave(m, 0);
  m->sendBuffer[0] = 'W';
  sendToSlave(m);
  destroyMaster(m);
  return 0;
}
