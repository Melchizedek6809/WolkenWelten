#pragma once
#include <stdbool.h>

extern int  optionWorldSeed;
extern int  optionPort;
extern bool optionSingleplayer;
extern bool optionProfileWG;
extern char optionSavegame[32];
extern bool verbose;

void parseOptions(int argc,const char *argv[]);
void initOptions (int argc,const char *argv[]);
void sanityCheckOptions();
