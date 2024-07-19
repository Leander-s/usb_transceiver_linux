#include "connection_slave.h"

int initConnection(const char *path) {
  int port = open(path, O_RDWR);
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
  tty.c_cc[VTIME] = 0;
  tty.c_cc[VMIN] = 0;

  cfsetspeed(&tty, B115200);

  if (tcsetattr(port, TCSANOW, &tty) != 0) {
    printf("Error from tcsetattr: %s\n", strerror(errno));
  }

  return port;
}

slave *createSlave() {
  slave *result = (slave *)malloc(sizeof(slave));
  result->connection = initConnection("/dev/ttyACM0");
  result->readBuffer = (char *)malloc(BUFFER_SIZE);
  memset(result->readBuffer, '\0', BUFFER_SIZE);
  result->sendBuffer = (char *)malloc(BUFFER_SIZE);
  memset(result->sendBuffer, '\0', BUFFER_SIZE);
  memcpy(result->sendBuffer, "ACK\n", 4);
  return result;
}

void destroySlave(slave *s) {
  close(s->connection);
  free(s->readBuffer);
  free(s->sendBuffer);
  free(s);
}

int receiveData(slave *s) {
  ssize_t n = 0;
  memset(s->readBuffer, '\0', BUFFER_SIZE);
  while (n < BUFFER_SIZE) {
    ssize_t received = read(s->connection, s->readBuffer + n, BUFFER_SIZE - n);
    n += received;
    if (n == 0) {
      continue;
    }
    char last = s->readBuffer[strlen(s->readBuffer) - 1];
    if (last == '\n') {
      write(s->connection, s->sendBuffer, strlen(s->sendBuffer));
      memset(s->sendBuffer + 4, '\0', BUFFER_SIZE - 4);
      break;
    }
    if (n > strlen(s->readBuffer)) {
      printf("Resetting\n");
      n = 0;
      memset(s->readBuffer, '\0', BUFFER_SIZE);
    }
  }
  return n;
}

void handleData(slave *s) {
  char *receivedData = s->readBuffer;

  // handle the data
  printf("%s", receivedData);
  memset(receivedData, '\0', BUFFER_SIZE);
}

void runSlave(slave *s) {
  while (1) {
    receiveData(s);
    handleData(s);
  }
}
