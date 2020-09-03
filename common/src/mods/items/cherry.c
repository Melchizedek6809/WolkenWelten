static const int ITEMID=272;

#include "../api_v1.h"

int cherryDamage(item *cItem){
	(void)cItem;
	
	return 12;
}

bool cherrySecondaryAction(item *cItem,character *cChar, int to){
	(void)cItem;

	if(to < 0){return false;}
	if(characterGetHP(cChar) >= characterGetMaxHP(cChar)){return false;}
	if(itemDecStack(cItem,1)){
		characterHP(cChar,8);
		characterAddCooldown(cChar,200);
		characterStartAnimation(cChar,4,600);
		sfxPlay(sfxImpact,1.f);
		return true;
	}
	return false;
}

mesh *cherryGetMesh(item *cItem){
	(void)cItem;

	return meshCherry;
}

int cherryGetAmmunition(item *cItem){
	(void)cItem;
	
	return ITEMID;
}