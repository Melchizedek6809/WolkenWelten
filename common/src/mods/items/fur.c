static const int ITEMID=280;

#include "../api_v1.h"

mesh *furGetMesh(const item *cItem){
	(void)cItem;

	return meshFur;
}

int furGetAmmunition(const item *cItem){
	(void)cItem;

	return ITEMID;
}

int furItemDropCallback(const item *cItem, float x, float y, float z){
	(void)cItem;
	(void)x;
	(void)y;
	(void)z;
	if(rngValM(1<<16) == 0){ return -1;}
	return 0;
}