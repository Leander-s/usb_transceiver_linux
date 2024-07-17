#include "config.h"
#include <string.h>
#include <termios.h>

int main() {
  int port = open("/dev/ttyACM0", O_RDWR);
  if (port < 0) {
    printf("Error while connecting: %s\n", strerror(errno));
    return 0;
  }
  printf("Connected successfully\n");

  struct termios tty;

  if (tcgetattr(port, &tty) != 0) {
    printf("Error from tcgetattr: %s\n", strerror(errno));
    return 0l;
  }

  tty.c_iflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~CRTSCTS;
  tty.c_cflag |= CREAD | CLOCAL;
  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ECHONL;
  tty.c_lflag &= ~ISIG;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
  tty.c_oflag &= ~OPOST;
  tty.c_oflag &= ~ONLCR;
  tty.c_cc[VTIME] = 10;
  tty.c_cc[VMIN] = 0;

  cfsetspeed(&tty, B115200);

  if (tcsetattr(port, TCSANOW, &tty) != 0) {
    printf("Error from tcsetattr: %s\n", strerror(errno));
  }

  char readbuffer[1024];
  memset(readbuffer, '\0', 1024);
  char message[1024];
  memset(message, '\0', 1024);
  memcpy(message, "ACK\n", strlen("ACK\n"));
  write(port, message, 1024);

  int n = 0;
  while (n < 100) {
    n = read(port, &readbuffer, 1024);
  }
  printf("%s\n", readbuffer);

  close(port);
  return 0;
}
