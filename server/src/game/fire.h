#pragma once
#include "../../../common/src/game/fire.h"

void  fireNewF       (u16 x, u16 y, u16 z, i16 strength, i16 blockDmg);
void  fireIntro      (uint c);
void  fireRecvUpdate (uint c, const packet *p);
void  fireUpdateAll  ();
void  fireSyncPlayer (uint c);
