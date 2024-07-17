#include "connection_master.h"
#include <stdio.h>

int main() {
  int port = initConnection("/dev/ttyACM0");

  char readbuffer[BUFFER_SIZE];
  memset(readbuffer, '\0', BUFFER_SIZE);
  char message[BUFFER_SIZE];
  memset(message, '\0', BUFFER_SIZE);
  memcpy(message, "ACK\n", strlen("ACK\n"));
  int err = write(port, message, strlen(message));
  if (err < 0) {
    printf("Error on write\n");
  }

  int n = 0;
  while (readbuffer[n] != '\n') {
    n += read(port, &readbuffer, BUFFER_SIZE);
    if(n == -1){
        printf("Error while reading\n");
    }
  }
  printf("%s\n", readbuffer);

  close(port);
  return 0;
}
