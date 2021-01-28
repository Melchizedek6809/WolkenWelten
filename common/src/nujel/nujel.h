#pragma once
#include "../common.h"

typedef enum lType {
	ltNoAlloc = 0, ltBool, ltPair, ltLambda, ltInt, ltFloat, ltVec, ltString, ltSymbol, ltNativeFunc, ltInf, ltArray
} lType;

typedef struct lVal     lVal;
typedef struct lClosure lClosure;
typedef struct lString  lString;
typedef struct lArray   lArray;
typedef struct lNFunc   lNFunc;

typedef struct {
	union {
		char c[16];
		u64 v[2];
	};
} lSymbol;

typedef struct {
	lVal *car,*cdr;
} lPair;

struct lArray {
	lVal **data;
	int length;
	uint flags;
	lArray *nextFree;
};

struct lNFunc {
	lVal *(*fp)(lClosure *, lVal *);
	lVal *docString;
};

struct lVal {
	u8 flags;
	lType type;
	union {
		lVal         *vNA;
		bool          vBool;
		lPair         vList;
		int           vInt;
		vec           vVec;
		float         vFloat;
		lString      *vString;
		lSymbol       vSymbol;
		lClosure     *vLambda;
		lArray       *vArr;
		lNFunc        vFunc;
	};
};

struct lClosure {
	lClosure *parent;
	lVal *data;
	lVal *text;
	uint flags;
};
#define lfMarked    ( 1)
#define lfDynamic   ( 2)
#define lfNoGC      ( 4)
#define lfConst     ( 8)
#define lfHeapAlloc (16)

struct lString {
	const char *buf,*data;
	union {
		const char *bufEnd;
		lString *next;
	};
	uint flags;
};

extern lSymbol symQuote,symArr;

void      lInit             ();
int       lMemUsage         ();
void      lPrintError       (const char *format, ...);

lClosure *lClosureAlloc     ();
lClosure *lClosureNewRoot   ();
lClosure *lClosureNew       (lClosure *parent);
void      lClosureFree      (lClosure *c);
lVal     *lClosureAddNF     (lClosure *c, const char *sym, lVal *(*func)(lClosure *,lVal *));

lVal     *lValAlloc         ();
void      lValFree          (lVal *v);

lArray   *lArrayAlloc       ();
void      lArrayFree        (lArray *v);

lString  *lStringAlloc      ();
void      lStringFree       (lString *s);
lString  *lStringNew        (const char *str, uint len);

void      lClosureGC        ();
void      lDisplayVal       (lVal *v);
void      lWriteVal         (lVal *v);

void      lAddNativeFunc    (lClosure *c, const char *sym, const char *doc, lVal *(*func)(lClosure *,lVal *));
lVal     *lValNativeFunc    (lVal *(*func)(lClosure *,lVal *), lVal *docString);
lVal     *lGetClosureSym    (lClosure *c, const lSymbol s);
lVal     *lResolveClosureSym(lClosure *c, const lSymbol s);
lVal     *lDefineClosureSym (lClosure *c, const lSymbol s);
lVal     *lResolveSym       (lClosure *c, const lSymbol s);
lVal     *lApply            (lClosure *c, lVal *v, lVal *(*func)(lClosure *,lVal *));
lVal     *lCast             (lClosure *c, lVal *v, lType t);
lVal     *lEval             (lClosure *c, lVal *v);
lType     lTypecast         (const lType a,const lType b);
lType     lTypecastList     (lVal *a);

lVal     *lCons         (lVal *car,lVal *cdr);
lVal     *lValBool      (bool v);
lVal     *lValInf       ();
lVal     *lValInt       (int v);
lVal     *lValFloat     (float v);
lVal     *lValVec       (const vec v);
lVal     *lValSymS      (const lSymbol s);
lVal     *lValSym       (const char *s);
lVal     *lValString    (const char *s);
lVal     *lnfCat        (lClosure *c, lVal *v);
lVal     *lValCopy      (lVal *dst, const lVal *src);

#define forEach(n,v) for(lVal *n = v;(n != NULL) && (n->type == ltPair) && (n->vList.car != NULL); n = n->vList.cdr)

static inline lVal *lValDup(const lVal *v){
	return v == NULL ? NULL : lValCopy(lValAlloc(),v);
}
static inline lVal *lWrap(lVal *v){
	return lCons(lValSym("begin"),lCons(lCons(NULL,NULL),v));
}
static inline lVal *lEvalCast(lClosure *c, lVal *v){
	lVal *t = lApply(c,v,lEval);
	return lCast(c,t,lTypecastList(t));
}
static inline lVal *lEvalCastSpecific(lClosure *c, lVal *v, const lType type){
	lVal *t = lApply(c,v,lEval);
	return lCast(c,t,type);
}
static inline lVal *lEvalCastNumeric(lClosure *c, lVal *v){
	lVal *t = lApply(c,v,lEval);
	lType type = lTypecastList(t);
	if(type == ltString){type = ltFloat;}
	return lCast(c,t,type);
}
static inline lVal *lLastCar(lVal *v){
	forEach(a,v){if(a->vList.cdr == NULL){return a->vList.car;}}
	return NULL;
}
static inline lVal *lCarOrV(lVal *v){
	return (v != NULL) && (v->type == ltPair) ? v->vList.car : v;
}
static inline lVal *lCarOrN(lVal *v){
	return (v != NULL) && (v->type == ltPair) ? v->vList.car : NULL;
}
static inline lVal *lCadrOrN(lVal *v){
	return (v != NULL) && (v->type == ltPair) ? lCarOrN(v->vList.cdr) : NULL;
}
static inline int lListLength(lVal *v){
	int i = 0;
	for(lVal *n = v;(n != NULL) && (n->type == ltPair) && (n->vList.car != NULL); n = n->vList.cdr){i++;}
	return i;
}
static inline uint lStringLength(lString *s){
	return s->bufEnd - s->buf;
}

#define lEvalCastApply(FUNC, c , v) do { \
	lVal *t = lEvalCast(c,v); \
	if((t == NULL) || (t->type != ltPair)){return lValInt(0);} \
	lVal *d = lValDup(t->vList.car); \
	if(d == NULL){return lValInt(0);} \
	switch(d->type){ \
	default:      return lValInt(0); \
	case ltInf:   return lValInf(); \
	case ltInt:   return FUNC##I(d,t); \
	case ltFloat: return FUNC##F(d,t); \
	case ltVec:   return FUNC##V(d,t); \
	}} while (0)
