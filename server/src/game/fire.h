#pragma once
#include "../../../common/src/game/fire.h"

void  fireIntro      (uint c);
void  fireRecvUpdate (uint c, const packet *p);
fire *fireGetAtPos   (u16 x,u16 y, u16 z);
void  fireUpdateAll  ();
void  fireSyncPlayer (uint c);
