//
// Created by trl on 2/22/16.
//
#include <stdio.h>
#include <ctype.h>
#include <termio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <time.h>
#include <signal.h>
#include <asnwave.h>
#include "wave.h"
#include <asnwave.h>


enum {
    SEND_DSRC_MESSAGE = 0, RECEIVE_DSRC_MESSAGE
};

static WMEApplicationRequest DSRC_RX_Entry;
static WMEApplicationRequest DSRC_TX_Entry;

static long long MyID_DSRC = 0;

void DSRC_Signal_Interrupt(void);

void DSRC_Signal_Terminate(void);

int DSRC_ConfirmBeforeJoin(WMEApplicationIndication *appind);

void DSRC_Set_Arguments(void *data, void *argname, int datatype);

int Extract_MAC_Address(u_int8_t *mac, char *str);

int Initialize_DSRC_RX_Environment(int arg, char *argv[]);

int Initialize_DSRC_TX_Environment(int arg, char *argv[]);

char *Dec2Hex(short Number);

int UpdateIDBytes_______(double Value, BasicSafetyMessage_t *bsm, int Start);

int UpdatePositionBytes_(double Value, BasicSafetyMessage_t *bsm, int Start);

int UpdateElevationBytes(double Value, BasicSafetyMessage_t *bsm, int Start);

int UpdateSpeedBytes____(double Value, BasicSafetyMessage_t *bsm, int Start);

int UpdateDirectionBytes(double Value, BasicSafetyMessage_t *bsm, int Start);

long long Current_TimeStamp();


int FillGPSInfo(BasicSafetyMessage_t *bsm);

int BuildMessage();

int TransmitMessage();

int Send_DSRC_Message();

int Receive_DSRC_Message();

int SendReceive();
