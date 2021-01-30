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

#include "throwable.h"

#include "entity.h"

#include <stdio.h>
void throwableNew(const vec pos, const vec rot, float speed, const item itm, being thrower, u16 flags){
	throwable *a = throwableAlloc();
	a->ent       = entityNew(pos,rot);
	a->ent->vel  = vecMulS(vecDegToVec(rot),speed);
	a->itm       = itm;
	a->flags     = flags;
	a->counter   = 0;
	a->nextFree  = -1;
	a->thrower   = thrower;
	throwableSendUpdate(-1,a - throwableList);
}
