#pragma once
#include "nujel.h"

lVal *lnfAdd(lClosure *c, lVal *v);
lVal *lnfSub(lClosure *c, lVal *v);
lVal *lnfMul(lClosure *c, lVal *v);
lVal *lnfDiv(lClosure *c, lVal *v);
lVal *lnfMod(lClosure *c, lVal *v);
lVal *lnfAbs(lClosure *c, lVal *v);

lVal *lnfInt(lClosure *c, lVal *v);
lVal *lnfFloat(lClosure *c, lVal *v);
lVal *lnfVec(lClosure *c, lVal *v);

lVal *lnfVX(lClosure *c, lVal *v);
lVal *lnfVY(lClosure *c, lVal *v);
lVal *lnfVZ(lClosure *c, lVal *v);
