#include "being.h"

#include "../game/animal.h"
#include "../game/character.h"
#include "../game/entity.h"
#include "../game/hook.h"

vec beingGetPos(being b){
	switch(beingType(b)){
		case BEING_CHARACTER: {
			character *c = characterGetByBeing(b);
			if(c == NULL){return vecNOne();}
			return vecAdd(c->pos,vecNew(0,c->yoff + 0.2f,0)); }
		case BEING_ANIMAL: {
			animal *c = animalGetByBeing(b);
			if(c == NULL){return vecNOne();}
			return c->pos; }
		case BEING_HOOK: {
			hook *c = hookGetByBeing(b);
			if(c == NULL)     {return vecNOne();}
			if(c->ent == NULL){return vecNOne();}
			return c->ent->pos; }
		case BEING_ENTITY: {
			entity *c = entityGetByBeing(b);
			if(c == NULL){return vecNOne();}
			return c->pos; }
		default:
			return vecNOne();
	}
}

void beingSetPos(being b, const vec pos){
	switch(beingType(b)){
		case BEING_CHARACTER: {
			character *c = characterGetByBeing(b);
			if(c == NULL){return;}
			c->pos = pos;
			return; }
		case BEING_ANIMAL: {
			animal *c = animalGetByBeing(b);
			if(c == NULL){return;}
			c->pos = pos;
			return; }
		case BEING_HOOK: {
			hook *c = hookGetByBeing(b);
			if(c == NULL)     {return;}
			if(c->ent == NULL){return;}
			c->ent->pos = pos;
			return; }
		case BEING_ENTITY: {
			entity *c = entityGetByBeing(b);
			if(c == NULL){return;}
			c->pos = pos;
			return; }
		default:
			return;
	}
}

void beingAddPos(being b, const vec pos){
	switch(beingType(b)){
		case BEING_CHARACTER: {
			character *c = characterGetByBeing(b);
			if(c == NULL){return;}
			c->pos = vecAdd(c->pos, pos);
			return; }
		case BEING_ANIMAL: {
			animal *c = animalGetByBeing(b);
			if(c == NULL){return;}
			c->pos = vecAdd(c->pos,pos);
			return; }
		case BEING_HOOK: {
			hook *c = hookGetByBeing(b);
			if(c == NULL){return;}
			c->ent->pos = vecAdd(c->ent->pos, pos);
			return; }
		case BEING_ENTITY: {
			entity *c = entityGetByBeing(b);
			if(c == NULL){return;}
			c->pos = vecAdd(c->pos,pos);
			return; }
		default:
			return;
	}
}

vec beingGetVel(being b){
	switch(beingType(b)){
		case BEING_CHARACTER: {
			character *c = characterGetByBeing(b);
			if(c == NULL){return vecNOne();}
			return c->vel; }
		case BEING_ANIMAL: {
			animal *c = animalGetByBeing(b);
			if(c == NULL){return vecNOne();}
			return c->vel; }
		case BEING_HOOK: {
			hook *c = hookGetByBeing(b);
			if(c == NULL){return vecNOne();}
			return c->ent->vel; }
		case BEING_ENTITY: {
			entity *c = entityGetByBeing(b);
			if(c == NULL){return vecNOne();}
			return c->vel; }
		default:
			return vecNOne();
	}
}

void beingSetVel(being b, const vec vel){
	switch(beingType(b)){
		case BEING_CHARACTER: {
			character *c = characterGetByBeing(b);
			if(c == NULL){return;}
			c->vel = vel;
			return; }
		case BEING_ANIMAL: {
			animal *c = animalGetByBeing(b);
			if(c == NULL){return;}
			c->vel = vel;
			return; }
		case BEING_HOOK: {
			hook *c = hookGetByBeing(b);
			if(c == NULL){return;}
			c->ent->vel = vel;
			return; }
		case BEING_ENTITY: {
			entity *c = entityGetByBeing(b);
			if(c == NULL){return;}
			c->vel = vel;
			return; }
		default:
			return;
	}
}

void beingAddVel(being b, const vec vel){
	switch(beingType(b)){
		case BEING_CHARACTER: {
			character *c = characterGetByBeing(b);
			if(c == NULL){return;}
			c->vel = vecAdd(c->vel,vel);
			return; }
		case BEING_ANIMAL: {
			animal *c = animalGetByBeing(b);
			if(c == NULL){return;}
			c->vel = vecAdd(c->vel,vel);
			return; }
		case BEING_HOOK: {
			hook *c = hookGetByBeing(b);
			if(c == NULL){return;}
			c->ent->vel = vecAdd(c->ent->vel,vel);
			return; }
		case BEING_ENTITY: {
			entity *c = entityGetByBeing(b);
			if(c == NULL){return;}
			c->vel = vecAdd(c->vel,vel);
			return; }
		default:
			return;
	}
}

being beingClosest(const vec pos, float maxDistance){
	character *c = characterClosest(pos,maxDistance);
	if(c != NULL){return characterGetBeing(c);}
	animal *a = animalClosest(pos,maxDistance);
	if(a != NULL){return animalGetBeing(a);}
	return 0;
}

float beingGetWeight(being b){
	switch(beingType(b)){
		case BEING_CHARACTER:
			return 80.f;
		case BEING_ANIMAL:
			return 10.f;
		case BEING_HOOK:
			return 1.f;
		case BEING_ENTITY:
			return 1.f;
		default:
			return 1.f;
	}
}