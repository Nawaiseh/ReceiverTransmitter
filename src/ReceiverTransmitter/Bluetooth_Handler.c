#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "Bluetooth_Handler.h"
#include "DSRC_Handler.h"
// ~~~~~~~~~~~~~~~~~~   Variables  ~~~~~~~~~~~~~~~~~~

void sig_int_bluetooth(void);

inquiry_info *Bluetooth_inquiry_info = NULL;
int Bluetooth_Loco_Channel = -1;
int Bluetooth_Socket = -1;
int Bluetooth_Connection_Established = FALSE;
char BluetoothAddress[18];
char Bluetooth_Message[1024] = "";
int Bluetooth_Forward = 0;
pthread_t Bluetooth_Thread = 0;
pthread_t DSRC_SEND_Thread_Pointer = 0;
pthread_t DSRC_RECEIVE_Thread_Pointer = 0;
static WSMRequest wsmtxreq;

int DSRC_Channel = 174;
#define null (void *)0

static WSMRequest MyMessageForSending;

#define MessageSize 76

char Message[MessageSize];

static BasicSafetyMessage_t *My__Bluetooth_BSM;

/* BSM Data structure declaration */


extern int SendReceive();

extern void *DSRC_SEND_Thread(void *arg);

extern void *DSRC_RECEIVE_Thread(void *arg);

extern int IsThisCarIsMyCar(WSMMessage RecievedDSRCMessage);

void sig_int_bluetooth(void) {
	if (Bluetooth_inquiry_info != NULL) { free(Bluetooth_inquiry_info); }
	if (Bluetooth_Socket >= 0) { close(Bluetooth_Socket); }
}

void Bluetooth_Signal_Interrupt(void) {

	removeUser(pid, &Bluetooth_Entry);
	pthread_cancel(Bluetooth_Thread);
	sig_int_bluetooth();
	signal(SIGINT, SIG_DFL);
	printf("\n\nPackets received = %llu\n", Bluetooth_Count);
	printf("Blank Poll = %llu\n", Bluetooth_Blank);
	printf("remoterx killed by kill signal\n");
	exit(0);

}

void Bluetooth_Signal_Terminate(void) {
	Bluetooth_Signal_Interrupt();
}

int String2UUID(const char *uuid_str, uuid_t *uuid) {
	uint32_t uuid_int[4];
	char *endptr;

	if (strlen(uuid_str) == 36) {
		// Parse uuid128 standard format: 12345678-9012-3456-7890-123456789012
		char buf[9] = {0};

		if ((uuid_str[8] != '-') && (uuid_str[13] != '-') && (uuid_str[18] != '-') && (uuid_str[23] != '-')) {
			return 0;
		}
		// first 8-bytes
		strncpy(buf, uuid_str, 8);
		uuid_int[0] = htonl(strtoul(buf, &endptr, 16));
		if (endptr != buf + 8) return 0;

		// second 8-bytes
		strncpy(buf, uuid_str + 9, 4);
		strncpy(buf + 4, uuid_str + 14, 4);
		uuid_int[1] = htonl(strtoul(buf, &endptr, 16));
		if (endptr != buf + 8) return 0;

		// third 8-bytes
		strncpy(buf, uuid_str + 19, 4);
		strncpy(buf + 4, uuid_str + 24, 4);
		uuid_int[2] = htonl(strtoul(buf, &endptr, 16));
		if (endptr != buf + 8) return 0;

		// fourth 8-bytes
		strncpy(buf, uuid_str + 28, 8);
		uuid_int[3] = htonl(strtoul(buf, &endptr, 16));
		if (endptr != (buf + 8)) { return 0; }

		if (uuid != NULL) sdp_uuid128_create(uuid, uuid_int);
	} else if (strlen(uuid_str) == 8) {
		// 32-bit reserved UUID
		uint32_t i = strtoul(uuid_str, &endptr, 16);
		if (endptr != uuid_str + 8) return 0;
		if (uuid != NULL) sdp_uuid32_create(uuid, i);
	} else if (strlen(uuid_str) == 4) {
		// 16-bit reserved UUID
		int i = strtol(uuid_str, &endptr, 16);
		if (endptr != uuid_str + 4) return 0;
		if (uuid != NULL) sdp_uuid16_create(uuid, i);
	} else {
		return 0;
	}

	return 1;
}

int Bluetooth_ConfirmBeforeJoin(WMEApplicationIndication *appind) {
	printf("\nJoin\n");
	return 1;
}

int Bluetooth_Write(char *data, int size) {
	int status = 0;
	pid = getpid();

	if (Bluetooth_Loco_Channel > 0) {
		status = write(Bluetooth_Socket, data, size);
		printf("Bluetooth Write Status %d \t", status);
		if (status < 0) {
			close(Bluetooth_Socket);
			Bluetooth_Socket = -1;
			Bluetooth_Loco_Channel = -1;
			Bluetooth_Connection_Established = FALSE;
			Bluetooth_ConnectionStatus = BluetoothConnectionLost;
			Bluetooth_Forward = -4;
			printf("Bluetooth Connection Is Lost \n");
		}
		return status;
	}
	else {
		return -5555;
	}
}

int Bluetooth_Read(char *data, int size) {

	int status = 0;
	if (Bluetooth_Loco_Channel > 0) {
		status = read(Bluetooth_Socket, data, size);
		if (status < 0) {
			close(Bluetooth_Socket);
			Bluetooth_Socket = -1;
			Bluetooth_Loco_Channel = -1;
			Bluetooth_Connection_Established = FALSE;
			Bluetooth_ConnectionStatus = BluetoothConnectionLost;
			Bluetooth_Forward = -4;
		}
		return status;
	}

	return -1;
}

void Bluetooth_Set_Arguments(void *data, void *argname, int datatype) {
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


int BuildBluetoothMessage() {
	/* WSM Channel and Tx info */
	MyMessageForSending.chaninfo.channel = DSRC_SEND_CHANNEL;
	MyMessageForSending.chaninfo.rate = 3;
	MyMessageForSending.chaninfo.txpower = 15;
	MyMessageForSending.version = 1;
	MyMessageForSending.security = 1;
	MyMessageForSending.psid = 5;
	MyMessageForSending.txpriority = 1;
	memset(&MyMessageForSending.data, 0, sizeof(WSMData));

	My__Bluetooth_BSM = (BasicSafetyMessage_t *) calloc(1, sizeof(*My__Bluetooth_BSM));
	My__Bluetooth_BSM->msgID.buf = (uint8_t *) calloc(1, sizeof(uint8_t));
	FillGPSInfo(My__Bluetooth_BSM);

	return 1;
}


int Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device(WSMMessage RecievedDSRCMessage) {

//	printf("==============================        01.   Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device       ============================\n");
	if (IsThisCarIsMyCar(RecievedDSRCMessage) != 0) {
		printf("==============================           The Sender Car Was Me     ============================\n");
		return;
	}
	uint32_t longitude_val;
	uint32_t latitude_val;
	uint16_t altitude_val;
	uint16_t speed_val;
	uint16_t heading_val;
	uint8_t valid_bsm = 0;
	int ret = 0;//, i = 0;
	void *Logdata;
	uint32_t temp_var;
	BasicSafetyMessage_t *Received_DSRC_BSM;
//	printf("==============================        02.   Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device       ============================\n");

	if (RecievedDSRCMessage.type == WSMMSG_BSM) {
		valid_bsm = 1;
		Received_DSRC_BSM = (BasicSafetyMessage_t *) RecievedDSRCMessage.structure;


		memcpy(&latitude_val, Received_DSRC_BSM->blob1.buf + 7, 4);
		temp_var = (uint32_t) htobe32((uint32_t) latitude_val);
		latitude_val = temp_var;
		memcpy(&longitude_val, Received_DSRC_BSM->blob1.buf + 11, 4);
		temp_var = longitude_val;
		longitude_val = htobe32(temp_var);
		memcpy(&altitude_val, Received_DSRC_BSM->blob1.buf + 15, 2);
		memcpy(&speed_val, Received_DSRC_BSM->blob1.buf + 21, 2);
		memcpy(&heading_val, Received_DSRC_BSM->blob1.buf + 23, 2);
		ret = 1;

//		printf("==============================        03.   Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device       ============================\n");
		int btooth_ret = -1;


		if (Bluetooth_Forward == 1) {


			BuildBluetoothMessage();

			bzero(Message, MessageSize);

			memcpy(Message + 00, My__Bluetooth_BSM->blob1.buf, 38);
			memcpy(Message + 38, Received_DSRC_BSM->blob1.buf, 38);
//			printf("==============================        04.   Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device       ============================\n");

			printf("==================================================================================\n");
			printf("\nForwarded Message[%d]: ", MessageSize);
			printf("\n   MyCar: ");
			int Index = 0;
			for (; Index < 38; Index++) { printf("%s ", Dec2Hex(My__Bluetooth_BSM->blob1.buf[Index])); }
			printf("\nOtherCar: ");

			Index = 0;
			for (; Index < 38; Index++) { printf("%s ", Dec2Hex(Received_DSRC_BSM->blob1.buf[Index])); }
			printf("\n==================================================================================\n");
			btooth_ret = Bluetooth_Write(Message, MessageSize); // Write to bluetooth socket
//			printf("==============================        05.   Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device       ============================\n");
		}
		else {
			printf("No Android Application Is Running \n");
		}
//		printf("==============================        06.   Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device       ============================\n");

	}
//	printf("==============================        07.   Decode_BSM_Message_And_Forward_It_To_BlueTooth_Device       ============================\n");
	return ret;
}


//char Message[1024];

int Initialize_Bluetooth_Environment(int arg, char *argv[]) {
	struct arguments arg1;
	int thread_arg = 2;
	int thread_ret = -1;
	Bluetooth_ConnectionStatus = BluetoothConnectionLost;


	thread_ret = pthread_create(&Bluetooth_Thread, NULL, Bluetooth_Main_Thread, (void *) &thread_arg);
	sched_yield();
	if (thread_ret < 0) {
		printf("\nERROR : main_bluethread not created\n");
		exit(1);
	}




	/* catch control-c and kill signal*/
	signal(SIGINT, (void *) Bluetooth_Signal_Interrupt);
	signal(SIGTERM, (void *) Bluetooth_Signal_Terminate);

	registerLinkConfirm(Bluetooth_ConfirmBeforeJoin);


	memset(&Bluetooth_Entry, 0, sizeof(WMEApplicationRequest));
	Bluetooth_Entry.psid = 10;

	int pid = Bluetooth_Entry.psid;
	if ((atoi(argv[1]) > USER_REQ_SCH_ACCESS_NONE) || (atoi(argv[1]) < USER_REQ_SCH_ACCESS_AUTO)) {
		Bluetooth_Entry.userreqtype = USER_REQ_SCH_ACCESS_AUTO;
	} else {
		Bluetooth_Entry.userreqtype = atoi(argv[1]);
	}
	if (Bluetooth_Entry.userreqtype == USER_REQ_SCH_ACCESS_AUTO_UNCONDITIONAL) {
		if (arg < 5) {
			printf("channel needed for unconditional access\n");
			exit(0);
		} else {
			Bluetooth_Entry.channel = atoi(argv[5]);
		}
	}

	DSRC_SEND_CHANNEL = Bluetooth_Entry.channel;
	Bluetooth_Entry.schaccess = atoi(argv[2]);
	Bluetooth_Entry.schextaccess = atoi(argv[3]);

	if (arg > 6) {
		/*
			strncpy(arg1.macaddr, argv[6], 17);
			Bluetooth_Set_Arguments(Bluetooth_Entry.macaddr, &arg1, ADDR_MAC);
		*/
	}

	/* if (invokeWAVEDevice(WAVEDEVICE_LOCAL, 0) < 0) {
		 printf("Open Failed. Quitting\n");
		 exit(-1);
	 }*/
	printf("Registering Bluetooth User %d\n", Bluetooth_Entry.psid);
	if (registerUser(pid, &Bluetooth_Entry) < 0) {
		printf("Register Bluetooth User Failed \n");
		printf("Removing Bluetooth user if already present  %d\n", !removeUser(pid, &Bluetooth_Entry));
		printf("Bluetooth USER Registered %d with PSID =%u \n", registerUser(pid, &Bluetooth_Entry),
		       Bluetooth_Entry.psid);
	}

}

void *Bluetooth_Main_Thread(void *arg) {

	pthread_t CurrentThread_ID = pthread_self();


	int i, err, sock, dev_id = -1;
	int num_rsp = 0, max_rsp = 5, flags = 0, length = 4;  /* [1.28 *<length>]seconds [1.28*4 = 5.12] seconds */
	char addr[19] = {0};
	char cmd[100], cmd1[50];
	char name[248] = {0}, bt_mac[19] = "00:00:00:00:00:00";
	uuid_t uuid = {0};
	int application_id = (int) *(int *) arg;
	char uuid_str[50];
	FILE *fd = NULL;
	if (application_id == 2) {
		printf("\nOpen application: Android Locomate Messaging\n");
		strcpy(uuid_str, "66841278-c3d1-11df-ab31-001de000a901");
	}
	else if (application_id == 3) {
		printf("\nOpen application: Spat Andriod Application\n");
		strcpy(uuid_str, "66841278-c3d1-11df-ab31-001de000a903");
	}
	else if (application_id == 4) {
		printf("\nOpen application: Bluetooth CAN application\n");
		strcpy(uuid_str, "00001101-0000-1000-8000-00805F9B34FB");
		fd = popen("cat /var/can.conf | grep BTCAN_MAC= | cut -d '=' -f 2", "r");
		if (fd != NULL) {
			fscanf(fd, "%s", bt_mac);
			pclose(fd);
			printf("Mac from conf file: %s\n", bt_mac);
		}
	}
	else {
		printf("\nOpen application: Locomate uSafety Application\n");
		strcpy(uuid_str, "66841278-c3d1-11df-ab31-001de000a902");
	}
	uint32_t range = 0x0000ffff;
	sdp_list_t *response_list = NULL, *search_list, *attrid_list;
	int status;
	int responses;
	int retries = 0;
	struct sockaddr_rc loc_addr = {0};
	signal(SIGINT, (void *) Bluetooth_Signal_Interrupt);

	/* find the bluetooth device is available or not */
	sprintf(cmd, "/usr/local/bin/hciconfig hci0 up");
	system(cmd);
	for (retries = 0; retries < 5; retries++) {
		dev_id = hci_get_route(NULL);
		if (dev_id < 0) {
			perror("No Bluetooth Adapter Available\n");
			sprintf(cmd, "/usr/local/bin/hciconfig hci0 down");
			system(cmd);
			sprintf(cmd1, "/usr/local/bin/hciconfig hci0 up");
			system(cmd1);
			printf("\nretry getting adapter : %d\n", retries);
		}
		else
			break;
	}
	if (retries == 5) {
		Bluetooth_Forward = -1;
		return NULL;
	}

	for (retries = 0; retries < 5; retries++) { //check for the socket
		sock = hci_open_dev(dev_id);
		if (sock < 0) {
			perror("HCI device open failed");
			retries++;
			printf("\nretries sock : %d\n", retries);
		}
		else
			break;
	}
	if (retries == 5) {
		Bluetooth_Forward = -2;
		return NULL;
	}

	for (retries = 0; retries < 5; retries++) { //check uuid is correct or not
		if (!String2UUID(uuid_str, &uuid)) {
			perror("Invalid UUID");
			retries++;
			printf("\nretries str2 uuid : %d\n", retries);
		}
		else
			break;
	}
	if (retries == 5) {
		Bluetooth_Forward = -3;
		return NULL;
	}

	//printf("\nBluetooth Adapter Found \n");
	Bluetooth_inquiry_info = (inquiry_info *) malloc(MAX_RSP * sizeof(inquiry_info));

	while (1) { // loop to check and establish connection with other device

		// printf("Start Checking for bluetooth connections \n");
		// if (Bluetooth_Connection_Established != FALSE) { Bluetooth_Send(); }
		while (Bluetooth_Connection_Established == FALSE) {

			bzero(Bluetooth_inquiry_info, (MAX_RSP * sizeof(inquiry_info)));

			num_rsp = hci_inquiry(dev_id, length, max_rsp, NULL, &Bluetooth_inquiry_info,
			                      flags); // inquire for how many devices are available
			if (num_rsp < 0) {
				perror("Inquiry failed");
				sched_yield();
				sleep(1);
				continue;
			}
			printf("Inquiry devices found : %d\n", num_rsp);

			Bluetooth_Loco_Channel = -1;
			for (i = 0; i < num_rsp; i++) {
				sdp_session_t *session;
				ba2str(&(Bluetooth_inquiry_info + i)->bdaddr, addr);
				printf("\nFound Mac: %s ", addr);
				if (application_id == 4 && strcmp("00:00:00:00:00:00", bt_mac)) {
					// check for appid and mac_id
					if (strcasecmp(addr, bt_mac)) { continue; }
				}
				memset(name, 0, sizeof(name));

				if (hci_read_remote_name(sock, &(Bluetooth_inquiry_info + i)->bdaddr, sizeof(name), name, 8000) <
				    0) //get devices by name
					strcpy(name, "[unknown]");


				printf("Found : %s name : [[ %s ]]\n", addr, name);
				// connect to the SDP server running on the remote machine
				session = NULL;
				retries = 0;
				while (!session) {
					session = sdp_connect(BDADDR_ANY, &(Bluetooth_inquiry_info + i)->bdaddr, 0);
					if (session) { break; }
					if (errno != 0) {
						fprintf(stderr, "sdp_connect failed error no %d : %s \n", errno, strerror(errno));
					}
					if ((retries < 2) && ((errno == EALREADY))) {
						retries++;
						fprintf(stderr, "Retry sdp_connect %d\t", retries);
						sched_yield();
						usleep(300000);//300 ms
						continue; //continue till 3 times
					}
					break;
				} /* while(!session) */
				if (session == NULL) {
					if (i < (num_rsp - 1)) { printf("Trying next device -> %d\n", i + 2); }
					continue;
				}

				search_list = NULL;
				attrid_list = NULL;
				response_list = NULL;

				search_list = sdp_list_append(NULL, &uuid); //append list of uuids
				attrid_list = sdp_list_append(NULL, &range); // append list of attributes
				err = 0;
				err = sdp_service_search_attr_req(session, search_list, SDP_ATTR_REQ_RANGE, attrid_list,
				                                  &response_list); //search for attributes from list
				sdp_list_t *r = response_list;
				responses = 0;


				// go through each of the service records
				for (; r; r = r->next) {
					responses++;
					sdp_record_t *rec = (sdp_record_t *) r->data;
					sdp_list_t *proto_list;

					// get a list of the protocol sequences
					if (sdp_get_access_protos(rec, &proto_list) == 0) {
						sdp_list_t *p = proto_list;

						// go through each protocol sequence
						for (; p; p = p->next) {
							sdp_list_t *pds = (sdp_list_t *) p->data;

							// go through each protocol list of the protocol sequence
							for (; pds; pds = pds->next) {

								// check the protocol attributes
								sdp_data_t *d = (sdp_data_t *) pds->data;
								int proto = 0;
								for (; d; d = d->next) {
									switch (d->dtd) {
										case SDP_UUID16:
										case SDP_UUID32:
										case SDP_UUID128:
											proto = sdp_uuid_to_proto(&d->val.uuid);
											break;
										case SDP_UINT8:
											if (proto == RFCOMM_UUID) {
												printf("rfcomm channel: %d\n", d->val.int8);
												Bluetooth_Loco_Channel = d->val.int8;
											}
											break;
									} /* switch(t->dtd) */
								} /* for( ; d; d = d->next) */
							} /* for( ; pds ; pds = pds->next) */
							sdp_list_free((sdp_list_t *) p->data, 0);
						} /* for( ; p; p = p->next) */
						sdp_list_free(proto_list, 0);
					} /* if(sdp_get_access_protos(rec, &proto_list)) */
					sdp_record_free(rec);
					if (Bluetooth_Loco_Channel > 0) {
						break;
					}
				} /* for (; r; r = r->next) */


				sdp_list_free(response_list, 0);
				sdp_list_free(search_list, 0);
				sdp_list_free(attrid_list, 0);
				printf("No of services= %d on device %d \n", responses, i + 1);
				if (Bluetooth_Loco_Channel > 0) {
					// printf("Found Locomate Safety Application on device: name [%s], sending message now\n",name);
					Bluetooth_Socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
					loc_addr.rc_family = AF_BLUETOOTH;
					loc_addr.rc_channel = Bluetooth_Loco_Channel;
					loc_addr.rc_bdaddr = (Bluetooth_inquiry_info + i)->bdaddr;
					status = connect(Bluetooth_Socket, (struct sockaddr *) &loc_addr, sizeof(loc_addr));
					if (status < 0) {
						perror("\nuh oh: Btooth socket not created\n");
						Bluetooth_Forward = -5;
					}
					else {
						sdp_close(session);
						Bluetooth_Forward = 1;
						Bluetooth_Connection_Established = TRUE;
						break;
					}
				}
				sdp_close(session);
			} /* for (i = 0; i < num_rsp; i++) */
			if (Bluetooth_Connection_Established == FALSE) {
				printf("Scanning again\n");
				//sprintf(cmd, "/usr/local/bin/hciconfig hci0 down");
				//system(cmd);
				sprintf(cmd1, "/usr/local/bin/hciconfig hci0 up");
				system(cmd1);
				sched_yield();
				sleep(1);
			}
			else {
				printf("***Connection established***\n");

				Bluetooth_ConnectionStatus = BluetoothIsConnected;

				int DSRC_SEND_THREAD_RET = -1;
				int DSRC_RECEIVE_THREAD_RET = -1;

				int DSRC_SEND_THREAD_arg = 2;
				int DSRC_RECEIVE_THREAD_arg = 2;
				DSRC_SEND_THREAD_RET = pthread_create(&DSRC_SEND_Thread_Pointer, NULL, DSRC_SEND_Thread,
				                                      (void *) &DSRC_SEND_THREAD_arg);
				DSRC_RECEIVE_THREAD_RET = pthread_create(&DSRC_RECEIVE_Thread_Pointer, NULL, DSRC_RECEIVE_Thread,
				                                         (void *) &DSRC_RECEIVE_THREAD_arg);

				// SendReceive(DSRC_Channel);

				// sched_yield();
			}
		} /* while(Bluetooth_Connection_Established == ...) */
		// sched_yield();
		//printf("wait for 3 seconds, before next check of bluetooth connection \n");
		sleep(3); // wait for 3seconds, before next check

	}

}