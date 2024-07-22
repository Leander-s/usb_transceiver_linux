#include "connection_master.h"

#ifdef _WIN32
int initConnection(const char* path){
    HANDLE hComm = CreateFile(L"\\\\.\\COM12",                // Port name
        GENERIC_READ | GENERIC_WRITE, // Read/Write
        0,                            // No sharing
        NULL,                         // No security
        OPEN_EXISTING,                // Open existing port only
        0,                            // Non-overlapped I/O
        NULL);                        // Null for Comm devices

    if (hComm == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening COM port" << std::endl;
        return 1;
    }

    // Set COM port parameters
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hComm, &dcbSerialParams)) {
        std::cerr << "Error getting COM port state" << std::endl;
        CloseHandle(hComm);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_115200;  // Baud rate
    dcbSerialParams.ByteSize = 8;         // Byte size
    dcbSerialParams.StopBits = ONESTOPBIT;// Stop bits
    dcbSerialParams.Parity = NOPARITY;    // Parity

    if (!SetCommState(hComm, &dcbSerialParams)) {
        std::cerr << "Error setting COM port state" << std::endl;
        CloseHandle(hComm);
        return 1;
    }

    // Set COM port timeouts
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hComm, &timeouts)) {
        std::cerr << "Error setting COM port timeouts" << std::endl;
        CloseHandle(hComm);
        return 1;
    }
    return hComm;
}

int writePort(HANDLE port, char* buffer){
    int amount;
    if(!WriteFile(port, buffer, strlen(buffer), &amount, NULL)){
        return -1;
    }
    return amount;
}

int readPort(HANDLE port, char* buffer, int amount){
    int amount;
    if(!ReadFile(port, buffer, strlen(buffer), &amount, NULL)){
        return -1;
    }
    return amount;
}
#else
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
  tty.c_cc[VTIME] = 1;
  tty.c_cc[VMIN] = 0;

  cfsetspeed(&tty, B115200);

  if (tcsetattr(port, TCSANOW, &tty) != 0) {
    printf("Error from tcsetattr: %s\n", strerror(errno));
  }

  return port;
}

int writePort(int port, char* buffer){
    return write(port, buffer, strlen(buffer));
}

int readPort(int port, char* buffer, int amount){
    return read(port, buffer, amount);
}
#endif

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
    ssize_t n = writePort(m->connection, m->sendBuffer);
    if (n == 0) {
      printf("Nothing was sent\n");
      memset(m->readBuffer, '\0', BUFFER_SIZE);
      memset(m->sendBuffer, '\0', BUFFER_SIZE);
      return 0;
    }
    n = 0;
    int read_failcounter = 0;
    while (n < BUFFER_SIZE) {
      ssize_t received = readPort(m->connection, m->readBuffer + n, BUFFER_SIZE);
      n += received;
      char last_c = m->readBuffer[strlen(m->readBuffer) - 1];
      if (last_c == '\n') {
        break;
      } 
      if(read_failcounter > 100){
          break;
      }
      read_failcounter++;
    }
    char ackBuffer[4];
    memcpy(ackBuffer, m->readBuffer, 4);
    if (strcmp(ackBuffer, "ACK\n") == 0) {
      ack = 1;
    } else {
      failcounter++;
    }
    if (failcounter > 10000) {
      printf("Failed to send\n");
      memset(m->readBuffer, '\0', BUFFER_SIZE);
      memset(m->sendBuffer, '\0', BUFFER_SIZE);
      return 1;
    }
  }
  memset(m->readBuffer, '\0', BUFFER_SIZE);
  memset(m->sendBuffer, '\0', BUFFER_SIZE);
  return 0;
}
