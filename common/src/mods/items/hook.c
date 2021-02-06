static const int ITEMID=275;

#include "../api_v1.h"

void hookInit(){
	recipeNew2(itemNew(ITEMID,1), itemNew(I_Iron_Bar,4), itemNew(I_Coal,4));
	lispDefineID("i-","hook",ITEMID);
}

char *hookGetItemName(const item *cItem){
	(void)cItem;
	return "Grappling Hook";
}

mesh *hookGetMesh(const item *cItem){
	(void)cItem;
	return meshBlaster;
}

int hookGetStackSize(const item *cItem){
	(void)cItem;
	return 1;
}
