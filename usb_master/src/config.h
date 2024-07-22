#ifdef _WIN32
#include <stdio.h>
#include <string.h>
#include <windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif

#define BUFFER_SIZE 1024
