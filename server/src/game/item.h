#pragma once
#include "../../../common/src/common.h"

item  itemNew           (uint16_t ID, int16_t amount);
item  itemEmpty         ();
void  itemDiscard       (item *i);
bool  itemIsEmpty       (item *i);
int   itemBlockDamage   (item *i, blockCategory cat);
bool  itemIsSingle      (item *i);
bool  itemHasMineAction (item *i);
bool  itemMineAction    (item *i, character *chr, int to);
bool  itemActivate      (item *i, character *chr);
bool  itemActivateBlock (item *i, character *chr);
int   itemCanStack      (item *i, uint16_t ID);
int   itemIncStack      (item *i, int16_t amount);
int   itemDecStack      (item *i, int16_t amount);
int   itemGetAmmo       (item *i);
int   itemIncAmmo       (item *i, int16_t amount);
int   itemDecAmmo       (item *i, int16_t amount);
