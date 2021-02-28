/*
 * Wolkenwelten - Copyright (C) 2020-2021 - Benjamin Vincent Schulenburg
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "options.h"

#include "../main.h"
#include "../gui/menu.h"
#include "../gfx/gfx.h"
#include "../tmp/assets.h"
#include "../misc/lisp.h"
#include "../sdl/sdl.h"
#include "../../../common/src/misc/misc.h"
#include "../../../common/src/tmp/cto.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

char playerName[28];
char serverName[64];
char optionSavegame[32];
char optionExecPath[256];

float optionSoundVolume      = 0.5f;
int   optionWorldSeed        = 0;
bool  optionDebugInfo        = false;
bool  optionRuntimeReloading = false;
bool  optionNoSave           = false;
bool  optionMute             = false;
float optionMouseSensitivy   = 0.5f;
bool  optionThirdPerson      = false;
bool  optionWireframe        = false;
int   optionWindowWidth      = 800;
int   optionWindowHeight     = 800;
int   optionWindowX          = -1;
int   optionWindowY          = -1;
bool  optionFullscreen       = false;


void printVersion(){
	printf("Wolkenwelten Pre-Alpha\n");
	printf("Version:   %s\n",VERSION);
	printf("Builddate: %s\n",BUILDDATE);
	printf("Commit:    %s\n",COMMIT);
	printf("\nAttribution:\n%s\n",txt_attribution_txt_data);
}

int checkString(const char *a, const char *b){
	int len = strlen(b);
	if(strncmp(a,b,len) == 0){return 1+len;}
	if(a[0] == '-' && (strncmp(a+1,b,len) == 0)){return 2+len;}
	return 0;
}

void getExecPath(const char *argvZero){
	char cwdBuf[512];
	char *cwd = getcwd(cwdBuf,sizeof(cwdBuf));
	int ll;

	if((argvZero[0] == '.') && (argvZero[1] == '/')){
		ll = snprintf(optionExecPath,sizeof(optionExecPath),"%s/%s",cwd,argvZero+2);
	}else{
		ll = snprintf(optionExecPath,sizeof(optionExecPath),"%s",argvZero);
	}
	for(;(ll>0);ll--){
		if(optionExecPath[ll] != '/'){continue;}
		optionExecPath[ll] = 0;
		return;
	}
}

void parseOptions(int argc,char *argv[]){
	int tmp,l;
	getExecPath(argv[0]);

	for(int i=1;i<argc;i++){
		if(argv[i][0] != '-'){continue;}

		if((l = checkString(argv[i]+1,"soundVolume="))){
			tmp = atoi(argv[i]+l);
			optionSoundVolume = (float)tmp / 100.f;
		}
		if((l = checkString(argv[i]+1,"worldSeed="))){
			optionWorldSeed = atoi(argv[i]+l);
		}
		if((l = checkString(argv[i]+1,"debugInfo="))){
			optionDebugInfo = atoi(argv[i]+l) != 0;
		}
		if((l = checkString(argv[i]+1,"mute="))){
			optionMute = atoi(argv[i]+l) != 0;
		}
		if((l = checkString(argv[i]+1,"playerName="))){
			strncpy(playerName,argv[i]+l,sizeof(playerName)-1);
		}
		if((l = checkString(argv[i]+1,"serverName="))){
			strncpy(serverName,argv[i]+l,sizeof(serverName)-1);
		}
		if((l = checkString(argv[i]+1,"savegame="))){
			strncpy(optionSavegame,argv[i]+l,sizeof(optionSavegame)-1);
			optionSavegame[sizeof(optionSavegame)-1]=0;
		}

		if(checkString(argv[i]+1,"noSave")){
			optionNoSave = true;
		}
		if(checkString(argv[i]+1,"version")){
			printVersion();
			exit(0);
		}
	}
}

void sanityCheckOptions(){
	if(optionSoundVolume < 0.f){
		optionSoundVolume = 0.f;
		printf("Using Minimum soundVolume of 0\n");
	}
	if(optionSoundVolume > 1.f){
		optionSoundVolume = 1.f;
		printf("Using Maximum soundVolume of 100\n");
	}
	if(optionWindowWidth <= 0){
		optionWindowWidth = 800;
	}
	if(optionWindowHeight <= 0){
		optionWindowHeight = 480;
	}
}

void loadOptionFile(const char *fname){
	size_t len = 0;
	char *b = loadFile(fname,&len);
	if((b == NULL) || (len == 0)){return;}
	lispEval(b);
	free(b);
}

void loadOptions(){
	#ifdef __EMSCRIPTEN__
	return;
	#endif
	if(optionNoSave){return;}
	loadOptionFile("client.nuj");
	loadOptionFile("custom.nuj");
	sanityCheckOptions();
}

void saveOptions(){
	static char buf[4096];
	char *b;
	#ifdef __EMSCRIPTEN__
	return;
	#endif
	if(optionNoSave){return;}

	b  = buf;
	b += snprintf(b,sizeof(buf)-(b-buf),";; This file gets automatically overwritten by the client.\n;; Do not edit this file, your changes will get lost!\n;; \n;; Instead, write everything into custom.nuj which the client will never change, only read and eval.\n\n");
	b += snprintf(b,sizeof(buf)-(b-buf),"(conf-v1!\n");
	b += snprintf(b,sizeof(buf)-(b-buf),"  (player-name!  \"%s\")\n",playerName);
	b += snprintf(b,sizeof(buf)-(b-buf),"  (sound-vol!    %f)\n",optionSoundVolume);
	b += snprintf(b,sizeof(buf)-(b-buf),"  (view-dist!    %f)\n",renderDistance);
	b += snprintf(b,sizeof(buf)-(b-buf),"  (mouse-sens!   %f)\n",optionMouseSensitivy);
	if(optionFullscreen){
		b += snprintf(b,sizeof(buf)-(b-buf),"  (fullscreen #t)\n");
	}else{
		b += snprintf(b,sizeof(buf)-(b-buf),"  (windowed %u %u %u %u)\n",screenWidth,screenHeight,getWindowX(),getWindowY());
	}
	b += snprintf(b,sizeof(buf)-(b-buf),"  (third-person! %s)\n",optionThirdPerson ? "#t" : "#f");
	b += snprintf(b,sizeof(buf)-(b-buf),"  (debug-info!   %s)",optionDebugInfo ? "#t" : "#f");
	for(int i=0;i<serverlistCount;i++){
		b += snprintf(b,sizeof(buf)-(b-buf),"\n  (server-add! \"%s\" \"%s\")",serverlistIP[i],serverlistName[i]);
	}
	b += snprintf(b,sizeof(buf)-(b-buf),")\n\n");

	saveFile("client.nuj",buf,strlen(buf));
}

void initOptions(int argc,char *argv[]){
	parseOptions(argc,argv);
	sanityCheckOptions();
}
