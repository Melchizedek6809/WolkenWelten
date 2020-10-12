#define _GNU_SOURCE

#include "command.h"

#include "../game/animal.h"
#include "../game/blockMining.h"
#include "../game/entity.h"
#include "../game/itemDrop.h"
#include "../network/server.h"
#include "../../../common/src/network/messages.h"
#include "../../../common/src/misc/misc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char replyBuf[256];

static void cmdDmg(int c, const char *cmd){
	int dmg = 4;
	int target = c;
	int argc;
	char **argv;

	argv = splitArgs(cmd,&argc);
	if(argc > 1){
		int tmp = getClientByName(argv[1]);
		if(tmp >= 0){
			target = tmp;
		}else{
			snprintf(replyBuf,sizeof(replyBuf),".dmg : Can't find '%s'",cmd+4);
			replyBuf[sizeof(replyBuf)-1]=0;
			serverSendChatMsg(replyBuf);
		}
	}
	if(argc > 2){
		dmg = atoi(argv[2]);
	}

	msgPlayerDamage(target,dmg,target,0,-1,vecZero());
}

static void cmdDie(int c, const char *cmd){
	int cmdLen = strnlen(cmd,252);
	int target = c;
	if((cmdLen > 3) && (cmd[3] == ' ')){
		target = getClientByName(cmd+4);
		if(target < 0){
			snprintf(replyBuf,sizeof(replyBuf),".die : Can't find '%s'",cmd+4);
			replyBuf[sizeof(replyBuf)-1]=0;
			serverSendChatMsg(replyBuf);
		}
	}
	msgPlayerDamage(target,1000,target,0,-1,vecZero());
}

static void cmdDbgitem(int c, const char *cmd){
	int cmdLen = strnlen(cmd,252);
	int target = c;
	if((cmdLen > 3) && (cmd[3] == ' ')){
		target = getClientByName(cmd+4);
		if(target < 0){
			snprintf(replyBuf,sizeof(replyBuf),".dbgitem : Can't find '%s'",cmd+4);
			replyBuf[sizeof(replyBuf)-1]=0;
			serverSendChatMsg(replyBuf);
		}
	}
	msgPickupItem(target,261,1);
	msgPickupItem(target,262,1);
	msgPickupItem(target,263,1);
	msgPickupItem(target,264,1);
	msgPickupItem(target,258,42);
	msgPickupItem(target,256,99);

	msgPickupItem(target,265,999*4);
}

static void cmdHeal(int c, const char *cmd){
	int dmg = 128;
	int target = c;
	int argc;
	char **argv;

	argv = splitArgs(cmd,&argc);
	if(argc > 1){
		int tmp = getClientByName(argv[1]);
		if(tmp >= 0){
			target = tmp;
		}else{
			snprintf(replyBuf,sizeof(replyBuf),".heal : Can't find '%s'",argv[1]);
			replyBuf[sizeof(replyBuf)-1]=0;
			serverSendChatMsg(replyBuf);
		}
	}
	if(argc > 2){
		dmg = atoi(argv[2]);
	}

	msgPlayerDamage(target,-dmg,target,0,-1,vecZero());
}

static void cmdGive(int c, const char *cmd){
	int amount = 1;
	int id = 0;
	int target = c;
	int argc;
	char **argv;

	argv = splitArgs(cmd,&argc);
	if(argc == 1){
		snprintf(replyBuf,sizeof(replyBuf),".give ID [PLAYER] [AMOUNT]");
		replyBuf[sizeof(replyBuf)-1]=0;
		serverSendChatMsg(replyBuf);
		return;
	}
	if(argc > 1){
		id = atoi(argv[1]);
		if(id <= 0){
			snprintf(replyBuf,sizeof(replyBuf),".give: error with ID %i",id);
			replyBuf[sizeof(replyBuf)-1]=0;
			serverSendChatMsg(replyBuf);
			return;
		}
	}
	if(argc > 2){
		int tmp = getClientByName(argv[2]);
		if(tmp >= 0){
			target = tmp;
		}
	}
	if(argc > 3){
		amount = atoi(argv[3]);
		if(amount <= 0){
			snprintf(replyBuf,sizeof(replyBuf),".give: error with amount %i",amount);
			replyBuf[sizeof(replyBuf)-1]=0;
			serverSendChatMsg(replyBuf);
			return;
		}
	}

	msgPickupItem(target,id,amount);
}

static void cmdTp(int c, const char *cmd){
	(void)c;
	int argc;
	char **argv;
	int target = c;

	argv = splitArgs(cmd,&argc);
	if(argc != 4){
		snprintf(replyBuf,sizeof(replyBuf),".tp X Y Z [PLAYER] : You need to pass 3 integer values");
		replyBuf[sizeof(replyBuf)-1]=0;
		serverSendChatMsg(replyBuf);
		return;
	}
	if(argc > 4){
		int tmp = getClientByName(argv[4]);
		if(tmp >= 0){
			target = tmp;
		}
	}
	const vec pos = vecNew(atof(argv[1]),atof(argv[2]),atof(argv[3]));
	msgPlayerSetPos(target,pos,vecZero());
}

static void cmdTpr(int c, const char *cmd){
	(void)c;
	int argc;
	char **argv;
	int target = c;

	argv = splitArgs(cmd,&argc);
	if(argc < 4){
		snprintf(replyBuf,sizeof(replyBuf),".tpr X Y Z [PLAYER] : You need to pass 3 integer values");
		replyBuf[sizeof(replyBuf)-1]=0;
		serverSendChatMsg(replyBuf);
		return;
	}
	if(argc > 4){
		int tmp = getClientByName(argv[4]);
		if(tmp >= 0){
			target = tmp;
		}
	}
	const vec pos = vecNew(atof(argv[1]),atof(argv[2]),atof(argv[3]));
	msgPlayerSetPos(target,vecAdd(pos,clients[target].c->pos),vecZero());
}

void cmdAni(int c, const char *cmd){
	(void)cmd;

	character *ch = clients[c].c;
	if(ch == NULL){return;}
	animalNew(vecAdd(ch->pos,vecDegToVec(ch->rot)),1);
}

void cmdHelp(int c, const char *cmd){
	(void)c;
	(void)cmd;
	serverSendChatMsg(" --=-- HELP --=--");
	serverSendChatMsg(".help = This help message");
	serverSendChatMsg(".ani  = Spawns an animal");
	serverSendChatMsg(".dmg [PLAYER] [DMG] = Damages a player");
	serverSendChatMsg(".dbgitem [PLAYER] = Gives a player some things");
	serverSendChatMsg(".die [PLAYER] = Kills a player");
	serverSendChatMsg(".heal [PLAYER] [HEAL] = Heals a player");
	serverSendChatMsg(".give ID [PLAYER] [QTY] = Heals a player");
	serverSendChatMsg(".tpr X Y Z [PLAYER} = Teleports a player to a relative position");
	serverSendChatMsg(".tp X Y Z [PLAYER} = Teleports a player to an absolute position");
}

int parseCommand(int c, const char *cmd){
	if(cmd[0] != '.'){return 0;}
	const char *tcmp = cmd+1;

	if(strncmp(tcmp,"help",4) == 0){
		cmdHelp(c,tcmp);
		return 1;
	}
	if(strncmp(tcmp,"ani",3) == 0){
		cmdAni(c,tcmp);
		return 1;
	}

	if(strncmp(tcmp,"dmg",3) == 0){
		cmdDmg(c,tcmp);
		return 1;
	}

	if(strncmp(tcmp,"dbgitem",3) == 0){
		cmdDbgitem(c,tcmp);
		return 1;
	}

	if(strncmp(tcmp,"die",3) == 0){
		cmdDie(c,tcmp);
		return 1;
	}

	if(strncmp(tcmp,"heal",4) == 0){
		cmdHeal(c,tcmp);
		return 1;
	}

	if(strncmp(tcmp,"give",4) == 0){
		cmdGive(c,tcmp);
		return 1;
	}

	if(strncmp(tcmp,"tpr",3) == 0){
		cmdTpr(c,tcmp);
		return 1;
	}

	if(strncmp(tcmp,"tp",2) == 0){
		cmdTp(c,tcmp);
		return 1;
	}

	if(strncmp(tcmp,"acount",2) == 0){
		snprintf(replyBuf,sizeof(replyBuf),".acount : %i",animalCount);
		replyBuf[sizeof(replyBuf)-1]=0;
		serverSendChatMsg(replyBuf);
		return 1;
	}

	if(strncmp(tcmp,"bmcount",2) == 0){
		snprintf(replyBuf,sizeof(replyBuf),".bmcount : %i",blockMiningGetActive());
		replyBuf[sizeof(replyBuf)-1]=0;
		serverSendChatMsg(replyBuf);
		return 1;
	}

	if(strncmp(tcmp,"idcount",2) == 0){
		snprintf(replyBuf,sizeof(replyBuf),".idcount : %i",itemDropGetActive());
		replyBuf[sizeof(replyBuf)-1]=0;
		serverSendChatMsg(replyBuf);
		return 1;
	}

	if(strncmp(tcmp,"ecount",2) == 0){
		snprintf(replyBuf,sizeof(replyBuf),".ecount : %i",entityCount);
		replyBuf[sizeof(replyBuf)-1]=0;
		serverSendChatMsg(replyBuf);
		return 1;
	}

	fprintf(stderr,"CMD [%s]\n",cmd);
	return 1;
}
