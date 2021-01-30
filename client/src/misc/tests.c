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

#include "tests.h"

#include "../main.h"
#include "../game/character.h"
#include "../game/projectile.h"
#include "../gui/gui.h"
#include "../misc/options.h"
#include "../network/client.h"
#include "../sdl/sdl.h"
#include "../../../common/src/misc/misc.h"

#include <math.h>
#include <stdio.h>

static vec doAutomaticWorldGenTest(const vec nv){
	(void)nv;
	static int iter=0;
	vec rv = vecZero();
	iter++;
	if(getTicks() > 30000){quit=true;}

	player->flags |= CHAR_NOCLIP;
	characterRotate(player,vecNew(1.f,cosf(iter/16.f)/4.f,0));
	rv.y = 0.5f;

	return rv;
}

static vec doAutomaticFireTest(const vec nv){
	(void)nv;
	static int iter=0;
	vec rv = vecZero();
	if(iter++ == 0){
		player->rot.y = 30.f;
	}
	if(iter < 512){
		projectileNewC(player, 0, 1);
	}
	if(getTicks() > 120000){
		const float compRatio = (float)recvUncompressedBytesCurrentSession / (float)recvBytesCurrentSession;
		printf("Bytes Recvd : %sB\n",getHumanReadableSize(recvBytesCurrentSession));
		printf("Uncompressed: %sB\n",getHumanReadableSize(recvUncompressedBytesCurrentSession));
		printf("Comp. Ratio : %s%2.2fX\n",colorSignalHigh(4,8,15,compRatio),compRatio);
		quit=true;
	}

	player->flags |= CHAR_NOCLIP;
	player->pos.y = 4550.f;
	characterRotate(player,vecNew(1.f,cosf(iter/16.f)/4.f,0));

	return rv;
}

vec doAutomatedupdate(const vec nv){
	switch(optionAutomatedTest){
	case 0:
	default:
		return nv;
	case 1:
		return doAutomaticWorldGenTest(nv);
	case 2:
		return doAutomaticFireTest(nv);
	}
}
