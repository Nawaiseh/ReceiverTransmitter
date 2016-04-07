//
// Created by trl on 2/12/16.
//


#include <dirent.h>

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <syslog.h>
#include <termio.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include "wave.h"
#include <pthread.h>
#include <semaphore.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>
#include <asnwave.h>
#include <BasicSafetyMessage.h>
#include <netinet/tcp.h> //for TCP_NODELAY
#include "Bluetooth_Handler.h"
#include "DSRC_Handler.h"

static int pid;
char addr1[1024];

sem_t addr;
WSMIndication rxpkt;


short HexDigitToDec(char HEX) {
	short Result = 0;
	switch (HEX) {
		case '0' :
			Result = 00;
			break;
		case '1' :
			Result = 01;
			break;
		case '2' :
			Result = 02;
			break;
		case '3' :
			Result = 3;
			break;
		case '4' :
			Result = 4;
			break;
		case '5' :
			Result = 5;
			break;
		case '6' :
			Result = 6;
			break;
		case '7' :
			Result = 7;
			break;
		case '8' :
			Result = 8;
			break;
		case '9' :
			Result = 9;
			break;

		case 'A' :
			Result = 10;
			break;
		case 'B' :
			Result = 11;
			break;
		case 'C' :
			Result = 12;
			break;
		case 'D' :
			Result = 13;
			break;
		case 'E' :
			Result = 14;
			break;
		case 'F' :
			Result = 15;
			break;
	}

	return Result;

}

short HexToDec(char HEX1, char HEX2) {
	return HexDigitToDec(HEX1) * 16 + HexDigitToDec(HEX2);
}

pid_t FindProcessID(const char *name) {
	DIR *dir;
	struct dirent *ent;
	char *endptr;
	char buf[512];

	if (!(dir = opendir("/proc"))) {
		perror("can't open /proc");
		return -1;
	}

	while ((ent = readdir(dir)) != NULL) {
		/* if endptr is not a null character, the directory is not
		 * entirely numeric, so ignore it */
		long lpid = strtol(ent->d_name, &endptr, 10);
		if (*endptr != '\0') {
			continue;
		}

		/* try to open the cmdline file */
		snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);

		FILE *fp = fopen(buf, "r");

		if (fp) {
			if (fgets(buf, sizeof(buf), fp) != NULL) {
				/* check the first token in the file, the program name */
				char *first = strtok(buf, " ");
				// printf("%s: %d\n", first, lpid);
				if (!strcmp(first, name)) {
					fclose(fp);
					closedir(dir);
					return (pid_t) lpid;
				}
			}
			fclose(fp);
		}

	}

	closedir(dir);
	return -1;
}

int main(int arg, char *argv[]) {


	if (arg < 6) {
		printf("Missing Device ID\n");
		exit(0);
	} else {
		PopulateID(argv[6]);
	}


	if (arg < 6) {
		//                    0                                    1                                        2             3               4       5      6
		printf("usage: ReceiverTransmitter_Bluetooth [user req type<1-auto> <2-unconditional> <3-none>] [imm access] [extended access] [PSID] [channel] [ID]\n");
		return 0;
	}


	printf("Invoking WAVE driver \n");
	if (invokeWAVEDevice(WAVEDEVICE_LOCAL, 0) < 0) {
		printf("Open Failed. Quitting\n");
		exit(-1);
	}
	printf("WAVE driver Invoked \n");


	Initialize_Bluetooth_Environment(arg, argv);
	Initialize_DSRC_RX_Environment(arg, argv);
	//Initialize_DSRC_TX_Environment(arg, argv);









	while (1) {

		pid_t pid = FindProcessID("/usr/local/bin/usbd");
		if (pid != -1) {
			while (1) {
				kill(pid, SIGINT);
				pid = FindProcessID("/usr/local/bin/usbd");
				if (pid == -1) {
					break;
				}
			}
		}


		pid = FindProcessID("/usr/local/bin/getwbsstxrxencdec");
		if (pid != -1) {
			while (1) {
				kill(pid, SIGINT);
				pid = FindProcessID("/usr/local/bin/getwbsstxrxencdec");
				if (pid == -1) {
					break;
				}
			}
		}

		pid = FindProcessID("/usr/local/bin/Asm.bin");
		if (pid != -1) {
			while (1) {
				kill(pid, SIGINT);
				pid = FindProcessID("/usr/local/bin/Asm.bin");
				if (pid == -1) {
					break;
				}
			}
		}

		sleep(1);
		sched_yield();

		/*
		  printf("Bluetooth_ConnectionStatus = %d \n", Bluetooth_ConnectionStatus);
		  if (Bluetooth_ConnectionStatus != BluetoothIsConnected) {
			 // usleep(10000);
			  sched_yield();
			  // printf("Bluetooth_ConnectionStatus = Connection Lost \n");

		  } else {
			  break;
		  }
  */
	}
	while (1) {
		// usleep(100000);
		sched_yield();
	}
	// sleep(60);

	// printf("Bluetooth_ConnectionStatus = %d \n", Bluetooth_ConnectionStatus);
	//if (Bluetooth_ConnectionStatus != BluetoothIsConnected) {
	//   printf("Bluetooth_ConnectionStatus = Connection Lost \n");

	// }
	printf("Bluetooth_ConnectionStatus = BluetoothIsConnected \n");


}
