#include "sky.h"

#include "../gfx/gl.h"
#include "../gfx/mat.h"
#include "../gfx/gfx.h"
#include "../gfx/shader.h"
#include "../gfx/particle.h"
#include "../gfx/texture.h"
#include "../gfx/mesh.h"
#include "../game/entity.h"
#include "../voxel/chungus.h"
#include "../tmp/assets.h"
#include "../../../common/src/misc/noise.h"

#include <math.h>
#include <stdio.h>

float sunAngle = 40.f;

mesh *skyMesh;
mesh *sunMesh;
texture *tSky;
texture *tSun;

uint8_t cloudTex[256][256];
float cloudOffset=0.f;

#define CLOUD_FADED 65536
#define CLOUD_MIND  (65536*3)

void cloudsDraw(int cx, int cy, int cz){
	const int density = 168;
	const int toff    = cloudOffset;
	float sx=cx * 256.f + (cloudOffset - (float)toff);
	float sy=cy * 256.f;
	float sz=cz * 256.f;
	if(cy&1){return;}
	const float dy = (sy - player->y) * (sy - player->y);

	for(uint x=0;x<256;x++){
		const float px  = sx+x;
		const float dxy = dy + ((px - player->x) * (px - player->x));
		const int tx    = (x - toff)&0xFF;
		for(uint z=0;z<256;z++){
			const uint8_t  v  = cloudTex[tx][z];
			if(v < density){ continue; }
			const float    pz = sz+z;
			const float    dz = (pz - player->z) * (pz - player->z);
			const float    dd = dxy+dz;
			const float    vf = v-170;
			const uint8_t  ta = (208+((256 - v)/2));
			const uint8_t  tb = (188+((256 - v)/4));
			const uint8_t  ba = (164+((256 - v)  ));
			const uint8_t  bb = (148+((256 - v)/2));
			uint32_t a;
			if(dd > (CLOUD_MIND+CLOUD_FADED)){continue;}
			if(dd > CLOUD_MIND){
				a = (uint8_t)(v*(1.f-((dd - CLOUD_MIND)/CLOUD_FADED))) << 24;
			}else{
				a = v << 24;
			}
			const uint32_t ct = a | (tb<<16) | (ta<<8) | ta;
			const uint32_t cb = a | (bb<<16) | (ba<<8) | ba;

			newParticle(px,sy+vf/18.f,pz,0,0,0,0,0,0,1024,0,cb,1);
			newParticle(px,sy-vf/12.f,pz,0,0,0,0,0,0,1024,0,cb,1);
			newParticle(px,sy+vf/ 6.f,pz,0,0,0,0,0,0,1024,0,ct,1);
		}
	}
}


void initSky(){
	generateNoise(0x84407db3, cloudTex);
	tSky = textureNew(gfx_sky_png_data, gfx_sky_png_len, "client/gfx/sky.png");
	tSun = textureNew(gfx_sun_png_data, gfx_sun_png_len, "client/gfx/sun.png");

	skyMesh = meshNew();
	skyMesh->tex = tSky;

	meshAddVert(skyMesh, -1536,  1536, -1536, 0.250f, 0.00f);
	meshAddVert(skyMesh,  1536,  1536, -1536, 0.500f, 0.00f);
	meshAddVert(skyMesh,  1536,  1536,  1536, 0.500f, 0.33f);

	meshAddVert(skyMesh,  1536,  1536,  1536, 0.500f, 0.33f);
	meshAddVert(skyMesh, -1536,  1536,  1536, 0.250f, 0.33f);
	meshAddVert(skyMesh, -1536,  1536, -1536, 0.250f, 0.00f);

	meshAddVert(skyMesh, -1536, -1536, -1536, 0.500f, 0.66f);
	meshAddVert(skyMesh, -1536, -1536,  1536, 0.500f, 1.00f);
	meshAddVert(skyMesh,  1536, -1536,  1536, 0.250f, 1.00f);

	meshAddVert(skyMesh,  1536, -1536,  1536, 0.250f, 1.00f);
	meshAddVert(skyMesh,  1536, -1536, -1536, 0.250f, 0.66f);
	meshAddVert(skyMesh, -1536, -1536, -1536, 0.500f, 0.66f);



	meshAddVert(skyMesh,     0, -1536,  1536, 0.250f, 0.66f);
	meshAddVert(skyMesh,     0,  1536,  1536, 0.250f, 0.33f);
	meshAddVert(skyMesh,  1024,  1536,  1024, 0.375f, 0.33f);

	meshAddVert(skyMesh,  1024,  1536,  1024, 0.375f, 0.33f);
	meshAddVert(skyMesh,  1024, -1536,  1024, 0.375f, 0.66f);
	meshAddVert(skyMesh,     0, -1536,  1536, 0.250f, 0.66f);

	meshAddVert(skyMesh, -1024, -1536,  1024, 0.375f, 0.66f);
	meshAddVert(skyMesh, -1024,  1536,  1024, 0.375f, 0.33f);
	meshAddVert(skyMesh,     0,  1536,  1536, 0.500f, 0.33f);

	meshAddVert(skyMesh,     0,  1536,  1536, 0.500f, 0.33f);
	meshAddVert(skyMesh,     0, -1536,  1536, 0.500f, 0.66f);
	meshAddVert(skyMesh, -1024, -1536,  1024, 0.375f, 0.66f);


	meshAddVert(skyMesh,     0, -1536, -1536, 0.750f, 0.66f);
	meshAddVert(skyMesh,  1024,  1536, -1024, 0.875f, 0.33f);
	meshAddVert(skyMesh,     0,  1536, -1536, 0.750f, 0.33f);

	meshAddVert(skyMesh,  1024,  1536, -1024, 0.875f, 0.33f);
	meshAddVert(skyMesh,     0, -1536, -1536, 0.750f, 0.66f);
	meshAddVert(skyMesh,  1024, -1536, -1024, 0.875f, 0.66f);

	meshAddVert(skyMesh, -1024, -1536, -1024, 1.000f, 0.66f);
	meshAddVert(skyMesh,     0,  1536, -1536, 0.750f, 0.33f);
	meshAddVert(skyMesh, -1024,  1536, -1024, 0.875f, 0.33f);

	meshAddVert(skyMesh,     0,  1536, -1536, 1.000f, 0.33f);
	meshAddVert(skyMesh, -1024, -1536, -1024, 0.875f, 0.66f);
	meshAddVert(skyMesh,     0, -1536, -1536, 1.000f, 0.66f);


	meshAddVert(skyMesh,  1536, -1536,     0, 0.500f, 0.66f);
	meshAddVert(skyMesh,  1024,  1536,  1024, 0.625f, 0.33f);
	meshAddVert(skyMesh,  1536,  1536,     0, 0.500f, 0.33f);

	meshAddVert(skyMesh,  1024,  1536,  1024, 0.625f, 0.33f);
	meshAddVert(skyMesh,  1536, -1536,     0, 0.500f, 0.66f);
	meshAddVert(skyMesh,  1024, -1536,  1024, 0.625f, 0.66f);

	meshAddVert(skyMesh,  1024, -1536, -1024, 0.625f, 0.66f);
	meshAddVert(skyMesh,  1536,  1536,     0, 0.750f, 0.33f);
	meshAddVert(skyMesh,  1024,  1536, -1024, 0.655f, 0.33f);

	meshAddVert(skyMesh,  1536,  1536,     0, 0.750f, 0.33f);
	meshAddVert(skyMesh,  1024, -1536, -1024, 0.625f, 0.66f);
	meshAddVert(skyMesh,  1536, -1536,     0, 0.750f, 0.66f);


	meshAddVert(skyMesh, -1536, -1536,     0, 0.500f, 0.66f);
	meshAddVert(skyMesh, -1536,  1536,     0, 0.500f, 0.33f);
	meshAddVert(skyMesh, -1024,  1536,  1024, 0.625f, 0.33f);

	meshAddVert(skyMesh, -1024,  1536,  1024, 0.625f, 0.33f);
	meshAddVert(skyMesh, -1024, -1536,  1024, 0.625f, 0.66f);
	meshAddVert(skyMesh, -1536, -1536,     0, 0.500f, 0.66f);

	meshAddVert(skyMesh, -1024, -1536, -1024, 0.625f, 0.66f);
	meshAddVert(skyMesh, -1024,  1536, -1024, 0.655f, 0.33f);
	meshAddVert(skyMesh, -1536,  1536,     0, 0.750f, 0.33f);

	meshAddVert(skyMesh, -1536,  1536,     0, 0.750f, 0.33f);
	meshAddVert(skyMesh, -1536, -1536,     0, 0.750f, 0.66f);
	meshAddVert(skyMesh, -1024, -1536, -1024, 0.625f, 0.66f);
	meshFinish(skyMesh, GL_STATIC_DRAW);


	sunMesh = meshNew();
	sunMesh->tex = tSun;

	meshAddVert(sunMesh, -48,  512, -48, 0.0f, 0.0f);
	meshAddVert(sunMesh,  48,  512, -48, 1.0f, 0.0f);
	meshAddVert(sunMesh,  48,  512,  48, 1.0f, 1.0f);

	meshAddVert(sunMesh,  48,  512,  48, 1.0f, 1.0f);
	meshAddVert(sunMesh, -48,  512,  48, 0.0f, 1.0f);
	meshAddVert(sunMesh, -48,  512, -48, 0.0f, 0.0f);
	meshFinish(sunMesh, GL_STATIC_DRAW);
}

void renderSky(const character *cam){
	float matMVP[16];
	shaderBind(sMesh);

	matIdentity(matMVP);
	matMulRotXY(matMVP,cam->yaw,cam->pitch);
	matMul(matMVP,matMVP,matProjection);
	shaderMatrix(sMesh,matMVP);
	meshDrawLin(skyMesh);


	matIdentity(matMVP);
	matMulRotXY(matMVP,cam->yaw,cam->pitch);
	matMulRotX(matMVP,sunAngle);
	matMul(matMVP,matMVP,matProjection);
	shaderMatrix(sMesh,matMVP);
	meshDrawLin(sunMesh);
}
