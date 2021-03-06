#pragma once
#include "stdint.h"
#include <time.h>

typedef struct {
	uint ID,w,h,d;
	const char *filename;
} texture;

#pragma pack(push, 1)
typedef struct vertex {
	float x,y,z;
	float u,v;
	float c;
} vertex;

typedef struct vertex2D {
	i16 x,y;
	i16 u,v;
	u32 rgba;
} vertex2D;
#pragma pack(pop)

typedef struct {
	vertex2D *dataBuffer;
	 int sx,sy,mx,my,wrap,size,font;
	 u32 fgc, bgc;
	uint vao,vbo,usage,dataCount,bufferSize,vboSize;
	texture *tex;
	 int finished;
} textMesh;

typedef struct {
	texture *tex;
	const vertex *roData;
	uint dataCount,vboSize;
	uint vbo,vao;
	void *nextFree;
} mesh;

typedef struct {
	uint pID;
	uint vsID,fsID;
	char *vss,*fss;
	uint attrMask;
  int lMVP,lAlpha,lTransform,lBrightness,lSideTint,lSizeMul;
} shader;

typedef struct {
	union {
		struct { float x,y,z; };
		struct { float v[3]; };
		struct { float yaw,pitch,roll; };
	};
} vec;

typedef struct {
	union {
		struct { int x,y,z; };
		struct { int w,h,d; };
	};
} ivec;

typedef struct {
	union {
		struct { uint x,y,z; };
		struct { uint w,h,d; };
	};
} uvec;

#pragma pack(push, 1)
typedef struct {
	float x,y,z,size;
} glRainDrop;
#pragma pack(pop)

typedef struct {
	float vx,vy,vz,vsize;
} rainDrop;
