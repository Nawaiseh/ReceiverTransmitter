# Copyright (c) 2005-2007 Arada Syatems, Inc. All rights reserved.

# Proprietary and Confidential Material.

#TOOLCHAIN=/opt/build/toolchain/PB44/buildroot/build_mips/staging_dir/usr/bin
#CC=mips-linux-uclibc-gcc
include ./makefile_tlcn.inc

export PATH:=$(TOOLCHAIN):/${PATH}
OBJDIR= ./../..

INCS+= -I ${OBJDIR}/incs

INCS+= -I ${OBJDIR}/src

BINDIR= ${OBJDIR}/bin

LIBDIR= ${OBJDIR}/lib


ALL= ReceiverTransmitter_Bluetooth


all:	${ALL}
	mv -f ${ALL} ${OBJDIR}/bin

%.o: %.c
	$(CC) $(INCS) -c -DLOCOMATE -DSDK_NEW -o $@ $<

LDFLAGS= -lm -lpthread -lwave -lbluetooth -L $(LIBDIR) -Xlinker -rpath -Xlinker $(LIBDIR) -lwave-encdec -Lasn1c/

ReceiverTransmitter_Bluetooth: ReceiverTransmitter_Bluetooth.o Bluetooth_Handler.o DSRC_Handler.o GPS_Handler.o
	${CC} -g  ${LDFLAGS} -o ReceiverTransmitter_Bluetooth ReceiverTransmitter_Bluetooth.o Bluetooth_Handler.o DSRC_Handler.o GPS_Handler.o

clean:
	rm -f *.o
	cd ${BINDIR}; rm -f ReceiverTransmitter_Bluetooth; cd -
