static const int ITEMID=256;

#include "../../game/grenade.h"
#include "../../game/item.h"
#include "../../game/character.h"
#include "../../game/recipe.h"

void grenadeInit(){
	recipeAdd2I(ITEMID,1, 258,1, 3,1); // Pear(1) + Stone(1) -> Grenade(1)
}

bool grenadeActivateItem(item *cItem,character *cChar){
	if(itemDecStack(cItem,1)){
		//grenadeNew(cChar->ent,1);
		return true;
	}
	return false;
}
