#include "../voxel/chunk.h"

#include "../game/blockType.h"
#include "../gfx/gfx.h"
#include "../gfx/glew.h"
#include "../gfx/shader.h"
#include "../../../common/src/misc/misc.h"

#include <math.h>
#include <string.h>
#include <stdio.h>


#pragma pack(push, 1)
typedef struct vertexTiny {
	uint8_t x,y,z,f;
	uint16_t u,v;
} vertexTiny;
#pragma pack(pop)

vertexTiny blockMeshBuffer[1<<16];
int chunkFreeCount = 0;
int chunkCount     = 0;
int chunksGeneratedThisFrame = 0;
chunk *chunkFirstFree = NULL;

#ifdef __EMSCRIPTEN__
	chunk chunkList[1<<16];
	const float CHUNK_RENDER_DISTANCE = 320.f;
	const float CHUNK_FADEOUT_DISTANCE = 64.f;
#elif __HAIKU__
	chunk chunkList[1<<16];
	const float CHUNK_RENDER_DISTANCE = 256.f;
	const float CHUNK_FADEOUT_DISTANCE = 32.f;
#else
	chunk chunkList[1<<18];
	const float CHUNK_RENDER_DISTANCE = 512.f;
	const float CHUNK_FADEOUT_DISTANCE = 64.f;
#endif

int     chunkGetFree()               { return chunkFreeCount;           }
int     chunkGetActive()             { return chunkCount;               }
int     chunkGetGeneratedThisFrame() { return chunksGeneratedThisFrame; }
void    chunkResetCounter()          { chunksGeneratedThisFrame = 0;    }

void chunkOptimizePlane(uint32_t plane[CHUNK_SIZE][CHUNK_SIZE]){
	for(int y=15;y>=0;y--){
		for(int x=14;x>=0;x--){
			if(plane[x][y] && ((plane[x][y] & 0xFF) == (plane[x+1][y] & 0xFF)) && (((plane[x][y]>>16) & 0xFF) == ((plane[x+1][y]>>16)&0xFF))){
				plane[x][y] += plane[x+1][y] & 0xFF00;
				plane[x+1][y] = 0;
			}
		}
	}
	for(int x=15;x>=0;x--){
		for(int y=14;y>=0;y--){
			if(plane[x][y] && ((plane[x][y]&0xFF) == (plane[x][y+1]&0xFF)) && (((plane[x][y]>>8)&0xFF) == ((plane[x][y+1]>>8)&0xFF))){
				plane[x][y] += plane[x][y+1]&0xFF0000;
				plane[x][y+1] = 0;
			}
		}
	}
}

uint8_t chunkGetSides(int x,int y,int z,uint8_t *d,int size) {
	const int tize = size*size;
	uint8_t sides = 0;
	uint8_t *base = d + z + (y*size) + (x*tize);

	if((z == size-1) || (base[    1] == 0)){ sides |=  1;}
	if((z == 0     ) || (base[   -1] == 0)){ sides |=  2;}
	if((y == size-1) || (base[ size] == 0)){ sides |=  4;}
	if((y == 0     ) || (base[-size] == 0)){ sides |=  8;}
	if((x == size-1) || (base[ tize] == 0)){ sides |= 16;}
	if((x == 0     ) || (base[-tize] == 0)){ sides |= 32;}

	return sides;
}

chunk *chunkNew(uint16_t x,uint16_t y,uint16_t z){
	chunk *c = NULL;
	if(chunkFirstFree == NULL){
		if(chunkCount >= (int)(sizeof(chunkList) / sizeof(chunk))-1){
			fprintf(stderr,"chunkList Overflow!\n");
			return NULL;
		}
		c = &chunkList[chunkCount++];
	}else{
		c = chunkFirstFree;
		chunkFirstFree = c->nextFree;
		chunkFreeCount--;
	}
	c->x         = x & (~0xF);
	c->y         = y & (~0xF);
	c->z         = z & (~0xF);
	c->ready     = 0;
	c->nextFree  = NULL;
	c->vbo       = 0;
	c->vboSize   = 0;
	c->dataCount = 0;
	memset(c->data,0,sizeof(c->data));
	return c;
}

void chunkFree(chunk *c){
	if(c == NULL){return;}
	chunkFreeCount++;
	if(!c->vbo){
		glDeleteBuffers(1,&c->vbo);
	}
	c->nextFree = chunkFirstFree;
	chunkFirstFree = c;
}

inline void chunkFinish(chunk *c){
	if(!c->vbo) { glGenBuffers(1,&c->vbo); }
	glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
	if(c->dataCount <= c->vboSize){
		glBufferSubData(GL_ARRAY_BUFFER, 0, c->dataCount*sizeof(vertexTiny), blockMeshBuffer);
	}else{
		glBufferData(GL_ARRAY_BUFFER, c->dataCount*sizeof(vertexTiny), blockMeshBuffer, GL_STATIC_DRAW);
		c->vboSize = c->dataCount;
	}
}

inline void chunkAddVert(chunk *c, uint8_t x,uint8_t y,uint8_t z,uint16_t u,uint16_t v,uint8_t f){
	blockMeshBuffer[c->dataCount++] = (vertexTiny){x,y,z,f,u,v};
}
void chunkAddFront(chunk *c, uint8_t b,uint8_t x,uint8_t y,uint8_t z, uint8_t w, uint8_t h, uint8_t d) {
	const uint16_t texXa = blockTypeGetTexX(b,0)<<8;
	const uint16_t texYa = blockTypeGetTexY(b,0)<<8;
	const uint16_t texXb = texXa|w;
	const uint16_t texYb = texYa|h;
	chunkAddVert(c, x  ,y  ,z+d,texXa,texYb,2);
	chunkAddVert(c, x+w,y  ,z+d,texXb,texYb,2);
	chunkAddVert(c, x+w,y+h,z+d,texXb,texYa,2);
	chunkAddVert(c, x+w,y+h,z+d,texXb,texYa,2);
	chunkAddVert(c, x  ,y+h,z+d,texXa,texYa,2);
	chunkAddVert(c, x  ,y  ,z+d,texXa,texYb,2);
}
void chunkAddBack(chunk *c, uint8_t b,uint8_t x,uint8_t y,uint8_t z, uint8_t w, uint8_t h, uint8_t d) {
	(void)d;
	const uint16_t texXa = blockTypeGetTexX(b,1)<<8;
	const uint16_t texYa = blockTypeGetTexY(b,1)<<8;
	const uint16_t texXb = texXa|w;
	const uint16_t texYb = texYa|h;
	chunkAddVert(c, x  ,y  ,z  ,texXb,texYb,2);
	chunkAddVert(c, x  ,y+h,z  ,texXb,texYa,2);
	chunkAddVert(c, x+w,y+h,z  ,texXa,texYa,2);
	chunkAddVert(c, x+w,y+h,z  ,texXa,texYa,2);
	chunkAddVert(c, x+w,y  ,z  ,texXa,texYb,2);
	chunkAddVert(c, x  ,y  ,z  ,texXb,texYb,2);
}
void chunkAddTop(chunk *c, uint8_t b,uint8_t x,uint8_t y,uint8_t z, uint8_t w, uint8_t h, uint8_t d) {
	const uint16_t texXa = blockTypeGetTexX(b,2)<<8;
	const uint16_t texYa = blockTypeGetTexY(b,2)<<8;
	const uint16_t texXb = texXa|w;
	const uint16_t texYb = texYa|d;
	chunkAddVert(c, x  ,y+h,z  ,texXa,texYa,3);
	chunkAddVert(c, x  ,y+h,z+d,texXa,texYb,3);
	chunkAddVert(c, x+w,y+h,z+d,texXb,texYb,3);
	chunkAddVert(c, x+w,y+h,z+d,texXb,texYb,3);
	chunkAddVert(c, x+w,y+h,z  ,texXb,texYa,3);
	chunkAddVert(c, x  ,y+h,z  ,texXa,texYa,3);
}
void chunkAddBottom(chunk *c, uint8_t b,uint8_t x,uint8_t y,uint8_t z, uint8_t w, uint8_t h, uint8_t d) {
	(void)h;
	const uint16_t texXa = blockTypeGetTexX(b,3)<<8;
	const uint16_t texYa = blockTypeGetTexY(b,3)<<8;
	const uint16_t texXb = texXa|w;
	const uint16_t texYb = texYa|d;
	chunkAddVert(c, x  ,y  ,z  ,texXb,texYa,0);
	chunkAddVert(c, x+w,y  ,z  ,texXa,texYa,0);
	chunkAddVert(c, x+w,y  ,z+d,texXa,texYb,0);
	chunkAddVert(c, x+w,y  ,z+d,texXa,texYb,0);
	chunkAddVert(c, x  ,y  ,z+d,texXb,texYb,0);
	chunkAddVert(c, x  ,y  ,z  ,texXb,texYa,0);
}
void chunkAddRight(chunk *c, uint8_t b,uint8_t x,uint8_t y,uint8_t z, uint8_t w, uint8_t h, uint8_t d) {
	const uint16_t texXa = blockTypeGetTexX(b,4)<<8;
	const uint16_t texYa = blockTypeGetTexY(b,4)<<8;
	const uint16_t texXb = texXa|d;
	const uint16_t texYb = texYa|h;
	chunkAddVert(c, x+w,y  ,z  ,texXb,texYb,2);
	chunkAddVert(c, x+w,y+h,z  ,texXb,texYa,2);
	chunkAddVert(c, x+w,y+h,z+d,texXa,texYa,2);
	chunkAddVert(c, x+w,y+h,z+d,texXa,texYa,2);
	chunkAddVert(c, x+w,y  ,z+d,texXa,texYb,2);
	chunkAddVert(c, x+w,y  ,z  ,texXb,texYb,2);
}
void chunkAddLeft(chunk *c, uint8_t b,uint8_t x,uint8_t y,uint8_t z, uint8_t w, uint8_t h, uint8_t d) {
	(void)w;
	const uint16_t texXa = blockTypeGetTexX(b,5)<<8;
	const uint16_t texYa = blockTypeGetTexY(b,5)<<8;
	const uint16_t texXb = texXa|d;
	const uint16_t texYb = texYa|h;
	chunkAddVert(c, x  ,y  ,z  ,texXa,texYb,2);
	chunkAddVert(c, x  ,y  ,z+d,texXb,texYb,2);
	chunkAddVert(c, x  ,y+h,z+d,texXb,texYa,2);
	chunkAddVert(c, x  ,y+h,z+d,texXb,texYa,2);
	chunkAddVert(c, x  ,y+h,z  ,texXa,texYa,2);
	chunkAddVert(c, x  ,y  ,z  ,texXa,texYb,2);
}

void chunkGenMesh(chunk *c) {
	int ac,bc;
	uint8_t sides;
	const int CS   = CHUNK_SIZE;
	const int CSCS = CS*CS;
	uint8_t *d = (uint8_t *)c->data;
	static uint8_t sideCache[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	static uint32_t   aplane[CHUNK_SIZE][CHUNK_SIZE];
	static uint32_t   bplane[CHUNK_SIZE][CHUNK_SIZE];
	if(++chunksGeneratedThisFrame > 64){return;}
	c->dataCount = 0;

	for(int x=CS-1;x>=0;--x){
		for(int y=CS-1;y>=0;--y){
			for(int z=CS-1;z>=0;--z){
				if(d[(x*CSCS)+(y*CS)+z] == 0){
					sideCache[x][y][z] = 0;
				}else{
					sideCache[x][y][z] = chunkGetSides(x,y,z,(uint8_t *)d,CS);
				}
			}
		}
	}

	// Front / Back
	for(int z=CS-1;z>=0;--z){
		ac = bc = 0;
		memset(aplane,0,CHUNK_SIZE*CHUNK_SIZE*4);
		memset(bplane,0,CHUNK_SIZE*CHUNK_SIZE*4);
		for(int y=CS-1;y>=0;--y){
			for(int x=CS-1;x>=0;--x){
				if(!d[(x*CSCS)+(y*CS)+z]){continue;}
				sides = sideCache[x][y][z];
				if(sides&1){
					ac++;
					aplane[y][x] = d[(x*CSCS)+(y*CS)+z] | 0x010100;
				}
				if(sides&2){
					bc++;
					bplane[y][x] = d[(x*CSCS)+(y*CS)+z] | 0x010100;
				}
			}
		}
		chunkOptimizePlane(aplane);
		chunkOptimizePlane(bplane);
		if(ac || bc){
			const int cd = 1;
			for(int y=CS-1;y>=0;--y){
				for(int x=CS-1;x>=0;--x){
					if(aplane[y][x]){
						const int cw = ((aplane[y][x] >> 16) & 0xFF);
						const int ch = ((aplane[y][x] >>  8) & 0xFF);
						const uint8_t b = aplane[y][x] & 0xFF;
						chunkAddFront(c,b,x,y,z,cw,ch,cd);
					}
					if(bplane[y][x]){
						const int cw = ((bplane[y][x] >> 16) & 0xFF);
						const int ch = ((bplane[y][x] >>  8) & 0xFF);
						const uint8_t b = bplane[y][x] & 0xFF;
						chunkAddBack(c,b,x,y,z,cw,ch,cd);
					}
				}
			}
		}
	}

	// Top / Bottom
	for(int y=CS-1;y>=0;--y){
		ac = bc = 0;
		memset(aplane,0,CHUNK_SIZE*CHUNK_SIZE*4);
		memset(bplane,0,CHUNK_SIZE*CHUNK_SIZE*4);
		for(int z=CS-1;z>=0;--z){
			for(int x=CS-1;x>=0;--x){
				if(!d[(x*CSCS)+(y*CS)+z]){continue;}
				sides = sideCache[x][y][z];
				if(sides&4){
					ac++;
					aplane[z][x] = d[(x*CSCS)+(y*CS)+z] | 0x010100;
				}
				if(sides&8){
					bc++;
					bplane[z][x] = d[(x*CSCS)+(y*CS)+z] | 0x010100;
				}
			}
		}
		chunkOptimizePlane(aplane);
		chunkOptimizePlane(bplane);
		if(ac || bc){
			const int ch = 1;
			for(int z=CS-1;z>=0;--z){
				for(int x=CS-1;x>=0;--x){
					if(aplane[z][x]){
						const int cw = ((aplane[z][x] >> 16) & 0xFF);
						const int cd = ((aplane[z][x] >>  8) & 0xFF);
						const uint8_t b = aplane[z][x] & 0xFF;
						chunkAddTop(c,b,x,y,z,cw,ch,cd);
					}
					if(bplane[z][x]){
						int cw = ((bplane[z][x] >> 16) & 0xFF);
						int cd = ((bplane[z][x] >>  8) & 0xFF);
						const uint8_t b = bplane[z][x] & 0xFF;
						chunkAddBottom(c,b,x,y,z,cw,ch,cd);
					}
				}
			}
		}
	}

	// Right / Left
	for(int x=CS-1;x>=0;--x){
		ac = bc = 0;
		memset(aplane,0,CHUNK_SIZE*CHUNK_SIZE*4);
		memset(bplane,0,CHUNK_SIZE*CHUNK_SIZE*4);
		for(int y=CS-1;y>=0;--y){
			for(int z=CS-1;z>=0;--z){
				if(!d[(x*CSCS)+(y*CS)+z]){continue;}
				sides = sideCache[x][y][z];
				if(sides&16){
					ac++;
					aplane[y][z] = d[(x*CSCS)+(y*CS)+z] | 0x010100;
				}
				if(sides&32){
					bc++;
					bplane[y][z] = d[(x*CSCS)+(y*CS)+z] | 0x010100;
				}
			}
		}
		chunkOptimizePlane(aplane);
		chunkOptimizePlane(bplane);
		if(ac || bc){
			const int cw = 1;
			for(int y=CS-1;y>=0;--y){
				for(int z=CS-1;z>=0;--z){
					if(aplane[y][z]){
						const int ch = ((aplane[y][z] >>  8) & 0xFF);
						const int cd = ((aplane[y][z] >> 16) & 0xFF);
						const uint8_t b = aplane[y][z] & 0xFF;
						chunkAddRight(c,b,x,y,z,cw,ch,cd);
					}
					if(bplane[y][z]){
						const int ch = ((bplane[y][z] >>  8) & 0xFF);
						const int cd = ((bplane[y][z] >> 16) & 0xFF);
						const uint8_t b = bplane[y][z] & 0xFF;
						chunkAddLeft(c,b,x,y,z,cw,ch,cd);
					}
				}
			}
		}
	}
	chunkFinish(c);
	c->ready |= 1;
}

void chunkBox(chunk *c, int x,int y,int z,int gx,int gy,int gz,uint8_t block){
	for(int cx=x;cx<gx;cx++){
		for(int cy=y;cy<gy;cy++){
			for(int cz=z;cz<gz;cz++){
				c->data[cx][cy][cz] = block;
			}
		}
	}
	c->ready &= ~15;
}

void chunkSetB(chunk *c,int x,int y,int z,uint8_t block){
	c->data[x&0xF][y&0xF][z&0xF] = block;
	c->ready &= ~15;
}

void chunkDraw(chunk *c, float d){
	if(!(c->ready & 1)){ chunkGenMesh(c); }
	if(!c->vbo){ return; }
	if(d > (CHUNK_RENDER_DISTANCE - CHUNK_FADEOUT_DISTANCE)){
		shaderAlpha(sBlockMesh,(1.f-((d-(CHUNK_RENDER_DISTANCE - CHUNK_FADEOUT_DISTANCE))/CHUNK_FADEOUT_DISTANCE)));
	}else{
		shaderAlpha(sBlockMesh,1.f);
	}
	shaderTransform(sBlockMesh,c->x,c->y,c->z);

	glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_BYTE,  GL_FALSE, sizeof(vertexTiny), (void *)(((char *)&blockMeshBuffer[0].x) - ((char *)blockMeshBuffer)));
	glVertexAttribPointer(1, 2, GL_SHORT,  GL_FALSE, sizeof(vertexTiny), (void *)(((char *)&blockMeshBuffer[0].u) - ((char *)blockMeshBuffer)));
	glVertexAttribPointer(2, 1, GL_UNSIGNED_BYTE,  GL_FALSE, sizeof(vertexTiny), (void *)(((char *)&blockMeshBuffer[0].f) - ((char *)blockMeshBuffer)));
	glDrawArrays(GL_TRIANGLES,0,c->dataCount);
	vboTrisCount += c->dataCount/3;
}
