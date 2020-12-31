#include "sdl.h"
#include "../main.h"
#include "../game/character.h"
#include "../gfx/gfx.h"
#include "../gfx/gl.h"
#include "../gui/gui.h"
#include "../gui/menu.h"
#include "../menu/inventory.h"
#include "../misc/options.h"
#include "../sdl/sfx.h"

#include "../sdl/input_mouse.h"
#include "../sdl/input_keyboard.h"
#include "../sdl/input_gamepad.h"
#include "../sdl/input_touch.h"

#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_hints.h>
#ifdef EMSCRIPTEN
	#include <SDL2/SDL_mixer.h>
#else
	#include <SDL_mixer.h>
#endif

bool textInputEvent(const SDL_Event *e);

SDL_Window* gWindow = NULL;
SDL_GLContext gContext;

uint  frameTimeLast       = 0;
uint  frameCountSinceLast = 0;
float curFPS              = 0;
uint  worstFrame          = 0;

void fpsTick() {
	uint curTicks = SDL_GetTicks();
	static uint lastFrame = 0;

	if(lastFrame == 0){lastFrame = curTicks;}
	worstFrame = MAX(worstFrame,curTicks - lastFrame);
	lastFrame = curTicks;

	frameCountSinceLast++;
	if(curTicks > frameTimeLast+1000){
		curFPS = ((float)curTicks - (float)frameTimeLast) / (float)frameCountSinceLast;
		curFPS = 1000.f / curFPS;
		frameTimeLast += (curTicks - frameTimeLast);
		frameCountSinceLast = 0;
	}
}

static void initSDLMixer(){
	if(optionMute){return;}
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
		fprintf(stderr,"Mix_OpenAudio: %s\n", Mix_GetError());
	}
	if(Mix_Init(MIX_INIT_OGG) == 0){
		fprintf(stderr,"Mix_Init Error\n");
	}
	Mix_AllocateChannels(128);
	sfxEnable = 1;
}

void initSDL(){
	SDL_DisplayMode dm;
	int desktopWidth  = optionWindowWidth;
	int desktopHeight = optionWindowHeight;
	char windowTitle[64];

	snprintf(windowTitle,sizeof(windowTitle)-1,"%s - WolkenWelten",playerName);

	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,"0");
	unsigned int initFlags = SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO;
	if(SDL_Init( initFlags) < 0){
		fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		exit(1);
	}

	#if defined __EMSCRIPTEN__
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
	#else
		#ifdef WOLKENWELTEN__GL_ES
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
		#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
		#endif
	#endif

	if( SDL_GetDesktopDisplayMode(0, &dm) == 0){
		desktopWidth  = screenWidth  = dm.w;
		desktopHeight = screenHeight = dm.h;
	}

	int cwflags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_RESIZABLE;
	#ifdef __EMSCRIPTEN__
		cwflags |= SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_INPUT_GRABBED;
	#else
		if(optionFullscreen){
			cwflags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}else{
			screenWidth = optionWindowWidth;
			screenHeight = optionWindowHeight;
		}
	#endif

	int windowx = SDL_WINDOWPOS_CENTERED;
	int windowy = SDL_WINDOWPOS_CENTERED;
	if((optionWindowOrientation & 0x0F) == 0x01){
		windowx = 0;
	}else if((optionWindowOrientation & 0x0F) == 0x02){
		windowx = desktopWidth - screenWidth;
	}
	if((optionWindowOrientation & 0xF0) == 0x10){
		windowy = 0;
	}else if((optionWindowOrientation & 0xF0) == 0x20){
		windowy = desktopHeight - screenHeight;
	}

	gWindow = SDL_CreateWindow( windowTitle, windowx, windowy, screenWidth, screenHeight, cwflags);
	if( gWindow == NULL ) {
		fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError() );
		exit(1);
	}

	gContext = SDL_GL_CreateContext( gWindow );
	if( gContext == NULL ){
		fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
		exit(1);
	}
	#ifndef __HAIKU__
	if( SDL_GL_SetSwapInterval(-1) != 0 ){ //Use AdaptiveVsync
		if( SDL_GL_SetSwapInterval(1) != 0 ){ //Use Vsync
			fprintf(stderr, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
		}
	}
	#endif

	mousex = screenWidth/2;
	mousey = screenHeight/2;
	SDL_ShowCursor(SDL_FALSE);
	SDL_DisableScreenSaver();

	initSDLMixer();

	initGL();
	gamepadInit();
}

void setFullscreen(bool fs){
	if(fs){
		SDL_SetWindowFullscreen(gWindow,SDL_WINDOW_FULLSCREEN_DESKTOP);
	}else{
		SDL_SetWindowFullscreen(gWindow,0);
		SDL_SetWindowSize(gWindow,800,600);
		SDL_SetWindowPosition(gWindow,SDL_WINDOWPOS_CENTERED ,SDL_WINDOWPOS_CENTERED);
	}
	optionFullscreen = fs;
}

void handleEvents(){
	SDL_Event e;

	while( SDL_PollEvent( &e ) != 0 ){
		switch(e.type){
		case SDL_QUIT:
			quit = true;
		break;

		case SDL_WINDOWEVENT:
			if((e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) || (e.window.event == SDL_WINDOWEVENT_RESIZED)){
				sdlResize(e.window.data1,e.window.data2);
			}
		break;
		}
		if(textInputEvent(&e)){continue;}
		mouseEventHandler(&e);
		keyboardEventHandler(&e);
		gamepadEventHandler(&e);
		touchEventHandler(&e);
	}
}

void sdlResize(int newW,int newH){
	if((newW == screenWidth) && (newH == screenHeight)){return;}
	screenWidth  = newW;
	screenHeight = newH;
	SDL_SetWindowSize(gWindow,screenWidth,screenHeight);
	initGL();
	resizeUI();
}

void closeSDL(){
	SDL_DestroyWindow( gWindow );
	closeGamepad();
	Mix_Quit();
	SDL_Quit();
}

void swapWindow(){
	SDL_GL_SwapWindow(gWindow);
}

void warpMouse(int mx,int my){
	SDL_WarpMouseInWindow(gWindow,mx,my);
}

void setRelativeMouseMode(bool ra){
	SDL_SetRelativeMouseMode(ra == true ? SDL_TRUE : SDL_FALSE);
}

unsigned int getMouseState(int *mx, int *my){
	return SDL_GetMouseState(mx,my);
}

unsigned int getTicks(){
	return SDL_GetTicks();
}

bool inputSneak(){
	return gamepadSneak()     || keyboardSneak()     || mouseSneak()     || touchSneak();
}
bool inputBoost(){
	return gamepadBoost()     || keyboardBoost()     || mouseBoost()     || touchBoost();
}
bool inputPrimary(){
	return gamepadPrimary()   || keyboardPrimary()   || mousePrimary()   || touchPrimary();
}
bool inputSecondary(){
	return gamepadSecondary() || keyboardSecondary() || mouseSecondary() || touchSecondary();
}
bool inputTertiary(){
	return gamepadTertiary()  || keyboardTertiary()  || mouseTertiary()  || touchTertiary();
}

bool gameControlsInactive(){
	if(widgetFocused == NULL)             {return true;}
	if(widgetFocused->type == wGameScreen){return false;}
	return true;
}
