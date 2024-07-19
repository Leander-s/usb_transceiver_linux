#include "connection_master.h"

int main() {
  master *m = createMaster();

  for (int i = 0; i < 100; i++) {
    memcpy(m->sendBuffer, "Give me an ack\n", 15);
    sendToSlave(m);
  }
  destroyMaster(m);
  return 0;
}
