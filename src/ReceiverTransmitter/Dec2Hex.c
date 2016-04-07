//
// Created by trl on 3/2/16.
//

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <termio.h>
#include <sys/types.h>
#include <sys/ioctl.h>


#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>   //ifreq


#define _FF000000 4278190080;
#define _00FF0000 16711680;
#define _0000FF00 65280;
#define _000000FF 255;

#define _0_Bytes 0;
#define _1_Bytes 8;
#define _2_Bytes 16;
#define _3_Bytes 24;


#define _F0 240;
#define _0F 15;

char Result[2];

long long MyID = 0;

char *Dec2Hex(short Number) {
	Result[0] = '0';
	Result[1] = '0';

	// printf("Dec2Hex. Number = %d :- Starting \n", Number);

	short N_0 = Number & _F0;
	N_0 = N_0 >> 4;
	short N_1 = Number & _0F;

	//  printf("\t short  N_0 = Number & F0. = %d ", N_0);
	//  printf("\t short  N_1 = Number & 0F. = %d \n", N_1);


	if (N_0 < 10) {
		Result[0] = N_0 + 48;
		//     printf("\t\t  N_0 < 10.  N_0 + 48. as Char = %c\n", Result[0]);
	}
	else {
		Result[0] = N_0 + 55;
		//    printf("\t\t  N_0 >= 10.  N_0 + 55. as Char = %c\n", Result[0]);
	}
	if (N_1 < 10) {
		Result[1] = N_1 + 48;
		//    printf("\t\t  N_1 < 10.  N_1 + 48. as Char = %c\n", Result[1]);
	}
	else {
		Result[1] = N_1 + 55;
		//   printf("\t\t  N_1 >= 10.  N_1 + 55. as Char = %c\n", Result[1]);
	}

	//  printf("Dec2Hex. Number = %d :- Ended With Result %s \n", Number, Result);
	return Result;
}

long long current_timestamp() {
	struct timeval te;
	gettimeofday(&te, NULL); // get current time
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // caculate milliseconds
	// printf("milliseconds: %lld\n", milliseconds);
	return milliseconds;
}

int longLatToFourBytes(double LongLat, short Bytes[4]) {

	printf("longLatToFourBytes. LongLat = %lf :- Starting \n", LongLat);

	ulong Number = (ulong)(LongLat * 10000000);

	printf("\tulong Number = LongLat * 10000000. = %ld ", Number);
	if (Number < 0) {
		printf("\tNumber is -ve. Adding .4294967296  = %ld \n", Number);
		Number = Number + 4294967296;
	} else {
		printf("\tNumber is +ve. No Change  = %ld \n", Number);
	}


/*
 *
 * #define _FF000000 4278190080;
 * #define _00FF0000 16711680;
 * #define _0000FF00 65280;
 * #define _000000FF 255;
 *
 *
 * #define _0_Bytes 0;
 * #define _2_Bytes 16;
 * #define _4_Bytes 32;
 * #define _6_Bytes 48;
 *
 *
 */

	ulong N_0 = Number & _FF000000;
	printf("\t\tulong N_0  = %ld & FF000000 = %ld \t", Number, N_0);
	N_0 = N_0 >> _3_Bytes; // Shift Right 6 Bytes
	printf("Shift N_0 Right 3 Bytes    = %ld\n", N_0);

	ulong N_1 = Number & _00FF0000;
	printf("\t\tulong N_1  = %ld & 00FF0000 = %ld  \t", Number, N_1);
	N_1 = N_1 >> _2_Bytes; // Shift Right 4 Bytes
	printf("Shift N_1 Right 2 Bytes    = %ld\n", N_1);

	ulong N_2 = Number & _0000FF00;
	printf("\t\tulong N_2  = %ld & 0000FF00 = %ld  \t", Number, N_2);
	N_2 = N_2 >> _1_Bytes; // Shift Right 2 Bytes
	printf("Shift N_2 Right 1 Bytes    = %ld\n", N_2);

	ulong N_3 = Number & _000000FF;
	printf("\t\tulong N_3  = %ld & 000000FF = %ld  \t", Number, N_3);
	N_3 = N_3 >> _0_Bytes; // Shift Right 0 Bytes
	printf("Shift N_3 Right 0 Bytes    = %ld\n", N_3);


	//           MS       LS
	// Octect =  N0 N1 N2 N3

	Bytes[0] = (short) N_0;
	Bytes[1] = (short) N_1;
	Bytes[2] = (short) N_2;
	Bytes[3] = (short) N_3;

	printf("longLatToFourBytes :- Ended \n");

}


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

int UpdateIDBytes_______(double Value, short Bytes[4]) {
	// printf("longLatToFourBytes. LongLat = %lf :- Starting \n", LongLat);


	char *Mac = "00:26:AD:05:11:7D";


	short MyID[4];

	char string[11];
	memcpy(string, Mac + 6, 11);


	long long N1 = (long long) MyID[0] * (long long) 4096;
	long long N2 = (long long) MyID[1] * (long long) 256;
	long long N3 = (long long) MyID[2] * (long long) 16;
	long long N4 = (long long) MyID[3];
	long long Sum = N1 + N2 + N3 + N4;

	printf("Actual MAC Address = %s\n", Mac);
	printf("Used MAC Address = %s\n", string);
	printf("              MyID = %ld = %ld + %ld + %ld + %ld\n", Sum, N1, N2, N3, N4);
	printf("    MyID_As_String = %s:%s:%s:%s\n", Dec2Hex(MyID[0]), Dec2Hex(MyID[1]), Dec2Hex(MyID[2]),
	       Dec2Hex(MyID[3]));


	printf("\n\n");


	int fd;
	struct ifreq ifr;
	char *iface = "hci0";
	unsigned char *mac;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);

	close(fd);

	mac = (unsigned char *) ifr.ifr_hwaddr.sa_data;

	//display mac address
	printf("Mac : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);


	long long Number = Value;


	ulong N_0 = Number & _FF000000;
	//  printf("\t\tulong N_0  = %ld & FF000000 = %ld \t", Number, N_0);
	N_0 = N_0 >> _3_Bytes; // Shift Right 6 Bytes
	// printf("Shift N_0 Right 3 Bytes    = %ld\n", N_0);

	ulong N_1 = Number & _00FF0000;
	// printf("\t\tulong N_1  = %ld & 00FF0000 = %ld  \t", Number, N_1);
	N_1 = N_1 >> _2_Bytes; // Shift Right 4 Bytes
	// printf("Shift N_1 Right 2 Bytes    = %ld\n", N_1);

	ulong N_2 = Number & _0000FF00;
	// printf("\t\tulong N_2  = %ld & 0000FF00 = %ld  \t", Number, N_2);
	N_2 = N_2 >> _1_Bytes; // Shift Right 2 Bytes
	//  printf("Shift N_2 Right 1 Bytes    = %ld\n", N_2);

	ulong N_3 = Number & _000000FF;
	// printf("\t\tulong N_3  = %ld & 000000FF = %ld  \t", Number, N_3);
	N_3 = N_3 >> _0_Bytes; // Shift Right 0 Bytes
	// printf("Shift N_3 Right 0 Bytes    = %ld\n", N_3);


	Bytes[0] = N_0;
	Bytes[1] = N_1;
	Bytes[2] = N_2;
	Bytes[3] = N_3;
}


int main(int Count, char *Arguments[]) {

	char *Mac = "00:26:AD:05:11:7D";


	short MyID[4];

	char string[11];
	memcpy(string, Mac + 6, 11);

	MyID[0] = HexToDec(string[0], string[1]);
	MyID[1] = HexToDec(string[3], string[4]);
	MyID[2] = HexToDec(string[6], string[7]);
	MyID[3] = HexToDec(string[9], string[10]);


	long long N1 = (long long) MyID[0] * (long long) 4096;
	long long N2 = (long long) MyID[1] * (long long) 256;
	long long N3 = (long long) MyID[2] * (long long) 16;
	long long N4 = (long long) MyID[3];
	long long Sum = N1 + N2 + N3 + N4;

	printf("Actual MAC Address = %s\n", Mac);
	printf("Used MAC Address = %s\n", string);
	printf("              MyID = %lld = %lld + %lld + %lld + %lld\n", Sum, N1, N2, N3, N4);
	printf("    MyID_As_String = %s:%s:%s:%s\n", Dec2Hex(MyID[0]), Dec2Hex(MyID[1]), Dec2Hex(MyID[2]),
	       Dec2Hex(MyID[3]));


	printf("\n\n");


	MyID = current_timestamp();

	short IDBytes[4];
	IDBytes[0] = 0;
	IDBytes[1] = 0;
	IDBytes[2] = 0;
	IDBytes[3] = 0;

	UpdateIDBytes_______(MyID, IDBytes);

	printf("My ID = %ld\tIDBytes =\t", MyID);


	int Index;
	for (Index = 0; Index < 4; Index++) { printf("%s\t", Dec2Hex(IDBytes[Index])); }
	printf("\n\n");


	if (Count < 2) {
		printf("Format: CMD Number\n");
		exit(-1);
	}
	double Number = strtod(Arguments[1], NULL);

	short Bytes[4];
	Bytes[0] = 0;
	Bytes[1] = 0;
	Bytes[2] = 0;
	Bytes[3] = 0;


	longLatToFourBytes(Number, Bytes);


	for (Index = 0; Index < 4; Index++) {

		printf("Bytes[%d] = %d \t Hex = %s\n", Index, Bytes[Index], Dec2Hex(Bytes[Index]));

	}

	exit(0);
}

