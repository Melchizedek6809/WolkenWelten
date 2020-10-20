#include "menu.h"

#include "../main.h"
#include "../gui/gui.h"
#include "../menu/mainmenu.h"
#include "../menu/singleplayer.h"
#include "../menu/multiplayer.h"
#include "../menu/inventory.h"
#include "../menu/options.h"
#include "../gui/textInput.h"
#include "../gui/widget.h"
#include "../gfx/gl.h"
#include "../gfx/gfx.h"
#include "../gfx/shader.h"
#include "../gfx/texture.h"
#include "../gfx/textMesh.h"
#include "../network/client.h"
#include "../misc/options.h"
#include "../tmp/assets.h"
#include "../voxel/bigchungus.h"
#include "../../../common/src/tmp/cto.h"
#include "../../../common/src/misc/misc.h"

#include <stdio.h>
#include <string.h>

bool showAttribution  = false;
int  attributionLines = 0;

char menuTextInputLabel[32];

int  serverlistCount = 0;
char serverlistName[16][32];
char serverlistIP[16][64];

widget *rootMenu;

widget *menuBackground;
widget *menuText;
widget *menuErrorLabel;
widget *menuAttribution;

void startMultiplayer(){
	gameRunning     = true;
	connectionTries = 0;

	closeAllMenus();
	hideMouseCursor();
	openInventoryPanel();
	widgetFocus(widgetGameScreen);
}

void startSingleplayer(){
	singleplayer    = true;
	startMultiplayer();
}

void closeAllMenus(){
	if(gameRunning){
		menuBackground->flags |=  WIDGET_HIDDEN;
	}else{
		menuBackground->flags &= ~WIDGET_HIDDEN;
	}
	menuText->flags &= ~WIDGET_HIDDEN;
	menuAttribution->flags |= WIDGET_HIDDEN;
	closeMainMenu();
	closeSingleplayerMenu();
	closeMultiplayerMenu();
	closeOptionsMenu();
	closeInventoryPanel();
	widgetFocus(NULL);
}

static void handlerRoot(widget *wid){
	(void)wid;
	if(showAttribution){openMainMenu();}
}

void initMenu(){
	widget *wid;

	rootMenu = widgetNewCP(wSpace,NULL,0,0,-1,-1);
	widgetBind(rootMenu,"click",handlerRoot);

	menuBackground = widgetNewCP(wBackground,rootMenu,0,0,-1,-1);
	menuText = widgetNewCP(wSpace,menuBackground,32,32,256,-65);
	wid = widgetNewCPL(wLabel,menuText,0,0,256,32,"Wolkenwelten");
	wid->flags |= WIDGET_BIG;
	widgetNewCPL(wLabel,menuText,0,32,256,32,(const char *)VERSION);
	menuErrorLabel = widgetNewCPL(wLabel,menuText,1,-97,256,16,"");
	widgetNewCPL(wLabel,menuText,1,-33,256,16,menuTextInputLabel);
	menuAttribution = widgetNewCPL(wTextScroller,rootMenu,0,0,-1,-1,(const char *)txt_attribution_txt_data);
	menuAttribution->flags |= WIDGET_HIDDEN;

	initMainMenu();
	initSingleplayerMenu();
	initMultiplayerMenu();
	initOptionsMenu();
}

void openAttributions(){
	menuText->flags |=  WIDGET_HIDDEN;
	menuAttribution->flags &= ~WIDGET_HIDDEN;
}

void menuChangeFocus(int xoff,int yoff,bool ignoreOnTextInput){
	if(widgetFocused != NULL){
		if(widgetFocused->type == wGameScreen){return;}
		if(ignoreOnTextInput && widgetFocused->type == wTextInput){return;}
		if((widgetFocused->type == wSlider) && (xoff != 0)){
			widgetFocused->vali = MAX(0,MIN(4096,(widgetFocused->vali + xoff*128)));
		}
		if(yoff < 0){
			if(!widgetEmit(widgetFocused,"selectNext")){
				widgetFocus(widgetNextSel(widgetFocused));
			}
		}else if(yoff > 0){
			if(!widgetEmit(widgetFocused,"selectPrev")){
				widgetFocus(widgetPrevSel(widgetFocused));
			}
		}
	} // No else if because then we immediatly focus on the widget on the other side
	if(widgetFocused == NULL){
		if(yoff < 0){
			widgetFocus(widgetNextSel(rootMenu));
		}else if(yoff > 0){
			widgetFocus(widgetPrevSel(rootMenu));
		}
	}
}

void menuKeyClick(int btn){
	(void)btn;
	if(widgetFocused == NULL){return;}
	if(widgetFocused->type == wGameScreen){return;}
	if(btn == 1){
		widgetEmit(widgetFocused,"altclick");
	}else{
		widgetEmit(widgetFocused,"click");
	}
}

void menuSetError(char *error){
	menuErrorLabel->vals = error;
}

void menuCancel(){
	if((widgetFocused != NULL) && (widgetFocused->type == wGameScreen)){return;}
	if(gameRunning){return;}
	openMainMenu();
}

void menuCloseGame(){
	gameRunning=false;
	clientGoodbye();
	clientFree();
	bigchungusFree(&world);
	openMainMenu();
}
