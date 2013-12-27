#ifndef __RS232_H__
#define __RS232_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

// #################################################################################
// ################################ LINUX SECTION ##################################
// #################################################################################
#ifdef __linux__

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

// #################################################################################
// ################################ WINDOWS SECTION ################################
// #################################################################################
#else

#include <windows.h>

#endif

int RS232_Open(int, int);
int RS232_Read(int, unsigned char *, int);
int RS232_Write(int, unsigned char *, int);
void RS232_Close(int);

#ifdef __cplusplus
}
#endif

#endif