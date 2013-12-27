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


// #################################################################################
// ################################ FUNCTIONS ######################################
// #################################################################################

int RS232_Open(int comport_number, int baudrate);
int RS232_Read(int comport_number, unsigned char* buf, int size);
int RS232_Write(int comport_number, unsigned char* buf, int size);
void RS232_Close(int comport_number);

#ifdef __cplusplus
}
#endif

#endif