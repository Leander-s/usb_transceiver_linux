#include "config.h"

int main() {
  int port = open("/dev/tty", O_RDWR);
  if(port < 0){
      printf("Not connected");
  }

  close(port);
  return 0;
}
