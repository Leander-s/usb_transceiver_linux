#include "connection_slave.h"

int main() {
  slave *s = createSlave();
  runSlave(s);
  destroySlave(s);
  return 0;
}
