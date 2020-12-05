#pragma once

typedef enum lType {
	ltNoAlloc = 0, ltNil, ltBool, ltList, ltLambda, ltClosure, ltInt, ltString, ltCString, ltSymbol, ltNativeFunc, ltInf
} lType;

typedef struct lVal     lVal;
typedef struct lClosure lClosure;
typedef struct lString  lString;
typedef struct lCString lCString;

typedef struct {
	union {
		char c[8];
		unsigned long long int v;
	};
} lSymbol;

struct lVal {
	unsigned int flags;
	lType type;
	lVal *next;
	union {
		void         *vNil;
		int           vBool;
		lVal         *vList;
		lVal         *vLambda;
		int           vInt;
		unsigned int  vUint;
		unsigned int  vChar;
		lString      *vString;
		lCString     *vCString;
		lClosure     *vClosure;
		lSymbol       vSymbol;
		lVal       *(*vNativeFunc)(lClosure *, lVal *);
	};
};
#define lfMarked (1)

struct lClosure {
	lClosure *parent;
	lVal *data;
	unsigned int flags;
};

struct lString {
	char *buf,*data;
	union {
		char *bufEnd;
		lString *next;
	};
	int len;
};

struct lCString {
	const char *data;
	union {
		const char *bufEnd;
		lCString *next;
	};
};

void      lInit             ();
int       lMemUsage         ();
lClosure *lClosureNew       (lClosure *parent);
void      lClosureFree      (lClosure *c);
lVal     *lClosureAddNF     (lClosure *c, const char *sym, lVal *(*func)(lClosure *,lVal *));
void      lValFree          (lVal *v);
void      lClosureGC        (lClosure *c);
lVal     *lParseSExprCS     (const char *str);
void      lPrintVal         (lVal *v);
void      lPrintChain       (lVal *v);
char     *lSPrintVal        (lVal *v, char *buf, char *bufEnd);
char     *lSPrintChain      (lVal *v, char *buf, char *bufEnd);
lVal     *lResolveClosureSym(lClosure *c, const lSymbol s);
lVal     *lDefineClosureSym (lClosure *c, const lSymbol s);
lVal     *lResolveSym       (lClosure *c, const lSymbol s);
lVal     *lEval             (lClosure *c, lVal *v);

lVal     *lValNil       ();
lVal     *lValBool      (int v);
lVal     *lValInf       ();
lVal     *lValInt       (int v);
lVal     *lValSym       (const char *s);
