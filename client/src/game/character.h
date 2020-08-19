#pragma once
#include "../../../common/src/common.h"

extern character *player;

character *characterNew       ();
void  characterFree           (character *c);
bool  characterLOSBlock       (character *c, int *retX,int *retY,int *retZ,int returnBeforeBlock);
void  characterPrimary        (character *c);
void  characterSecondary      (character *c);
void  characterTertiary       (character *c);
void  characterStopMining     (character *c);
float characterMineProgress   (character *c);
void  characterDie            (character *c);
bool  characterHP             (character *c, int addhp);
int   characterGetHP          (character *c);
int   characterGetMaxHP       (character *c);
bool  characterDamage         (character *c, int hp);
int   characterGetItemAmount  (character *c, uint16_t itemID);
int   characterDecItemAmount  (character *c, uint16_t itemID,int amount);
bool  characterPickupItem     (character *c, uint16_t itemID,int amount);
void  characterSetActiveItem  (character *c, int i);
item *characterGetItemBarSlot (character *c, int i);
void  characterSetItemBarSlot (character *c, int i, item *itm);
void  characterDropItem       (character *c, int i);
void  characterSwapItemSlots  (character *c, int a,int b);
void  characterMove           (character *c, float mx,float my,float mz);
void  characterRotate         (character *c, float vYaw,float vPitch,float vRoll);
void  characterUpdate         (character *c);
void  characterFireHook       (character *c);
void  characterFreeHook       (character *c);
void  characterSetPos         (character *c, float x, float y, float z);
void  characterSetVelocity    (character *c, float vx, float vy, float vz);
void  characterAddInaccuracy  (character *c, float inc);
void  characterDraw           (character *c);
void  characterAddCooldown    (character *c, int cooldown);
void  characterDrawAll        ();
uint32_t characterCollision   (character *c, float cx, float cy, float cz, float wd);
void  characterHitCheck       (character *c, int origin, float x, float y, float z, float yaw, float pitch, float roll, float pwr);
void  characterGotHitBroadcast(int c,float pwr);

void  characterMoveDelta      (character *c, packet *p);
void  characterDamagePacket   (character *c, packet *p);
void  characterSetPlayerPos   (const packet *p);
void  characterRemovePlayer   (int c, int len);

bool  itemPlaceBlock          (item *i, character *chr, int to);
