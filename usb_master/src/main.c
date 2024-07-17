#include "connection_master.h"

int main() {
  int port = initConnection("/dev/ttyACM0");

  char readbuffer[BUFFER_SIZE];
  memset(readbuffer, '\0', BUFFER_SIZE);
  char message[BUFFER_SIZE];
  memset(message, '\0', BUFFER_SIZE);
  memcpy(message, "ACK\n", strlen("ACK\n"));
  int err = write(port, message, BUFFER_SIZE);
  if (err < 0) {
    printf("Error on write\n");
  }

  int n = 0;
  while (n < 4) {
    n += read(port, &readbuffer, BUFFER_SIZE);
  }
  printf("%s\n", readbuffer);

  close(port);
  return 0;
}
