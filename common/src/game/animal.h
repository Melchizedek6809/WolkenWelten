#pragma once
#include "../../../common/src/common.h"

void     animalReset          (animal *e);
int      animalUpdate         (animal *e);
float    animalDistance       (animal *e, character *c);
uint32_t animalCollision      (float cx, float cy, float cz);
