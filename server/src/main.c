#include "main.h"

#include "misc/command.h"
#include "misc/options.h"
#include "game/animal.h"
#include "game/being.h"
#include "game/blockMining.h"
#include "game/fire.h"
#include "game/itemDrop.h"
#include "game/grenade.h"
#include "game/time.h"
#include "game/water.h"
#include "persistence/savegame.h"
#include "voxel/bigchungus.h"
#include "voxel/chungus.h"
#include "voxel/chunk.h"
#include "network/server.h"
#include "../../common/nujel/nujel.h"
#include "../../common/src/tmp/cto.h"
#include "../../common/src/misc/misc.h"
#include "../../common/src/game/blockType.h"
#include "../../common/src/game/projectile.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

int playerID  = 64;
bool isClient = false;
bool quit = false;
char *termColors[16];
char *termReset;
char *nop = "";

u64 getTicks(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (tv.tv_usec / 1000) + (tv.tv_sec * 1000);
}

void signalQuit(int signo){
	(void)signo;
	quit = true;
}

void initSignals(){
	#ifdef __MINGW32__
	signal(SIGTERM, signalQuit);
	signal(SIGABRT, signalQuit);
	#else
	signal(SIGPIPE, SIG_IGN);
	signal(SIGKILL, signalQuit);
	signal(SIGSTOP, signalQuit);
	signal(SIGHUP,  signalQuit);
	#endif
	signal(SIGINT,  signalQuit);
}

static void initTermColors(){
	char *clicolor = getenv("CLICOLOR");
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if((clicolor == NULL) || (*clicolor == 0) || (atoi(clicolor) == 0)){
		termReset = "";
		for(int i=0;i<16;i++){
			termColors[i] = nop;
		}
	}else{
		termReset = ansiRS;
		for(int i=0;i<16;i++){
			termColors[i] = ansiFG[i];
		}
	}
}

static void updateWorldStep(){
	blockMiningUpdate();
	itemDropUpdateAll();
	grenadeUpdateAll();
	animalUpdateAll();
	projectileUpdateAll();
	animalThinkAll();
	fireUpdateAll();
	waterUpdateAll();
	itemDropUpdateFireAll();
	animalNeedsAll();
	animalCheckBurnAll();
	gtimeUpdate();
}

int updateWorld(){
	static u64 lastUpdate  = 0;
	const u64 cTicks = getTicks();
	if(lastUpdate  == 0){lastUpdate  = getTicks() - 4;}

	int i = 64;
	for(;lastUpdate < cTicks;lastUpdate += 4){
		updateWorldStep();
		if(--i == 0){break;}
	}
	if(lastUpdate < cTicks){return 0;}
	return 1;
}

void handleAnimalPriorities(){
	static u64 lastCall   = 0;
	static uint c = 0;
	const u64 cTicks = getTicks();
	if(cTicks < lastCall + 200) {return;}
	c = (c+1) & 0x1F;
	if(clients[c].state)        {return;}
	if(clients[c].c == NULL)    {return;}
	lastCall = cTicks;
	animalUpdatePriorities(c);
}

void mainTick(){
	freeTime = getTicks();
	chungusUnsubFarChungi();
	chungusFreeOldChungi(30000);
	handleAnimalPriorities();
	bigchungusSafeSave(&world);
	serverHandleEvents();
	updateWorld();
	serverHandleEvents();
}

void mainInit(){
	gtimeSetTime(1<<19);
	savegameLoad();
	seedRNG(time(NULL));
	serverInit();
	beingListEntryInit();
	chungusInit();
	chunkInit();
	initCommands();
	savegameSave();

	bigchungusInit(&world);
	blockTypeInit();
	if(optionProfileWG){
		bigchungusGenHugeSpawn(&world);
	}else{
		bigchungusGenSpawn(&world);
	}
}

static void checkSPQuit(){
	static u64 startTime = 0;
	if(!optionSingleplayer){return;}
	if(startTime == 0){startTime = getTicks();}
	if(clientCount > 0){return;}
	if(getTicks() > startTime + 1000){quit = true;}
}

#ifndef __EMSCRIPTEN__
int main( int argc, const char* argv[] ){
	initSignals();
	initTermColors();
	initOptions(argc,argv);
	mainInit();

	printf("%sWolkenwelten",termColors[2]                );
	printf(" %s%s"         ,termColors[6],VERSION        );
	printf(" %s[%.16s]"    ,termColors[3],COMMIT         );
	printf(" %sSeed[%u]"   ,termColors[4],optionWorldSeed);
	printf(" %s{%s}"       ,termColors[5],optionSavegame );
	printf(" %sbuilt %s\n" ,termReset    ,BUILDDATE      );

	while(!quit){
		if(optionProfileWG){quit = true;}
		if(clientCount == 0){checkSPQuit();}
		mainTick();
		usleep(1000);
	}

	printf("[SRV] Exiting cleanly\n");
	savegameSave();
	serverFree();
	return 0;
}
#endif
