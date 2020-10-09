#pragma once

#include "../../../common/src/common.h"

#define CHUNGUS_SIZE (16*16)

struct chungus {
	int    x,y,z;
	ivec   spawn;
	u64    clientsSubscribed;
	u64    clientsUpdated;
	void  *nextFree;
	chunk *chunks[16][16][16];
};

chungus     *chungusNew              (int x,int y, int z);
void         chungusFree             (chungus *c);
void         chungusRoughBox         (chungus *c, int x, int y, int z, int w, int h, int d, u8 block);
void         chungusBox              (chungus *c, int x, int y, int z, int w, int h, int d, u8 block);
void         chungusBoxIfEmpty       (chungus *c, int x, int y, int z, int w, int h, int d, u8 block);
void         chungusBoxF             (chungus *c, int x, int y, int z, int w, int h, int d, u8 block);
void         chungusBoxSphere        (chungus *c, int x, int y, int z, int r, u8 block);
void         chungusFill             (chungus *c, int x, int y, int z, u8 block);
void         chungusSetB             (chungus *c, int x, int y, int z, u8 block);
u8           chungusGetB             (chungus *c, int x, int y, int z);
chunk       *chungusGetChunk         (chungus *c, int x, int y, int z);
void         chungusSetSpawn         (chungus *c, const ivec spawn);
void         chungusSubscribePlayer  (chungus *c, uint p);
int          chungusUnsubscribePlayer(chungus *c, uint p);
uint         chungusIsSubscribed     (chungus *c, uint p);
int          chungusUpdateClient     (chungus *c, uint p);
uint         chungusIsUpdated        (chungus *c, uint p);
void         chungusSetUpdated       (chungus *c, uint p);
void         chungusUnsetUpdated     (chungus *c, uint p);
int          chungusGetHighestP      (chungus *c, int x, int *retY, int z);
chungus     *chungusGetActive        (uint i);
void         chungusSetActiveCount   (uint i);
uint         chungusGetActiveCount   ();
