#pragma once
#include "../common.h"

vec   beingGetPos    (being b);
void  beingSetPos    (being b, const vec pos);
void  beingAddPos    (being b, const vec pos);

vec   beingGetVel    (being b);
void  beingSetVel    (being b, const vec vel);
void  beingAddVel    (being b, const vec vel);

float beingGetWeight (being b);

being beingClosest   (const vec pos, float maxDistance);

void  beingListInit       (beingList *bl, beingList *parent);
void  beingListAdd        (beingList *bl, being entry);
void  beingListDel        (beingList *bl, being entry);
beingList *beingListGet   (u16 x, u16 y, u16 z);
beingList *beingListUpdate(beingList *bl, being entry);
being beingListGetClosest (const beingList *bl, const vec pos, uint type, float *d);

void            beingListEntryInit();
beingListEntry *beingListEntryNew ();
void            beingListEntryFree(beingListEntry *ble);
void            beingListPrint(beingList *bl);
