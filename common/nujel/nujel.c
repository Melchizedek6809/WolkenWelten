#include "nujel.h"
#include "common.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lVal      lValBuf[1<<14];
uint      lValMax = 0;
lVal     *lValFFree = NULL;

lClosure  lClosureList[1<<8];
uint      lClosureMax   = 0;
lClosure *lClosureFFree = NULL;

lString   lStringList[1<<8];
lString  *lStringFFree = NULL;

lCString   lCStringList[1<<8];
lCString  *lCStringFFree = NULL;

lClosure *lClosureAlloc();
void      lClosureFree (lClosure *c);
lVal     *lValAlloc    ();
void      lValFree     (lVal *v);
lString  *lStringAlloc ();
void      lStringFree  (lString *s);
lCString *lCStringAlloc();
void      lCStringFree (lCString *s);

lSymbol symQuote;

void lInit(){
	strncpy(symQuote.c,"quote",7);
	symQuote.c[7] = 0;
	lValMax       = 0;
	lClosureMax   = 0;

	for(uint i=0;i<countof(lValBuf)-1;i++){
		lValBuf[i].type = ltNoAlloc;
		lValBuf[i].next = &lValBuf[i+1];
	}
	lValFFree = &lValBuf[0];

	for(uint i=0;i<countof(lClosureList)-1;i++){
		lClosureList[i].parent = &lClosureList[i+1];
	}
	lClosureFFree = &lClosureList[0];

	for(uint i=0;i<countof(lStringList)-1;i++){
		lStringList[i].next = &lStringList[i+1];
	}
	lStringFFree = &lStringList[0];

	for(uint i=0;i<countof(lCStringList)-1;i++){
		lCStringList[i].next = &lCStringList[i+1];
	}
	lCStringFFree = &lCStringList[0];
}

lClosure *lClosureAlloc(){
	if(lClosureFFree == NULL){
		fprintf(stderr,"lClosure OOM\n");
		return NULL;
	}
	lClosure *ret = lClosureFFree;
	lClosureFFree = ret->parent;
	lClosureMax   = MAX(lClosureMax,(uint)(ret-lClosureList) + 1);
	return ret;
}
void lClosureFree(lClosure *c){
	c->parent = lClosureFFree;
	c->data   = NULL;
	lClosureFFree = c;
	lValFree(c->data);
}

lString *lStringAlloc(){
	if(lStringFFree == NULL){
		fprintf(stderr,"lString OOM\n");
		return NULL;
	}
	lString *ret = lStringFFree;
	lStringFFree = ret->next;
	return ret;
}
void lStringFree(lString *s){
	if(s == NULL){return;}
	if(s->buf != NULL){free(s->buf);}
	s->buf = s->data = NULL;
	s->next = lStringFFree;
	lStringFFree = s;
}

lCString *lCStringAlloc(){
	if(lCStringFFree == NULL){
		fprintf(stderr,"lCString OOM\n");
		return NULL;
	}
	lCString *ret = lCStringFFree;
	lCStringFFree = ret->next;
	return ret;
}
void lCStringFree(lCString *s){
	if(s == NULL){return;}
	s->data = NULL;
	s->next = lCStringFFree;
	lCStringFFree = s;
}

lVal *lValAlloc(){
	if(lValFFree == NULL){
		fprintf(stderr,"lVal OOM\n");
		return NULL;
	}
	lVal *ret  = lValFFree;
	lValFFree  = ret->next;
	ret->next  = NULL;
	ret->flags = 0;
	lValMax    = MAX(lValMax,(uint)(ret-lValBuf) + 1);
	return ret;
}
void lValFree(lVal *v){
	if(v == NULL){return;}
	switch(v->type){
	case ltString:
		lStringFree(v->vString);
		break;
	default:
		break;
	}
	v->type   = ltNoAlloc;
	v->next   = lValFFree;
	v->flags  = 0;
	lValFFree = v;
}
lVal *lValCopy(const lVal *v){
	lVal *ret = lValAlloc();
	if(ret == NULL)       {return ret;}
	*ret = *v;
	ret->next = NULL;
	return ret;
}

lVal *lValNil(){
	lVal *ret = lValAlloc();
	if(ret == NULL)       {return ret;}
	ret->type = ltNil;
	return ret;
}
lVal *lValInf(){
	lVal *ret = lValAlloc();
	if(ret == NULL)       {return ret;}
	ret->type = ltInf;
	return ret;
}
lVal *lValInt(int v){
	lVal *ret = lValAlloc();
	if(ret == NULL)       {return ret;}
	ret->type = ltInt;
	ret->vInt = v;
	return ret;
}
lVal *lValBool(bool v){
	lVal *ret = lValAlloc();
	if(ret == NULL)       {return ret;}
	ret->type = ltBool;
	ret->vBool = v;
	return ret;
}

lVal *lValSym(const char *s){
	lVal *ret = lValAlloc();
	if(ret == NULL)       {return ret;}
	ret->type = ltSymbol;
	ret->vSymbol.v = 0;
	strncpy(ret->vSymbol.c,s,7);
	return ret;
}
lVal *lValList(lVal *v){
	lVal *ret  = lValAlloc();
	if(ret == NULL)       {return ret;}
	ret->type  = ltList;
	ret->vList = v;
	return ret;
}
lVal *lValListS(const char *s, lVal *v){
	lVal *ret = lValList(lValSym(s));
	if(ret == NULL)       {return ret;}
	if(ret->vList == NULL){return ret;}
	ret->vList->next = v;
	return ret;
}

lClosure *lClosureNew(lClosure *parent){
	lClosure *c = lClosureAlloc();
	if(c == NULL){
		return NULL;
	}
	c->parent = parent;
	c->data = lValAlloc();
	c->data->type = ltNil;
	return c;
}
/*
static lVal *lStringNew(const char *str){
	size_t len = strlen(str);
	lString *s = lStringAlloc();
	s->buf = s->data = malloc(len+1)
	memcpy(s->data,str,len)
	s->data[len] = 0;
	s->len       = len;
	s->bufEnd    = &s->buf[len+1];
	return s;
}
*/
static void lStringAdvanceToNextCharacter(lCString *s){
	for(;(*s->data != 0) && (isspace(*s->data));s->data++){}
}

static lVal *lParseString(lCString *s){
	static char buf[4096];
	char *b = buf;
	for(uint i=0;i<sizeof(buf);i++){
		if(*s->data == '\\'){
			s->data++;
			switch(*s->data){
			case '\\':
				*b++ = '\\';
				break;
			case '"':
				*b++ = '"';
				break;
			case 'n':
				*b++ = '\n';
				break;
			case 'r':
				*b++ = '\r';
				break;
			case 't':
				*b++ = '\t';
				break;
			case '0':
				*b++ = 0;
				break;
			}
			s->data++;
		}else if(*s->data == '"'){
			s->data++;
			lVal *v = lValAlloc();
			v->type = ltString;
			lString *vs = v->vString = lStringAlloc();
			vs->data = vs->buf = malloc((b-buf)+1);
			memcpy(vs->buf,buf,b-buf);
			vs->len = b-buf;
			vs->data[vs->len] = 0;
			vs->bufEnd = vs->data + vs->len;
			return v;
		}else{
			*b++ = *s->data++;
		}
	}
	lVal *v = lValAlloc();
	v->type = ltNil;
	return v;
}

static lVal *lParseNumber(lCString *s){
	lVal *v = lValInt(0);
	int c   = *s->data;
	int fc  = c;
	if(fc == '-'){c = *++s->data;}
	while(!isspace(c)){
		if(c == 0){break;}
		if(isdigit(c)){
			v->vInt *= 10;
			v->vInt += c - '0';
		}else if((c != ',') && (c != '.') && (c != '_')){
			break;
		}
		c = *++s->data;
	}
	if(fc == '-'){
		v->vInt = -v->vInt;
	}
	return v;
}

static lVal *lParseSymbol(lCString *s){
	lVal *v = lValAlloc();
	v->type = ltSymbol;
	v->vSymbol.v = 0;
	for(int i=0;i<8;i++){
		char c = *s->data++;
		if(isspace(c) || (c == ')') || (c ==0)){
			s->data--;
			break;
		}
		v->vSymbol.c[i] = c;
	}
	while(isspace(*s->data)){
		if(*s->data == 0){break;}
		s->data++;
	}
	return v;
}

static lVal *lParseSpecial(lCString *s){
	lVal *v = lValNil();
	if(*s->data++ != '#'){return v;}
	switch(*s->data++){
	default:
	case 'n': return v;
	case 't':
		v->type  = ltBool;
		v->vBool = true;
		return v;
	case 'f':
		v->type  = ltBool;
		v->vBool = false;
		return v;
	case 'i':
		v->type  = ltInf;
		return v;
	case 'z':
		v->type  = ltNoAlloc;
		return v;
	case 'c':
		v->type  = ltNativeFunc;
		return v;
	}

}

static lVal *lParse(lCString *s){
	lVal *v = NULL, *ret = NULL;
	while(1){
		lStringAdvanceToNextCharacter(s);
		char c = *s->data;
		lVal *t = NULL;
		if((c == 0) || (c == ')') || (s->data >= s->bufEnd)){
			if(v == NULL){
				v = lValAlloc();
				v->type = ltNil;
			}
			if(ret == NULL){ret = v;}
			s->data++;
			return ret;
		}else if(c == '('){
			s->data+=1;
			t = lValList(lParse(s));
		}else if((c == '\'') && (s->data[1] == '(')){
			s->data+=2;
			t = lValListS("quote",lValList(lParse(s)));
		}else if(c == '"'){
			s->data++;
			t = lParseString(s);
		}else if(isdigit(c)){
			t = lParseNumber(s);
		}else if(c == '#'){
			t = lParseSpecial(s);
		}else if((c == '-') && (isdigit(s->data[1]))){
			t = lParseNumber(s);
		}else{
			t = lParseSymbol(s);
		}
		if(v != NULL){v->next = t;}
		if(ret == NULL){ret = t;}
		v = t;
	}
}

lVal *lParseSExprCS(const char *str){
	lCString *s = lCStringAlloc();
	s->data     = str;
	int len     = strlen(str);
	s->bufEnd   = &str[len];
	lVal *ret   = lParse(s);
	lCStringFree(s);
	return ret;
}

void lPrintChain(lVal *v){
	static char buf[8192];
	char *end = lSPrintChain(v,buf,&buf[sizeof(buf) - 1]);
	*end = 0;
	printf("%s\n",buf);
}

void lPrintVal(lVal *v){
	static char buf[256];
	lSPrintVal(v,buf,&buf[sizeof(buf)]);
	printf("%s\n",buf);
}

char *lSPrintVal(lVal *v, char *buf, char *bufEnd){
	if(v == NULL){return buf;}
	char *cur = buf;
	int t = 0;
	int len = bufEnd-buf;
	if(v == NULL){
		t = snprintf(buf,len,"NULL");
	}else{
		switch(v->type){
		case ltNoAlloc:
			t = snprintf(buf,len,"#zzz");
			break;
		case ltNil:
			t = snprintf(buf,len,"#nil");
			break;
		case ltBool:
			if(v->vBool){
				t = snprintf(buf,len,"#t");
			}else{
				t = snprintf(buf,len,"#f");
			}
			break;
		case ltLambda: {
			t = snprintf(buf,bufEnd-cur,"λ(");
			if(t > 0){cur += t;}
			for(lVal *n = v->vList;n != NULL;n = n->next){
				cur = lSPrintVal(n,cur,bufEnd);
				if(n->next != NULL){*cur++ = ' ';}
			}
			t = snprintf(cur,bufEnd-cur,")");
			break; }
		case ltList: {
			t = snprintf(buf,bufEnd-cur,"(");
			if(t > 0){cur += t;}
			for(lVal *n = v->vList;n != NULL;n = n->next){
				cur = lSPrintVal(n,cur,bufEnd);
				if(n->next != NULL){*cur++ = ' ';}
			}
			t = snprintf(cur,bufEnd-cur,")");
			break; }
		case ltInt:
			t = snprintf(buf,len,"%i",v->vInt);
			break;
		case ltCString:
		case ltString:
			t = snprintf(buf,len,"\"%s\"",v->vString->data);
			break;
		case ltSymbol:
			t = snprintf(buf,len,"%.8s",v->vSymbol.c);
			break;
		case ltNativeFunc:
			t = snprintf(buf,len,"#cfn");
			break;
		case ltInf:
			t = snprintf(buf,len,"#inf");
			break;
		}
	}
	if(t > 0){cur += t;}
	return cur;
}

char *lSPrintChain(lVal *start, char *buf, char *bufEnd){
	if(start == NULL){return buf;}
	char *cur = buf;
	for(lVal *v = start;v!=NULL;v = v->next){
		cur = lSPrintVal(v,cur,bufEnd);
		if((bufEnd - cur) < 1){return cur;}
		*cur++ = ' ';
		if(v->type == ltNoAlloc){break;}
	}
	*bufEnd = 0;
	return cur;
}

lVal *lnfAdd(lClosure *c, lVal *v){
	lVal *t = lEval(c,v);
	if(t == NULL){return NULL;}
	t = lValCopy(t);
	for(lVal *r = v->next;r != NULL;r = r->next){
		lVal *rv = lEval(c,r);
		if(rv->type == ltInf){return rv;}
		if(rv->type != ltInt){continue;}
		t->vInt += rv->vInt;
	}
	return t;
}

lVal *lnfSub(lClosure *c, lVal *v){
	lVal *t = lEval(c,v);
	if(t == NULL){return NULL;}
	t = lValCopy(t);
	for(lVal *r = v->next;r != NULL;r = r->next){
		lVal *rv = lEval(c,r);
		if(rv->type == ltInf){return rv;}
		if(rv->type != ltInt){continue;}
		t->vInt -= rv->vInt;
	}
	if(v->next == NULL){
		t->vInt = -t->vInt;
	}
	return t;
}

lVal *lnfMul(lClosure *c, lVal *v){
	lVal *t = lEval(c,v);
	if(t == NULL){return NULL;}
	t = lValCopy(t);
	for(lVal *r = v->next;r != NULL;r = r->next){
		lVal *rv = lEval(c,r);
		if(rv->type == ltInf){return rv;}
		if(rv->type != ltInt){continue;}
		t->vInt *= rv->vInt;
	}
	return t;
}

lVal *lnfDiv(lClosure *c, lVal *v){
	lVal *t = lEval(c,v);
	if(t == NULL){return NULL;}
	t = lValCopy(t);
	for(lVal *r = v->next;r != NULL;r = r->next){
		lVal *rv = lEval(c,r);
		if(rv->type != ltInt){continue;}
		if(rv->vInt == 0){return lValInf();}
		t->vInt /= rv->vInt;
	}
	return t;
}

lVal *lnfMod(lClosure *c, lVal *v){
	lVal *t = lEval(c,v);
	if(t == NULL){return NULL;}
	t = lValCopy(t);
	for(lVal *r = v->next;r != NULL;r = r->next){
		lVal *rv = lEval(c,r);
		if(rv->type != ltInt){continue;}
		if(rv->vInt == 0){return lValInf();}
		t->vInt %= rv->vInt;
	}
	return t;
}

static lVal *lnfDef(lClosure *c, lVal *v){
	if(v == NULL)                {return lValNil();}
	lVal *sym = v;
	if(sym->type != ltSymbol)    {sym = lEval(c,sym);}
	if(sym->type != ltSymbol)    {return lValNil();}
	lVal *t = lDefineClosureSym(c,sym->vSymbol);
	if(t == NULL)                {return lValNil();}
	lVal *val = lEval(c,v->next);
	if(val != NULL){*t = *val;}
	return t;
}

static lVal *lnfSet(lClosure *c, lVal *v){
	if(v == NULL)                {return lValNil();}
	lVal *sym = v;
	if(sym->type != ltSymbol)    {sym = lEval(c,sym);}
	if(sym->type != ltSymbol)    {return lValNil();}
	lVal *t = lResolveClosureSym(c,sym->vSymbol);
	if(t == NULL)                {return lValNil();}
	lVal *val = lEval(c,v->next);
	if(val != NULL){*t = *val;}
	return t;
}

static lVal *lnfCl(lClosure *c, lVal *v){
	if(c == NULL)       {return lValNil();}
	if(v == NULL)       {return c->data;}
	if(v->next == NULL) {return c->data;}
	lVal *t = v->next;
	if(t->type == ltSymbol){t = lResolveSym(c,t->vSymbol);}
	if(t->type == ltInt){
		if(t->vInt > 0){
			t = lValCopy(t);
			t->vInt--;
			return lnfCl(c->parent,v);
		}
	}
	return c->data;
}

static lVal *lnfLambda(lClosure *c, lVal *v){
	(void)c;
	lVal *ret = lValList(v);
	if(ret == NULL){return NULL;}
	ret->type = ltLambda;
	return ret;
}

static lVal *lnfQuote(lClosure *c, lVal *v){
	(void)c;
	return v;
}

static lVal *lnfMem(lClosure *c, lVal *v){
	(void)c;
	(void)v;
	return lValInt(lMemUsage());
}

static lVal *lnfLet(lClosure *c, lVal *v){
	if(v == NULL)          {return lValNil();}
	if((v->type != ltList)){return lValNil();}
	lClosure *nc = lClosureNew(c);
	for(lVal *n = v->vList; n != NULL; n = n->next){
		if(n->type == ltSymbol){
			lDefineClosureSym(nc,n->vSymbol);
		}else if(n->type == ltList){
			if(n->vList == NULL)          {continue;}
			if(n->vList->type != ltSymbol){continue;}
			lVal *t = lDefineClosureSym(nc,n->vList->vSymbol);
			lVal *arg = lEval(c,n->vList->next);
			*t = *arg;
		}
	}
	lVal *ret = NULL;
	for(lVal *n = v->next;n != NULL;n = n->next){
		ret = lEval(nc,n);
	}
	lClosureFree(nc);
	if(ret == NULL){ret = lValNil();}
	return ret;
}

static lVal *lLambda(lClosure *c,lVal *v, lVal *lambda){
	if(lambda == NULL)          {return lValNil();}
	if((lambda->type != ltList)){return lValNil();}
	lClosure *nc = lClosureNew(c);
	lVal *vn = v;
	for(lVal *n = lambda->vList; n != NULL; n = n->next){
		if(n->type != ltSymbol){continue;}
		lVal *lv = lDefineClosureSym(nc,n->vSymbol);
		lVal *t = lEval(c,vn);
		if(t != NULL){*lv = *t;}
		if(vn != NULL){vn = vn->next;}
	}
	lVal *ret = NULL;
	for(lVal *n = lambda->next;n != NULL;n = n->next){
		ret = lEval(nc,n);
	}
	lClosureFree(nc);
	if(ret == NULL){ret = lValNil();}
	return ret;
}

static lVal *lValNativeFunc(lVal *(*func)(lClosure *,lVal *)){
	lVal *v = lValAlloc();
	v->type = ltNativeFunc;
	v->vNativeFunc = func;
	return v;
}

lVal *lClosureAddNF(lClosure *c, const char *sym, lVal *(*func)(lClosure *,lVal *)){
	lVal *nf = lValNativeFunc(func);
	lVal *vsym = lValSym(sym);
	if((vsym == NULL) || (vsym->type != ltSymbol)){return lValNil();}
	lVal *t = lDefineClosureSym(c,vsym->vSymbol);
	*t = *nf;
	return t;
}

static lVal *lnfLess(lClosure *c, lVal *v){
	lVal *a = lEval(c,v);
	if((a == NULL) || (a->type != ltInt)){return lValBool(false);}
	lVal *b = lEval(c,v->next);
	if((b == NULL) || (b->type != ltInt)){return lValBool(false);}
	return lValBool(a->vInt < b->vInt);
}

static lVal *lnfLessEqual(lClosure *c, lVal *v){
	lVal *a = lEval(c,v);
	if((a == NULL) || (a->type != ltInt)){return lValBool(false);}
	lVal *b = lEval(c,v->next);
	if((b == NULL) || (b->type != ltInt)){return lValBool(false);}
	return lValBool(a->vInt <= b->vInt);
}

static lVal *lnfEqual(lClosure *c, lVal *v){
	lVal *a = lEval(c,v);
	if((a == NULL) || (a->type != ltInt)){return lValBool(false);}
	lVal *b = lEval(c,v->next);
	if((b == NULL) || (b->type != ltInt)){return lValBool(false);}
	return lValBool(a->vInt == b->vInt);
}

static lVal *lnfGreater(lClosure *c, lVal *v){
	lVal *a = lEval(c,v);
	if((a == NULL) || (a->type != ltInt)){return lValBool(false);}
	lVal *b = lEval(c,v->next);
	if((b == NULL) || (b->type != ltInt)){return lValBool(false);}
	return lValBool(a->vInt > b->vInt);
}

static lVal *lnfGreaterEqual(lClosure *c, lVal *v){
	lVal *a = lEval(c,v);
	if((a == NULL) || (a->type != ltInt)){return lValBool(false);}
	lVal *b = lEval(c,v->next);
	if((b == NULL) || (b->type != ltInt)){return lValBool(false);}
	return lValBool(a->vInt >= b->vInt);
}

static lVal *lnfZero(lClosure *c, lVal *v){
	lVal *a = lEval(c,v);
	if((a == NULL) || (a->type != ltInt)){return lValBool(false);}
	return lValBool(a->vInt == 0);
}

static lVal *lnfNot(lClosure *c, lVal *v){
	lVal *a = lEval(c,v);
	if(a == NULL)        {return lValBool(true);}
	if(a->type == ltNil) {return lValBool(true);}
	if(a->type != ltBool){return lValBool(true);}
	if(!a->vBool)        {return lValBool(true);}
	return lValBool(false);
}

static lVal *lnfAnd(lClosure *c, lVal *v){
	for(lVal *n = v;n != NULL;n = n->next){
		lVal *a = lEval(c,n);
		if(a == NULL)        {return lValBool(false);}
		if(a->type == ltNil) {return a;}
		if(a->type != ltBool){continue;}
		if(a->vBool == false){return a;}
	}
	return lValBool(true);
}

static lVal *lnfOr(lClosure *c, lVal *v){
	for(lVal *n = v;n != NULL;n = n->next){
		lVal *a = lEval(c,n);
		if(a == NULL)        {continue;}
		if(a->type == ltNil) {continue;}
		if(a->type != ltBool){return a;}
		if(a->vBool == true) {return a;}
	}
	return lValBool(false);
}

static lVal *lnfCond(lClosure *c, lVal *v){
	if(v == NULL){return lValNil();}
	for(lVal *n = v;n != NULL;n = n->next){
		if(n->type != ltList){continue;}
		lVal *t = lEval(c,n->vList);
		if((t == NULL) || (n == NULL))        {continue;}
		if( t->type == ltNil)                 {continue;}
		if((t->type == ltBool) && (!t->vBool)){continue;}
		for(lVal *block = n->vList;block != NULL;block = block->next){
			t = lEval(c,block);
			if(block->next == NULL){return t;}
		}
	}
	return lValNil();
}

static lVal *lResolveNativeSym(const lSymbol s){
	if(s.c[1] == 0){
		switch(s.c[0]){
		case '+': return lValNativeFunc(lnfAdd);
		case '-': return lValNativeFunc(lnfSub);
		case '*': return lValNativeFunc(lnfMul);
		case '/': return lValNativeFunc(lnfDiv);
		case '%': return lValNativeFunc(lnfMod);
		case '<': return lValNativeFunc(lnfLess);
		case '=': return lValNativeFunc(lnfEqual);
		case '>': return lValNativeFunc(lnfGreater);
		}
	}
	if((s.c[2] == 0) && (s.c[1] == '=')){
		switch(s.c[0]){
		case '<': return lValNativeFunc(lnfLessEqual);
		case '>': return lValNativeFunc(lnfGreaterEqual);
		}
	}
	if(strcmp(s.c,"and") == 0)   {return lValNativeFunc(lnfAnd);}
	if(strcmp(s.c,"cl") == 0)    {return lValNativeFunc(lnfCl);}
	if(strcmp(s.c,"cond") == 0)  {return lValNativeFunc(lnfCond);}
	if(strcmp(s.c,"define") == 0){return lValNativeFunc(lnfDef);}
	if(strcmp(s.c,"lambda") == 0){return lValNativeFunc(lnfLambda);}
	if(strcmp(s.c,"let") == 0)   {return lValNativeFunc(lnfLet);}
	if(strcmp(s.c,"mem") == 0)   {return lValNativeFunc(lnfMem);}
	if(strcmp(s.c,"mod") == 0)   {return lValNativeFunc(lnfMod);}
	if(strcmp(s.c,"modulo") == 0){return lValNativeFunc(lnfMod);}
	if(strcmp(s.c,"not") == 0)   {return lValNativeFunc(lnfNot);}
	if(strcmp(s.c,"or") == 0)    {return lValNativeFunc(lnfOr);}
	if(strcmp(s.c,"quote") == 0) {return lValNativeFunc(lnfQuote);}
	if(strcmp(s.c,"set!") == 0)  {return lValNativeFunc(lnfSet);}
	if(strcmp(s.c,"zero?") == 0) {return lValNativeFunc(lnfZero);}
	return lValNil();
}

lVal *lDefineClosureSym(lClosure *c,const lSymbol s){
	if(c == NULL){return NULL;}
	lVal *v;
	for(v = c->data;v->next != NULL;v = v->next){
		if(v->type != ltList)         {continue;}
		if(v->vList == NULL)          {continue;}
		if(v->vList->type != ltSymbol){continue;}
		if(v->vList->vSymbol.v != s.v){continue;}
		return NULL;
	}
	if(v == NULL){return NULL;}
	v->next = lValNil();
	v->type = ltList;
	v->vList = lValAlloc();
	if(v->vList == NULL){
		v->vList = NULL;
		return NULL;
	}
	v->vList->type    = ltSymbol;
	v->vList->vSymbol = s;
	v->vList->next    = lValNil();
	if(v->vList->next == NULL){
		v->vList = NULL;
		return NULL;
	}
	return v->vList->next;
}

lVal *lResolveClosureSym(lClosure *c, const lSymbol s){
	if(c == NULL){return NULL;}
	for(lVal *v = c->data;v != NULL;v = v->next){
		if(v->type != ltList)         {continue;}
		if(v->vList == NULL)          {continue;}
		if(v->vList->type != ltSymbol){continue;}
		if(v->vList->vSymbol.v != s.v){continue;}
		return v->vList->next;
	}
	return lResolveClosureSym(c->parent,s);
}

lVal *lResolveSym(lClosure *c, const lSymbol s){
	lVal *v = lResolveClosureSym(c,s);
	if(v != NULL){
		return v;
	}else{
		return lResolveNativeSym(s);
	}
}

lVal *lEval(lClosure *c, lVal *v){
	lVal *ret = v;
	if(v == NULL){return NULL;}
	if(v->type == ltSymbol){
		ret = lResolveSym(c,v->vSymbol);
	} else if(v->type == ltList){
		ret = lEval(c,v->vList);
		if(ret == NULL){return NULL;}
		if(ret->type == ltNativeFunc){
			ret = ret->vNativeFunc(c,v->vList->next);
		}else if(ret->type == ltLambda){
			ret = lLambda(c,v->vList->next,ret->vList);
		}
	}
	return ret;
}

int lMemUsage(){
	int used=0;
	for(uint i=0;i<lValMax;i++){
		if(lValBuf[i].type == ltNoAlloc){continue;}
		used++;
	}
	return used;
}

static inline void lValGCMark(lVal *sv){
	for(lVal *v = sv;v != NULL;v = v->next){
		v->flags |= lfMarked;
		if(v->type == ltList)  {lValGCMark(v->vList);}
		if(v->type == ltLambda){lValGCMark(v->vList);}
	}
}
void lClosureGC(){
	for(uint i=0;i<lValMax;i++){lValBuf[i].flags &= ~lfMarked;}
	//for(;c != NULL;c = c->parent){lValGCMark(c->data);}
	for(uint i=0;i<lClosureMax;i++){lValGCMark(lClosureList[i].data);}
	for(uint i=0;i<lValMax;i++){
		if(lValBuf[i].type == ltNoAlloc){continue;}
		if(lValBuf[i].flags & lfMarked) {continue;}
		lValFree(&lValBuf[i]);
	}
}