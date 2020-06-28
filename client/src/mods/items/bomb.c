static const int ITEMID=257;

#include "../../gfx/mesh.h"
#include "../../gfx/objs.h"
#include "../../game/grenade.h"
#include "../../game/item.h"
#include "../../game/character.h"
#include "../../game/recipe.h"

void bombInit(){
	recipeAdd1I(ITEMID,1, 256,3);
}

bool bombActivateItem(item *cItem,character *cChar){
	if(itemDecStack(cItem,1)){
		grenadeNew(cChar,3);
		return true;
	}
	return false;
}

mesh *bombGetMesh(item *cItem){
	return meshBomb;
}
