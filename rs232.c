#include "rs232.h"

// #################################################################################
// ################################ LINUX SECTION ##################################
// ################################################################################# 
#ifdef __linux__ 

int Cport[30];
struct termios new_port_settings, old_port_settings[30];

char comports[30][16] = {
	"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
	"/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9","/dev/ttyS10","/dev/ttyS11",
	"/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0",
	"/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5",
	"/dev/ttyAMA0","/dev/ttyAMA1","/dev/ttyACM0","/dev/ttyACM1",
	"/dev/rfcomm0","/dev/rfcomm1","/dev/ircomm0","/dev/ircomm1"
};

int RS232_Open(int comport_number, int baudrate) 
	{
	int baudr, status, error;
	// make sure our port number falls in the appropriate limits
	if ((comport_number > 29) || (comport_number < 0)) {
		printf("invalid comport number\n");
		return -1;
	}

	// select the appropriate baudrate based on what was passed in
	switch (baudrate) {
		case      50 : baudr = B50;
					   break;
		case      75 : baudr = B75;
					   break;
		case     110 : baudr = B110;
					   break;
		case     134 : baudr = B134;
					   break;
		case     150 : baudr = B150;
					   break;
		case     200 : baudr = B200;
					   break;
		case     300 : baudr = B300;
					   break;
		case     600 : baudr = B600;
					   break;
		case    1200 : baudr = B1200;
					   break;
		case    1800 : baudr = B1800;
					   break;
		case    2400 : baudr = B2400;
					   break;
		case    4800 : baudr = B4800;
					   break;
		case    9600 : baudr = B9600;
					   break;
		case   19200 : baudr = B19200;
					   break;
		case   38400 : baudr = B38400;
					   break;
		case   57600 : baudr = B57600;
					   break;
		case  115200 : baudr = B115200;
					   break;
		case  230400 : baudr = B230400;
					   break;
		case  460800 : baudr = B460800;
					   break;
		case  500000 : baudr = B500000;
					   break;
		case  576000 : baudr = B576000;
					   break;
		case  921600 : baudr = B921600;
					   break;
		case 1000000 : baudr = B1000000;
					   break;
		default      : printf("invalid baudrate\n");
					   return -1;
					   break;
	}

	// open the file descriptor
	Cport[comport_number] = open(comports[comport_number], O_RDWR | O_NOCTTY | O_NDELAY);
	if (Cport[comport_number] == -1) {
		perror("unable to open comport ");
		return -1;
	}

	// grab the current port settings
	error = tcgetattr(Cport[comport_number], old_port_settings + comport_number);
	if (error == -1) {
		close(Cport[comport_number]);
		perror("unable to read portsettings ");
		return -1;
	}

	// wipe our struct, set appropriate flags
	memset(&new_port_settings, 0, sizeof(new_port_settings)); 
	new_port_settings.c_cflag = baudr | CS8 | CLOCAL | CREAD;
	new_port_settings.c_iflag = IGNPAR;
	new_port_settings.c_oflag = 0;
	new_port_settings.c_lflag = 0;
	new_port_settings.c_cc[VMIN] = 0;   
	new_port_settings.c_cc[VTIME] = 0;  

	// set the new port settings
	error = tcsetattr(Cport[comport_number], TCSANOW, &new_port_settings);
	if (error == -1) {
		close(Cport[comport_number]);
		perror("unable to adjust portsettings ");
		return -1;
	}

	if (ioctl(Cport[comport_number], TIOCMGET, &status) == -1) {
		perror("unable to get portstatus ");
		return -1;
	}

	status |= TIOCM_DTR;  // turn on DTR
	status |= TIOCM_RTS;  // turn on RTS

	if (ioctl(Cport[comport_number], TIOCMSET, &status) == -1) {
		perror("unable to set portstatus ");
		return -1;
	}

	return 0;
}

int RS232_Read(int comport_number, unsigned char* buf, int size) {
	int n;
	if (size > 4096) size = 4096;
	n = read(Cport[comport_number], buf, size);
	return n;
}

int RS232_Write(int comport_number, unsigned char* buf, int size) {
	return write(Cport[comport_number], buf, size);
}

void RS232_Close(int comport_number) {
	int status;

	if (ioctl(Cport[comport_number], TIOCMGET, &status) == -1) {
		perror("unable to get portstatus");
	}

	status &= ~TIOCM_DTR;  // turn off DTR
	status &= ~TIOCM_RTS;  // turn off RTS

	if (ioctl(Cport[comport_number], TIOCMSET, &status) == -1) {
		perror("unable to set portstatus");
	}

	tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
	close(Cport[comport_number]);
}

// #################################################################################
// ################################ WINDOWS SECTION ################################
// #################################################################################
#else

HANDLE Cport[16];
char baudr[64];

char comports[16][10]={
	"\\\\.\\COM1",  "\\\\.\\COM2",  "\\\\.\\COM3",  "\\\\.\\COM4",
	"\\\\.\\COM5",  "\\\\.\\COM6",  "\\\\.\\COM7",  "\\\\.\\COM8",
	"\\\\.\\COM9",  "\\\\.\\COM10", "\\\\.\\COM11", "\\\\.\\COM12",
	"\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15", "\\\\.\\COM16"
};

int RS232_Open(int comport_number, int baudrate) {
	if ((comport_number > 15) || (comport_number < 0)) {
		printf("invalid comport number\n");
		return -1;
	}

	switch (baudrate) {
		case     110 : strcpy(baudr, "baud=110 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case     300 : strcpy(baudr, "baud=300 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case     600 : strcpy(baudr, "baud=600 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case    1200 : strcpy(baudr, "baud=1200 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case    2400 : strcpy(baudr, "baud=2400 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case    4800 : strcpy(baudr, "baud=4800 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case    9600 : strcpy(baudr, "baud=9600 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case   19200 : strcpy(baudr, "baud=19200 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case   38400 : strcpy(baudr, "baud=38400 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case   57600 : strcpy(baudr, "baud=57600 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case  115200 : strcpy(baudr, "baud=115200 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case  128000 : strcpy(baudr, "baud=128000 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case  256000 : strcpy(baudr, "baud=256000 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case  500000 : strcpy(baudr, "baud=500000 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		case 1000000 : strcpy(baudr, "baud=1000000 data=8 parity=N stop=1 dtr=on rts=on");
					   break;
		default      : printf("invalid baudrate\n");
					   return -1;
					   break;
	}

	Cport[comport_number] = CreateFileA(comports[comport_number], GENERIC_READ|GENERIC_WRITE,
										0, NULL, OPEN_EXISTING, 0, NULL);                    

	if (Cport[comport_number] == INVALID_HANDLE_VALUE) {
		printf("unable to open comport\n");
		return -1;
	}

	// allocate space for and clear port settings
	DCB port_settings;
	memset(&port_settings, 0, sizeof(port_settings));
	port_settings.DCBlength = sizeof(port_settings);

	if (!BuildCommDCBA(baudr, &port_settings)) {
		printf("unable to set comport dcb settings\n");
		CloseHandle(Cport[comport_number]);
		return -1;
	}

	if (!SetCommState(Cport[comport_number], &port_settings)) {
		printf("unable to set comport cfg settings\n");
		CloseHandle(Cport[comport_number]);
		return -1;
	}

	COMMTIMEOUTS Cptimeouts;
	Cptimeouts.ReadIntervalTimeout         = MAXDWORD;
	Cptimeouts.ReadTotalTimeoutMultiplier  = 0;
	Cptimeouts.ReadTotalTimeoutConstant    = 0;
	Cptimeouts.WriteTotalTimeoutMultiplier = 0;
	Cptimeouts.WriteTotalTimeoutConstant   = 0;

	if (!SetCommTimeouts(Cport[comport_number], &Cptimeouts)) {
		printf("unable to set comport time-out settings\n");
		CloseHandle(Cport[comport_number]);
		return -1;
	}

	return 0;
}

int RS232_Read(int comport_number, unsigned char* buf, int size) {
	int n;
	if (size > 4096) size = 4096;
	ReadFile(Cport[comport_number], buf, size, (LPDWORD)((void *)&n), NULL);
	return n;
}

int RS232_Write(int comport_number, unsigned char* buf, int size) {
	int n;
	if (WriteFile(Cport[comport_number], buf, size, (LPDWORD)((void *)&n), NULL)) return n;
	return -1;
}

void RS232_CloseComport(int comport_number) {
	CloseHandle(Cport[comport_number]);
}

#endif


