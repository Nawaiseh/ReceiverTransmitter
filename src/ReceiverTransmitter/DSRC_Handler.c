//
// Created by TRL on 2/12/2016.
//
#include "Bluetooth_Handler.h"
#include "DSRC_Handler.h"


static int pid;
//static USTEntry entry;
static WMEApplicationRequest entry;
static uint64_t count = 0, blank = 0;


int rx_ret = -1, btooth_ret = -1;
WSMMessage rxmsg;

static WSMRequest MessageForSending;

static short MyID[4];

#define null (void *)0


WSMIndication rxpkt;
int ret = 0;
pthread_t DSRC_Thread = 0;

#define _FF000000 4278190080LL;
#define _00FF0000 16711680LL;
#define _0000FF00 65280LL;
#define _000000FF 255LL;

#define _0_Bytes 0LL;
#define _1_Bytes 8LL;
#define _2_Bytes 16LL;
#define _3_Bytes 24LL;


#define _F0 240LL;
#define _0F 15LL;


char Result[2];
BasicSafetyMessage_t *bsm;

/* BSM Data structure declaration */

void DSRC_Signal_Interrupt(void) {
	removeUser(pid, &entry);
	signal(SIGINT, SIG_DFL);
	printf("\n\nPackets received = %llu\n", count);
	printf("Blank Poll = %llu\n", blank);
	printf("Killed by kill signal\n");
	exit(0);
}

void DSRC_Signal_Terminate(void) { DSRC_Signal_Interrupt(); }

void DSRC_Set_Arguments(void *data, void *argname, int datatype) {
	u_int8_t string[1000];
	struct arguments *argument1;
	argument1 = (struct arguments *) argname;
	switch (datatype) {
		case ADDR_MAC:
			memcpy(string, argument1->macaddr, 17);
			string[17] = '\0';
			if (Extract_MAC_Address(data, string) < 0) {
				printf("invalid address\n");
			}
			break;
		case UINT8:

			//temp = atoi(argument1->channel);
			memcpy(data, (char *) argname, sizeof(u_int8_t));
			break;
	}
}


int Extract_MAC_Address(u_int8_t *mac, char *str) {
	int maclen = IEEE80211_ADDR_LEN;
	int len = strlen(str);
	int i = 0, j = 0, octet = 0, digits = 0, ld = 0, rd = 0;

	char num[2];

	u_int8_t tempmac[maclen];
	memset(tempmac, 0, maclen);
	memset(mac, 0, maclen);
	if ((len < (2 * maclen - 1)) || (len > (3 * maclen - 1)))
		return -1;
	while (i < len) {
		j = i;
		while (str[i] != ':' && (i < len)) {
			i++;
		}
		if (i > len) exit(0);
		digits = i - j;
		if ((digits > 2) || (digits < 1) || (octet >= maclen)) {
			return -1;
		}
		num[1] = tolower(str[i - 1]);
		num[0] = (digits == 2) ? tolower(str[i - 2]) : '0';
		if (isxdigit(num[0]) && isxdigit(num[1])) {
			ld = (isalpha(num[0])) ? 10 + num[0] - 'a' : num[0] - '0';
			rd = (isalpha(num[1])) ? 10 + num[1] - 'a' : num[1] - '0';
			tempmac[octet++] = ld * 16 + rd;
		} else {
			return -1;
		}
		i++;
	}
	if (octet > maclen) { return -1; }
	memcpy(mac, tempmac, maclen);

	return 0;
}

int Initialize_DSRC_RX_Environment(int arg, char *argv[]) {

	rxmsg.wsmIndication = &rxpkt;

	struct arguments arg1;
	memset(&DSRC_RX_Entry, 0, sizeof(WMEApplicationRequest));
	DSRC_RX_Entry.psid = atoi(argv[4]);

	if ((atoi(argv[1]) > USER_REQ_SCH_ACCESS_NONE) || (atoi(argv[1]) < USER_REQ_SCH_ACCESS_AUTO)) {
		printf("User request type invalid: setting default to auto\n");
		DSRC_RX_Entry.userreqtype = USER_REQ_SCH_ACCESS_AUTO;
	} else {
		DSRC_RX_Entry.userreqtype = atoi(argv[1]);
	}
	if (DSRC_RX_Entry.userreqtype == USER_REQ_SCH_ACCESS_AUTO_UNCONDITIONAL) {
		if (arg < 5) {
			printf("channel needed for unconditional access\n");
			exit(0);
		} else {
			DSRC_RX_Entry.channel = atoi(argv[5]);
		}
	}
	DSRC_RX_Entry.schaccess = atoi(argv[2]);
	DSRC_RX_Entry.schextaccess = atoi(argv[3]);
	if (arg > 6) {
		//  strncpy(arg1.macaddr, argv[6], 17);
		//  DSRC_Set_Arguments(DSRC_RX_Entry.macaddr, &arg1, ADDR_MAC);
	}


	int pid = getpid();
	printf("Registering DSRC User %d\n", DSRC_RX_Entry.psid);
	if (registerUser(pid, &DSRC_RX_Entry) < 0) {
		printf("Register DSRC User Failed \n");
		printf("Removing DSRC user if already present  %d\n", !removeUser(pid, &DSRC_RX_Entry));
		printf("DSRC USER Registered %d with PSID =%u \n", registerUser(pid, &DSRC_RX_Entry), DSRC_RX_Entry.psid);
	}
}

char *Dec2Hex(short Number) {
	Result[0] = '0';
	Result[1] = '0';
	short N_0 = Number & _F0;
	N_0 = N_0 >> 4;
	short N_1 = Number & _0F;
	if (N_0 < 10) { Result[0] = N_0 + 48; }
	else { Result[0] = N_0 + 55; }
	if (N_1 < 10) { Result[1] = N_1 + 48; } else { Result[1] = N_1 + 55; }
	return Result;
}

int PopulateID(char *MAC_Address) {
	char *string = malloc(11 * sizeof(char));
	bzero(string, 10);
	memcpy(string, MAC_Address + 6, 11);
	MyID[0] = HexToDec(string[0], string[1]);
	MyID[1] = HexToDec(string[3], string[4]);
	MyID[2] = HexToDec(string[6], string[7]);
	MyID[3] = HexToDec(string[9], string[10]);
	long long N1 = (long long) MyID[0];
	long long N2 = (long long) MyID[1];
	long long N3 = (long long) MyID[2];
	long long N4 = (long long) MyID[3];
	printf("Actual MAC Address = %s\n", MAC_Address);
	printf("              MyID = %lld \t %lld \t %lld \t %lld\n", N1, N2, N3, N4);
	free(string);
}

int UpdateIDBytes_______(BasicSafetyMessage_t *bsm, int Start) {
	bsm->blob1.buf[Start + 0] = MyID[0];
	bsm->blob1.buf[Start + 1] = MyID[1];
	bsm->blob1.buf[Start + 2] = MyID[2];
	bsm->blob1.buf[Start + 3] = MyID[3];
}

int UpdatePositionBytes_(double Value, BasicSafetyMessage_t *bsm, int Start) {
	ulong Number = (ulong)(Value * 10000000LL);
	if (Number < 0) {
		Number = Number + 4294967296LL;
	}
	ulong N_0 = Number & _FF000000;
	N_0 = N_0 >> _3_Bytes; // Shift Right 6 Bytes
	ulong N_1 = Number & _00FF0000;
	N_1 = N_1 >> _2_Bytes; // Shift Right 4 Bytes
	ulong N_2 = Number & _0000FF00;
	N_2 = N_2 >> _1_Bytes; // Shift Right 2 Bytes
	ulong N_3 = Number & _000000FF;
	N_3 = N_3 >> _0_Bytes; // Shift Right 0 Bytes

	bsm->blob1.buf[Start + 0] = N_0;
	bsm->blob1.buf[Start + 1] = N_1;
	bsm->blob1.buf[Start + 2] = N_2;
	bsm->blob1.buf[Start + 3] = N_3;

}

int UpdateElevationBytes(double Value, BasicSafetyMessage_t *bsm, int Start) {
	ulong Number = (ulong)(Value * 10);
	ulong N_0 = Number & _0000FF00;
	N_0 = N_0 >> _1_Bytes; // Shift Right 6 Bytes
	ulong N_1 = Number & _000000FF;
	N_1 = N_1 >> _0_Bytes; // Shift Right 4 Bytes
	bsm->blob1.buf[Start + 0] = N_0;
	bsm->blob1.buf[Start + 1] = N_1;
}

int UpdateSpeedBytes____(double Value, BasicSafetyMessage_t *bsm, int Start) {
	ulong Number = (ulong)(Value * 10);
	ulong N_0 = Number & _0000FF00;
	N_0 = N_0 >> _1_Bytes; // Shift Right 6 Bytes
	ulong N_1 = Number & _000000FF;
	N_1 = N_1 >> _0_Bytes; // Shift Right 4 Bytes
	bsm->blob1.buf[Start + 0] = N_0;
	bsm->blob1.buf[Start + 1] = N_1;
}

int UpdateDirectionBytes(double Value, BasicSafetyMessage_t *bsm, int Start) {
	ulong Number = (ulong)(Value / 1.5);
	ulong N_0 = Number & _000000FF;
	N_0 = N_0 >> _0_Bytes; // Shift Right 6 Bytes
	bsm->blob1.buf[Start + 0] = N_0;
}

int FillGPSInfo(BasicSafetyMessage_t *bsm) {
//	printf("==============================        01.   Fill GPS Info For DSRC SEND       ============================\n");
	static int count = 0;
	count++;                   /* count for the Number of packets Tx */
	int j;
	bsm->msgID.size = sizeof(uint8_t);
	bsm->msgID.buf[0] = DSRCmsgID_basicSafetyMessage; // Choose what type of message you want to transfer
	bsm->blob1.buf = (uint8_t *) calloc(1, 38 * sizeof(uint8_t)); // Allocate the memory for the blob buffer

	bsm->blob1.size = 38 * sizeof(uint8_t);
	for (j = 0; j < 38; j++) {
		bsm->blob1.buf[j] = j; /* We are filling some dummy data because of It is a Sample application */
	}
	bsm->blob1.buf[0] = count % 1000;

	//printf("==============================        02.   Fill GPS Info For DSRC SEND       ============================\n");

	char *GPSAddress = get_gpsc_devaddr();
	get_gps_status(&gpsdata, GPSAddress);

	GPSData *GPS = &gpsdata;


//	printf("==============================        03.   Fill GPS Info For DSRC SEND       ============================\n");
	UpdateIDBytes_______(bsm, 01); // Update Temporary ID Bytes
//	printf("==============================        04.   Fill GPS Info For DSRC SEND       ============================\n");
	UpdatePositionBytes_(GPS->latitude, bsm, 07); // Update Latitude Bytes
//	printf("==============================        05.   Fill GPS Info For DSRC SEND       ============================\n");
	UpdatePositionBytes_(GPS->longitude, bsm, 11); // Update Latitude Bytes
//	printf("==============================        06.   Fill GPS Info For DSRC SEND       ============================\n");
	UpdateElevationBytes(GPS->altitude, bsm, 15); // Update Latitude Bytes
//	printf("==============================        07.   Fill GPS Info For DSRC SEND       ============================\n");
	UpdateSpeedBytes____(GPS->speed, bsm, 21); // Update Temporary ID Bytes
//	printf("==============================        08.   Fill GPS Info For DSRC SEND       ============================\n");
	UpdateDirectionBytes(GPS->course, bsm, 25); // Update Temporary ID Bytes
//	printf("==============================        09.   Fill GPS Info For DSRC SEND       ============================\n");
}

int BuildMessage(int Channel) {

	asn_enc_rval_t rvalenc;
//	printf("==============================        01.   Build Message To SEND DSRC       ============================\n");

	/* WSM Channel and Tx info */
	MessageForSending.chaninfo.channel = DSRC_RX_Entry.channel;
	MessageForSending.chaninfo.rate = 3;
	MessageForSending.chaninfo.txpower = 15;
	MessageForSending.version = 1;
	MessageForSending.security = 1;
	MessageForSending.psid = 10;
	MessageForSending.txpriority = 1;

	/* BSM related information */

	bsm = (BasicSafetyMessage_t *) calloc(1, sizeof(*bsm));
	bsm->msgID.buf = (uint8_t *) calloc(1, sizeof(uint8_t));


	rxmsg.wsmIndication = &rxpkt;

	//memset(bsm->blob1.buf, 0, sizeof(bsm->blob1.buf));

	FillGPSInfo(bsm);
//	printf("==============================        02.   Build Message To SEND DSRC       ============================\n");

	rvalenc = der_encode_to_buffer(&asn_DEF_BasicSafetyMessage, bsm, &MessageForSending.data.contents,
	                               1000); // Encode your BSM in to WSM Packets
	if (rvalenc.encoded == -1) {
		fprintf(stderr, "Cannot encode %s: %s\n", rvalenc.failed_type->name, strerror(errno));
	} else {
		//  printf("Structure successfully encoded %d\n", rvalenc.encoded);
		MessageForSending.data.length = rvalenc.encoded;
		asn_DEF_BasicSafetyMessage.free_struct(&asn_DEF_BasicSafetyMessage, bsm, 0);
//		printf("==============================        03.   Build Message To SEND DSRC       ============================\n");

	}
//	printf("==============================        04.   Build Message To SEND DSRC       ============================\n");

	return 1;
}

int TransmitMessage() {

//	printf("==============================        01.   Transmit Message DSRC SEND       ============================\n");
	ret = txWSMPacket(pid, &MessageForSending);

}

int Send_DSRC_Message(int Channel) {

	BuildMessage(Channel);

	TransmitMessage();
//	printf("==============================        01.   Send DSRC Message Ended       ============================\n");

	return SEND_DSRC_MESSAGE;
}

int Receive_DSRC_Message() {

//	printf("==============================        01.   Receive_DSRC_Message       ============================\n");
	rx_ret = rxWSMMessage(pid, &rxmsg); /* Function to receive the Data from TX application */
	// sched_yield();
//	printf("==============================        02.   Receive_DSRC_Message       ============================\n");

	if (rx_ret > 0) {
/*		printf("Received DSRC Message txpower= %d, rateindex=%d Packet No =#%llu#\n", rxpkt.chaninfo.txpower,
		       rxpkt.chaninfo.rate, Bluetooth_Count++);
*/
		rxWSMIdentity(&rxmsg, 0); //Identify the type of received Wave Short Message.
//		printf("==============================        03.   Receive_DSRC_Message       ============================\n");
		if (!rxmsg.decode_status) {
			Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device(rxmsg);
			//xml_print(rxmsg); /* call the parsing function to extract the contents of the received message */
		}
//		printf("==============================        04.   Receive_DSRC_Message       ============================\n");

	}
	else { Bluetooth_Blank++; }
//	printf("==============================        05.   Receive_DSRC_Message       ============================\n");

	return RECEIVE_DSRC_MESSAGE;
}

int LastOperation = RECEIVE_DSRC_MESSAGE;

void *DSRC_SEND_Thread(void *arg) {

	pthread_t CurrentThread_ID = pthread_self();
	while (1) {
//		printf("==============================           Trying To SEND DSRC       ============================\n");

		usleep(20000);
		if (LastOperation != SEND_DSRC_MESSAGE) {
			LastOperation = Send_DSRC_Message(DSRC_SEND_CHANNEL);
			//		printf("==============================           DSRC IS Sent       ============================\n");


		} else {
			//		printf("==============================           DSRC IS Not Allowed to be  Sent     ============================\n");
		}
	}
}

int IsThisCarIsMyCar(WSMMessage RecievedDSRCMessage) {
	BasicSafetyMessage_t *Received_DSRC_BSM;
	if (RecievedDSRCMessage.type == WSMMSG_BSM) {
		Received_DSRC_BSM = (BasicSafetyMessage_t *) RecievedDSRCMessage.structure;
		int i = 1;
		for (; i < 5; i++) {
			short N = Received_DSRC_BSM->blob1.buf[i];
			if (N != MyID[i - 1]) { return 0; }
		}
		return 1;
	}
	return -1;
}

void *DSRC_RECEIVE_Thread(void *arg) {

	pthread_t CurrentThread_ID = pthread_self();
	while (1) {
//		printf("==============================           Trying To Receive DSRC Messages      ============================\n");

		usleep(20000);
		if (LastOperation == SEND_DSRC_MESSAGE) {
			LastOperation = Receive_DSRC_Message();

		} else {
//			printf("==============================           DSRC Message Receiving Mode IS Not Allowed to be  Received     ============================\n");
		}

	}

}

int SendReceive(int Channel) {

	int LastOperation = RECEIVE_DSRC_MESSAGE;

	printf("Start Send and Receive of DSRC Messages \n");
	while (1) {

		usleep(20000);
		if (LastOperation == SEND_DSRC_MESSAGE) { LastOperation = Receive_DSRC_Message(); }
		else { LastOperation = Send_DSRC_Message(Channel); }

	}//while
}
