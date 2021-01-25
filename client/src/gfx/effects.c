#include "effects.h"

#include "../game/blockType.h"
#include "../game/character.h"
#include "../game/rope.h"
#include "../gfx/particle.h"
#include "../gui/overlay.h"
#include "../network/chat.h"
#include "../sdl/sfx.h"
#include "../../../common/src/misc/misc.h"


void fxExplosionBomb(const vec pos,float pw){
	sfxPlayPos(sfxBomb,1.f,pos);

	for(int i=0;i<4096*pw;i++){
		const vec v  = vecMulS(vecRng(),0.15f*pw);
		newParticleV(pos,v,64.f,1.f,0xFF44AAFF,64);
	}
	for(int i=0;i<4096*pw;i++){
		const vec v  = vecMulS(vecRng(),0.1f*pw);
		newParticleV(pos,v,64.f,1.f,0xFF0099FF,78);
	}
	for(int i=0;i<2048*pw;i++){
		const vec v  = vecMulS(vecRng(),0.06f*pw);
		newParticleV(pos,v,64.f,1.f,0xFF0066CC,96);
	}
	for(int i=0;i<2048*pw;i++){
		const vec v  = vecMulS(vecRng(),0.03f*pw);
		newParticleV(pos,v,64.f,1.f,0xFF082299,128);
	}

	const float pd  = vecMag(vecSub(pos,player->pos));
	const float max = 16*pw*pw;
	if(pd < max){
		if(characterDamage(player,((max-pd)/max)*pw*4.f)){
			msgSendDyingMessage("got bombed", 65535);
			setOverlayColor(0x00000000,0);
			commitOverlayColor();
		}else{
			setOverlayColor(0xA03020F0,0);
			commitOverlayColor();
		}
	}
}
void fxGrenadeTrail(const vec pos,float pw){
	(void)pw;
	if((rngValR()&3)!=0){return;}
	const vec v = vecMulS(vecRng(),0.008f);
	newParticleV(pos,v,16.f,2.f,0xFF44AAFF,64);
}

void fxExplosionBlaster(const vec pos,float pw){
	for(int i=0;i<512;i++){
		const vec v = vecMulS(vecRng(),(1.f/8.f)*pw);
		newParticleV(pos,v,32.f,2.f,0xFF964AC0,156);
	}
	for(int i=0;i<512;i++){
		const vec v = vecMulS(vecRng(),(1.f/12.f)*pw);
		newParticleV(pos,v,16.f,4.f,0xFF7730A0,128);
	}
}
void fxBeamBlaster(const vec pa,const vec pb, float beamSize, float damageMultiplier){
	float lastDist = 999999.f;
	vec v,c = pb;

	u32 pac  = 0xD0000000 | ((0x50 + rngValM(0x40)) << 16) | ((0x30 + rngValM(0x40)) << 8) | (0xE0 + rngValM(0x1F));
	u32 pbc  = pac + 0x00202000;
	int ttl  = MIN(1024,MAX(64,12 * damageMultiplier));
	sfxPlayPos(sfxPhaser,MIN(0.5f,MAX(0.2f,damageMultiplier)),pa);

	for(uint max=1<<12;max;--max){
		vec dist = vecSub(pa,c);
		const float curDist = vecMag(dist);
		if(curDist > lastDist){break;}
		lastDist = curDist;
		v = vecMulS(vecNorm(dist),beamSize*64);
		vec po = vecZero();
		const vec postep = vecMulS(v,1.f/512.f);

		for(int i=0;i<512;i++){
			const vec pv = vecMulS(vecRng(),1/(beamSize*16));
			newParticleV(vecAdd(c,po),vecMulS(pv,1/12.f),beamSize*12,beamSize  ,pac,ttl  );
			newParticleV(vecAdd(c,po),vecMulS(pv,1/16.f),beamSize*12,beamSize/2,pbc,ttl*2);
			po = vecAdd(po,postep);
		}
		c = vecAdd(c,v);
	}
}

static void fxBlockBreakMining(const vec pos, u8 b){
	sfxPlayPos(sfxTock,1.f,pos);
	for(int i=0;i<1024;i++){
		const vec p = vecAdd(pos,vecRngAbs());
		newParticleS(p.x,p.y,p.z,blockTypeGetParticleColor(b),.7f,96);
	}
}
static void fxBlockBreakFire(const vec pos, u8 b){
	sfxPlayPos(sfxTock,0.2f,pos);
	for(int i=0;i<128;i++){
		const vec p = vecAdd(pos,vecRngAbs());
		newParticleS(p.x,p.y,p.z,blockTypeGetParticleColor(b),.7f,96);
	}
	for(int i=0;i<96;i++){
		newParticleV(vecAdd(pos,vecRngAbs()), vecAdd(vecNew(0,0.007f,0),vecMulS(vecRng(),0.005f)), 1.0f, 0.50f,0xFF60C8FF,  96);
		newParticleV(vecAdd(pos,vecRngAbs()), vecAdd(vecNew(0,0.007f,0),vecMulS(vecRng(),0.005f)), 0.7f, 0.65f,0xFF5098FF, 128);
		newParticleV(vecAdd(pos,vecRngAbs()), vecAdd(vecNew(0,0.007f,0),vecMulS(vecRng(),0.005f)), 0.6f, 0.75f,0xFF1F38EF, 156);
		newParticleV(vecAdd(pos,vecRngAbs()), vecAdd(vecNew(0,0.007f,0),vecMulS(vecRng(),0.005f)), 0.5f, 0.75f,0xFF1F38EF, 172);
	}
	for(int i=0;i<16;i++){
		const u32 c = 0x00101820 | (rngValR()&0x0003070F);
		newSparticleV(vecAdd(pos,vecRngAbs()), vecAdd(vecNew(0,0.001f+(rngValf()*0.001f),0),vecMulS(vecRng(),0.0001f)), 0.01f, 0.2f,c,768);
	}
}
static void fxBlockBreakVegetation(const vec pos, u8 b){
	sfxPlayPos(sfxTock,0.2f,pos);
	for(int i=0;i<256;i++){
		const vec p = vecAdd(pos,vecRngAbs());
		newParticleS(p.x,p.y,p.z,blockTypeGetParticleColor(b),.7f,256);
	}
}

void fxBlockBreak(const vec pos, u8 b, u8 cause){
	switch(cause){
	case 0: return fxBlockBreakMining(pos,b);
	case 1: return fxBlockBreakFire(pos,b);
	default:
	case 2: return fxBlockBreakVegetation(pos,b);
	}
}
void fxBlockMine(const vec pos, int dmg, unsigned char b){
	(void)dmg;
	for(int i=0;i<4;i++){
		const vec p = vecAdd(pos,vecRngAbs());
		newParticleS(p.x,p.y,p.z,blockTypeGetParticleColor(b),.9f,64);
	}
}

void fxBleeding(const vec pos, being victim, i16 dmg, u8 cause){
	(void)victim;
	(void)dmg;
	(void)cause;
	sfxPlayPos(sfxImpact,1,pos);
	sfxPlayPos(sfxUngh,1,pos);
	for(int i=dmg*64;i>0;i--){
		const vec v  = vecMulS(vecRng(),0.06f);
		newParticleV(pos,v,64.f,1.f,0xFF44AAFF,64);
	}
}

void fxAnimalDiedPacket (const packet *p){
	//const u8 type = p->v.u8[0];
	//const u8 age  = p->v.u8[1];
	const vec pos = vecNewP(&p->v.f[1]);
	being t = p->v.u32[4];
	sfxPlayPos(sfxBomb,0.3,pos);
	sfxPlayPos(sfxUngh,0.6,pos);
	for(int i=0;i<512;i++){
		const vec v  = vecMulS(vecRng(),0.06f);
		newParticleV(pos,v,64.f,1.f,0xFF44AAFF,64);
	}
	ropeDelBeing(t);
}

void fxProjectileHit(const packet *p){
	const u32 colorOffsets[] = {
		0x00010713,
		0x00031F39,
		0x000F0B1B,
		0x000C1D38
	};
	const u16 style = p->v.u16[0];

	//const u16 size  = p->v.u16[1];
	const vec pos = vecNewP(&p->v.f[1]);
	for(int i=256;i>0;i--){
		u32 color;
		uint ttl = 64;
		if(style == 3){
			color = 0xFF50A0F0 | colorOffsets[rngValA(3)];
			ttl   = 96;
		}else if(style == 2){
			if(rngValA(1)){
				color = 0xFFB05020;
			}else{
				color = 0xFF9F7830;
			}
		}else{
			if(rngValA(1)){
				color = 0xFF60C8FF;
			}else{
				color = 0xFF1F38EF;
			}
		}
		const vec v  = vecMulS(vecRng(),0.03f);
		newParticleV(pos,v,48.f,4.f,color,ttl);
	}
}

void fxRainDrop(const vec pos){
	for(int i=16;i>0;i--){
		u32 color = 0xFFB05020;
		const vec v  = vecMulS(vecRng(),0.01f);
		newParticleV(pos,v,16.f,6.f,color,64);
	}
}
