static const int ITEMID=272;

#include "../api_v1.h"

void cherryInit(){
	lispDefineID("i-","cherry",ITEMID);
}

bool cherrySecondaryAction(item *cItem,character *cChar){
	if(characterGetHP(cChar) >= characterGetMaxHP(cChar)){return false;}
	if(characterTryToUse(cChar,cItem,200,1)){
		characterHP(cChar,8);
		characterAddCooldown(cChar,200);
		characterStartAnimation(cChar,4,600);
		sfxPlay(sfxNibble,1.f);
		return true;
	}
	return false;
}

int cherryGetFireDmg(const itemDrop *id){
	(void)id;
	return 8;
}

int cherryGetFireHealth(const itemDrop *id){
	(void)id;
	return 100;
}
