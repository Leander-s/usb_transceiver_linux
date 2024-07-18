#include "connection_master.h"

int main() {
  master* m = createMaster();
  memcpy(m->sendBuffer, "Give me an ack\n", 15);
  sendToSlave(m);
  m->sendBuffer[0] = 'W';
  sendToSlave(m);
  destroyMaster(m);
  return 0;
}
