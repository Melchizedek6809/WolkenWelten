#define _GNU_SOURCE
#include "widgetDrawing.h"

#include "../game/character.h"
#include "../game/recipe.h"
#include "../gui/gui.h"
#include "../gui/textInput.h"
#include "../gui/widget.h"
#include "../gfx/textMesh.h"
#include "../sdl/sdl.h"
#include "../../../common/src/game/item.h"

#include <math.h>
#include <string.h>

static void widgetDrawButton(const widget *wid, textMesh *m, int x, int y, int w, int h){
	u32 color    = 0xFF555555;
	u32 tcolor   = 0xFF777777;
	u32 bcolor   = 0xFF333333;
	int textYOff = (h - (2*8))/2;
	int textXOff = (w-(strnlen(wid->label,w/16)*16))/2;

	if(wid == widgetFocused){
		 color = 0xFFAA6666;
		tcolor = 0xFFCC8888;
		bcolor = 0xFF884444;
	}
	if(wid->flags & WIDGET_CLICKED){
		if(wid != widgetFocused){
			color = 0xFF2A2A2A;
		}
		textXOff+=1;
		textYOff+=1;
		int tmp = tcolor;
		tcolor = bcolor;
		bcolor = tmp;
	}else if(wid->flags & WIDGET_HOVER){
		color = 0xFF444444;
	}

	textMeshVGradient(m,x+1,y+1,w-2,h-2, color,bcolor);
	textMeshSolidBox (m,x+1,y  ,w-2,  1,tcolor);
	textMeshSolidBox (m,x  ,y+1,  1,h-2,tcolor);
	textMeshSolidBox (m,x+1,y+h-1,w-2,  1,bcolor);
	textMeshSolidBox (m,x+w-1,y+1,  1,h-2,bcolor);

	textMeshAddLinePS(m,x+textXOff,y+textYOff,2,wid->label);
}

static void widgetDrawButtondel(const widget *wid, textMesh *m, int x, int y, int w, int h){
	u32 color    = 0xFF555555;
	u32 tcolor   = 0xFF777777;
	u32 bcolor   = 0xFF333333;

	u32 dcolor   = 0xFF555599;
	u32 dtcolor  = 0xFF7777AA;
	u32 dbcolor  = 0xFF333377;

	int textYOff = (h - (2*8))/2;
	int textXOff = (w-(strnlen(wid->label,w/16)*16))/2;

	if(wid == widgetFocused){
		 color = 0xFFAA6666;
		tcolor = 0xFFCC8888;
		bcolor = 0xFF884444;
	}else{
		if((int)mousex > (x+w-40)){
			if(wid->flags & WIDGET_CLICKED){
				dcolor  = 0xFF2A2A6A;
				int tmp = dtcolor;
				dtcolor = dbcolor;
				dbcolor = tmp;
			}else if(wid->flags & WIDGET_HOVER){
				dcolor  = 0xFF444488;
			}
		}else{
			if(wid->flags & WIDGET_CLICKED){
				color = 0xFF2A2A2A;
				textXOff+=1;
				textYOff+=1;
				int tmp = tcolor;
				tcolor = bcolor;
				bcolor = tmp;
			}else if(wid->flags & WIDGET_HOVER){
				color = 0xFF444444;
			}
		}
	}

	textMeshVGradient(m,x+1, y+1,w-41,h-1, color,bcolor);
	textMeshSolidBox (m,x+1, y  ,w-42,  1, tcolor);
	textMeshSolidBox (m,x  , y+1,   1,h-2, tcolor);
	textMeshSolidBox (m,x+1, y+h-1,w-42,  1, bcolor);

	textMeshVGradient(m,x+w-41, y+1,40,h-2, dcolor,dbcolor);
	textMeshSolidBox (m,x+w-1 , y+1  , 1,h-2, dbcolor);
	textMeshSolidBox (m,x+w-41, y+h-1,40,  1, dbcolor);
	textMeshSolidBox (m,x+w-41, y  ,40,  1, dtcolor);

	textMeshAddLinePS(m,x+textXOff,y+textYOff,2,wid->label);
	textMeshAddLinePS(m,x+w-24,y+textYOff,2,"X");
}

static void widgetDrawRadioButton(const widget *wid, textMesh *m, int x, int y, int w, int h){
	u32 color    = 0xFF555555;
	u32 tcolor   = 0xFF777777;
	u32 bcolor   = 0xFF333333;
	int textYOff = (h - (2*8))/2;
	int textXOff = (w-(strnlen(wid->label,w/16)*16))/2;

	if(wid == widgetFocused){
		 color = 0xFFAA6666;
		tcolor = 0xFFCC8888;
		bcolor = 0xFF884444;
	}
	if((wid->flags & WIDGET_CLICKED) || (wid->flags & WIDGET_ACTIVE)){
		if(wid != widgetFocused){
			color = 0xFF2A2A2A;
		}
		textXOff+=1;
		textYOff+=1;
		int tmp = tcolor;
		tcolor = bcolor;
		bcolor = tmp;
	}else if(wid->flags & WIDGET_HOVER){
		color = 0xFF444444;
	}

	textMeshVGradient(m,x+1,y+1,w-2,h-2, color,bcolor);
	textMeshSolidBox (m,x+1,y  ,w-2,  1,tcolor);
	textMeshSolidBox (m,x  ,y+1,  1,h-2,tcolor);
	textMeshSolidBox (m,x+1,y+h-1,w-2,  1,bcolor);
	textMeshSolidBox (m,x+w-1,y+1,  1,h-2,bcolor);

	textMeshAddLinePS(m,x+textXOff,y+textYOff,2,wid->label);
}


static void widgetDrawBackground(const widget *wid, textMesh *m, int x, int y, int w, int h){
	(void)wid;
	const uint i = getTicks() >> 12;
	int o = h/2 + sin(i)*((float)(h/3));
	textMeshVGradient(m,x,y    ,w,o,0xFFFFBF83, 0xFFFF6825);
	textMeshVGradient(m,x,y+o,w,h-o,0xFFFF6825, 0xFFE82410);
}

static void widgetDrawPanel(const widget *wid, textMesh *m, int x, int y, int w, int h){
	(void)wid;
	textMeshSolidBox(m,x,y,w,h,0xE0303030);
}

static void widgetDrawTextInput(const widget *wid, textMesh *m, int x, int y, int w, int h){
	u32 color    = 0xFF333333;
	u32 bcolor   = 0xFF555555;
	u32 tcolor   = 0xFF222222;
	int textYOff = (h - (2*8))/2;
	int textXOff = 8;
	int size     = 2;

	if((wid->flags & WIDGET_BIGGER) == WIDGET_BIGGER){
		size = 8;
	}else if(wid->flags & WIDGET_BIG){
		size = 4;
	}else if(wid->flags & WIDGET_SMALL){
		size = 1;
	}

	if(widgetFocused == wid){
		color = 0xFF292929;
	}

	textMeshSolidBox(m,x+1, y+1,w-1,h-2, color);
	textMeshSolidBox(m,x+1, y  ,w-2,  1,tcolor);
	textMeshSolidBox(m,x  , y+1,  1,h-2,tcolor);
	textMeshSolidBox(m,x+1, y+h-1,w-2,  1,bcolor);
	textMeshSolidBox(m,x+w-1, y+1,  1,h-2,bcolor);

	if(wid->vals == NULL){return;}
	if(wid->vals[0] == 0){
		textMeshAddLinePS(m,x+textXOff,y+textYOff,size,wid->label);
	}else{
		textMeshAddLinePS(m,x+textXOff,y+textYOff,size,wid->vals);
	}
	if((widgetFocused == wid) && (getTicks() & 512)){
		textMeshAddGlyph(m, x+textXOff+(textInputCursorPos*size*8), y+textYOff, size, 127,0xFFFFFFFF,0x00000000);
	}
}

static void widgetDrawLabel(const widget *wid, textMesh *m, int x, int y, int w, int h){
	(void)w;
	(void)h;
	int size = 2;

	if((wid->flags & WIDGET_BIGGER) == WIDGET_BIGGER){
		size = 8;
	}else if(wid->flags & WIDGET_BIG){
		size = 4;
	}else if(wid->flags & WIDGET_SMALL){
		size = 1;
	}

	m->sx = x;
	if(wid->label != NULL){
		textMeshAddLinePS(m,x,y,size,wid->label);
	}
	if(wid->vals != NULL){
		textMeshAddLinePS(m,m->sx,y,size,wid->vals);
	}
}

static void widgetDrawSlider(const widget *wid, textMesh *m, int x, int y, int w, int h){
	u32  bcolor = 0xFF555555;
	u32  tcolor = 0xFF222222;
	u32 abcolor = 0xFFC08840;
	u32 atcolor = 0xFFA04123;
	const int textYOff = (h - (2*8))/2;
	const int textXOff = (w-(strnlen(wid->label,w/16)*16))/2;
	const int size     = 2;
	const float v  = MAX(0,MIN(1,wid->vali / 4096.f));
	int o = v*(w-2);

	textMeshVGradient(m,x+1, y+1,o-3,h-2, abcolor,atcolor);
	textMeshSolidBox(m,x+o+3, y+1,w-o-3,h-2,tcolor);

	if(wid == widgetFocused){
		tcolor = abcolor;
		bcolor = atcolor;
	}
	textMeshSolidBox(m,x+1  , y    ,w-2,  1,tcolor);
	textMeshSolidBox(m,x    , y+1  ,  1,h-2,tcolor);
	textMeshSolidBox(m,x+1  , y+h-1,w-2,  1,bcolor);
	textMeshSolidBox(m,x+w-1, y+1  ,  1,h-2,bcolor);

	bcolor = 0xFF555555;
	tcolor = 0xFF222222;

	textMeshVGradient(m,x+o-2, y+1,4,h-2, bcolor,tcolor);
	textMeshSolidBox (m,x+o-3, y+1,1,h-2,tcolor);
	textMeshSolidBox (m,x+o+2, y+1,1,h-2,tcolor);
	textMeshSolidBox (m,x+o-2, y    ,4,1,tcolor);
	textMeshSolidBox (m,x+o-2, y+h-1,4,1,tcolor);

	m->sx = x;
	if(wid->label != NULL){
		textMeshAddLinePS(m,x+textXOff,y+textYOff,size,wid->label);
	}
}

static void widgetDrawHR(const widget *wid, textMesh *m, int x, int y, int w, int h){
	(void)wid;
	const u32 bcolor = 0xCC444444;
	const u32 tcolor = 0xCC111111;
	const uint o = MAX(0,(h/2)-2);
	textMeshVGradient(m,x,y+o,w,4,tcolor,bcolor);
}

static void widgetDrawItemSlot(const widget *wid, textMesh *m, int x, int y, int w, int h){
	(void)h;
	int style = 0;
	item *itm = wid->valItem;
	if((wid == widgetFocused) || (wid->flags & WIDGET_HOVER) || (itm == characterGetActiveItem(player))){
		style = 1;
	}
	textMeshItem(m, x, y, w, style, itm);
}

static void widgetDrawRecipeSlot(const widget *wid, textMesh *m, int x, int y, int w, int h){
	int style = 0;
	uint r    = wid->valu;
	item recipe = recipeGetResult(r);
	i16 a = recipeCanCraft(player,r);
	if(a == 0){
		style = 2;
	}
	if((wid == widgetFocused) || (wid->flags & WIDGET_HOVER)){
		style = 1;
	}
	textMeshItemSlot(m,x,y,MIN(w,h),style,recipe.ID,a);
}

static void widgetDrawRecipeInfo(const widget *wid, textMesh *m, int x, int y, int w, int h){
	const uint ticks = getTicks()>>4;
	const uint ts    = MIN(w,h);
	uint ii,xx,r = wid->vali;
	if(r >= recipeGetCount()){return;}
	const int animX = sin((float)ticks/24.f)*ts/8;
	const int animY = cos((float)ticks/24.f)*ts/8;


	for(ii=0;ii<4;ii++){
		xx = ii*2*ts + x;
		item ingred = recipeGetIngredient(r,ii);
		if(itemIsEmpty(&ingred)){break;}
		ingred.ID = ingredientSubstituteGetSub(ingred.ID,(ticks/96) % (ingredientSubstituteGetAmount(ingred.ID)+1));

		if(ii > 0){
			textMeshBox(m,xx-ts+ts/4+animX,y+ts/4+animY,ts/2,ts/2,24.f/32,31.f/32.f,1.f/32.f,1.f/32.f,~1);
		}
		textMeshItem(m,xx,y,ts,3,&ingred);
	}
	item result = recipeGetResult(r);

	xx = ii*2*ts + x;
	textMeshBox(m,xx-ts+ts/4+animX*2,y+ts/4,ts/2,ts/2,25.f/32.f,31.f/32.f,1.f/32.f,1.f/32.f,~1);
	textMeshItemSlot(m,xx,y,ts,3,result.ID,result.amount);

}

void widgetDrawSingle(const widget *wid, textMesh *m,int x, int y, int w, int h){
	if(wid == NULL){return;}

	switch(wid->type){
		case wNone:
		case wSpace:
			break;
		case wPanel:
			widgetDrawPanel(wid,m,x,y,w,h);
			break;
		case wBackground:
			widgetDrawBackground(wid,m,x,y,w,h);
			break;
		case wHR:
			widgetDrawHR(wid,m,x,y,w,h);
			break;
		case wLabel:
			widgetDrawLabel(wid,m,x,y,w,h);
			break;
		case wTextInput:
			widgetDrawTextInput(wid,m,x,y,w,h);
			break;
		case wButton:
			widgetDrawButton(wid,m,x,y,w,h);
			break;
		case wButtonDel:
			widgetDrawButtondel(wid,m,x,y,w,h);
			break;
		case wRadioButton:
			widgetDrawRadioButton(wid,m,x,y,w,h);
			break;
		case wSlider:
			widgetDrawSlider(wid,m,x,y,w,h);
			break;
		case wItemSlot:
			widgetDrawItemSlot(wid,m,x,y,w,h);
			break;
		case wRecipeSlot:
			widgetDrawRecipeSlot(wid,m,x,y,w,h);
			break;
		case wRecipeInfo:
			widgetDrawRecipeInfo(wid,m,x,y,w,h);
			break;
	}
}
