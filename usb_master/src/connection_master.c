#include "connection_master.h"

const char *requests[] = {
    "GET ACK",
};

const int to_receive[] = {
    4,
};

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
  tty.c_cc[VTIME] = 10;
  tty.c_cc[VMIN] = 0;

  cfsetspeed(&tty, B9600);

  if (tcsetattr(port, TCSANOW, &tty) != 0) {
    printf("Error from tcsetattr: %s\n", strerror(errno));
  }

  return port;
}

master *createMaster() {
  master *result = (master *)malloc(sizeof(master));
  result->connection = initConnection("/dev/ttyACM0");
  result->readBuffer = (char *)malloc(BUFFER_SIZE);
  memset(result->readBuffer, '\0', BUFFER_SIZE);
  result->sendBuffer = (char *)malloc(BUFFER_SIZE);
  memset(result->sendBuffer, '\0', BUFFER_SIZE);
  return result;
}

void destroyMaster(master *m) {
  close(m->connection);
  free(m->readBuffer);
  free(m->sendBuffer);
  free(m);
}

int sendToSlave(master *m) {
  int ack = 0;
  int failcounter = 0;
  while (!ack) {
    printf("Sending %s\n", m->sendBuffer);
    memset(m->sendBuffer, 'o', BUFFER_SIZE-1);
    ssize_t n = write(m->connection, m->sendBuffer, BUFFER_SIZE);
    printf("%lu sent\n", n);
    if (n == 0) {
      printf("Nothing was sent\n");
      return 0;
    }
    n = 0;
    while (n < BUFFER_SIZE) {
      n += read(m->connection, m->readBuffer, BUFFER_SIZE);
      char last_c = m->readBuffer[n - 1];
      if (last_c == '\0' || last_c == '\n') {
        break;
      }
    }
    if (strcmp(m->readBuffer, "ACK\n") == 0) {
      printf("Got ack\n");
      ack = 1;
    } else {
      failcounter++;
    }
    if (failcounter > 20) {
      return 1;
    }
  }
  memset(m->sendBuffer, '\0', BUFFER_SIZE);
  return 0;
}

int requestFromSlave(master *m, int request_num) {
  int amount_received = 0;
  int n;
  int failcounter = 0;
  const char *request = requests[request_num];
  int done = to_receive[request_num];
  memset(m->readBuffer, '\0', BUFFER_SIZE);
  memset(m->sendBuffer, '\0', BUFFER_SIZE);
  memcpy(m->sendBuffer, requests[request_num],
         strlen(requests[request_num]) + 1);
  sendToSlave(m);

  while (amount_received != done) {
    n = read(m->connection, m->readBuffer, BUFFER_SIZE);
    if (n == 0) {
      failcounter++;
    }
    if (failcounter == 20) {
      return 1;
    }
  }
  return 0;
}
