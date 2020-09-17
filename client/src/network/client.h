#pragma once
#include "../../../common/src/network/packet.h"

#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

extern int    connectionTries;
extern size_t sentBytesCurrentSession;
extern size_t recvBytesCurrentSession;
extern size_t recvUncompressedBytesCurrentSession;

void clientInit              ();
void clientFree              ();
void clientGetName           ();
void clientGoodbye           ();
void clientGreetServer       ();
void clientSendAllToServer   ();
void closeSingleplayerServer ();
void decompressPacket        (const packet *p);
void clientParsePacket       (const packet *p);

void clientHandleEvents      ();
void queueToServer           (const void *data, unsigned int len);
