#pragma once
#include <stdbool.h>

extern int  optionWorldSeed;
extern int  optionPort;
extern bool optionSingleplayer;

void parseOptions(int argc,const char *argv[]);
void initOptions (int argc,const char *argv[]);
void sanityCheckOptions();
