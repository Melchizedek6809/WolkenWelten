#pragma once
#include "../../../common/src/common.h"

extern float sunAngle;
extern float cloudOffset;

void initSky();
void renderSky(const character *cam);
