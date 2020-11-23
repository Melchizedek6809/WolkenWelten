#pragma once
#include "../common.h"

#include "../../../server/src/tmp/objs.h"
#include "../../../server/src/tmp/sfx.h"

#define I_Dirt           1
#define I_Grass          2
#define I_Stone          3
#define I_Coal           4
#define I_Spruce         5
#define I_Spruce_Leaf    6
#define I_Roots          7
#define I_Dirt_Roots     8
#define I_Obsidian       9
#define I_Oak           10
#define I_Oak_Leaf      11
#define I_Marble_Block  12
#define I_Hematite_Ore  13
#define I_Marble_Pillar 14
#define I_Marble_Blocks 15
#define I_Hewn_Log      16
#define I_Board         17
#define I_Crystal       18
#define I_Sakura        19
#define I_Birch         20
#define I_Flower        21

#define I_Grenade       256
#define I_Bomb          257
#define I_Pear          258
#define I_Stone_Axe     259
#define I_Stone_Pick    260
#define I_Blaster       261
#define I_MasterB       262
#define I_AssaultB      263
#define I_ShotgunB      264
#define I_Bullet        265
#define I_Iron_Bar      266
#define I_Iron_Axe      267
#define I_Iron_Pick     268
#define I_Crystal_Bar   269
#define I_Crystal_Axe   270
#define I_Crystal_Pick  271
#define I_Cherry        272
#define I_ClusterBomb   273
#define I_Glider        274
#define I_Hook          275
#define I_Jetpack       276
#define I_Poop          277
#define I_Meat          278
#define I_Cookedmeat    279
#define I_Fur           280
#define I_Burntmeat     281
#define I_FlintAndSteel 282

void recipeNew1 (const item result, const item ingred1);
void recipeNew2 (const item result, const item ingred1, const item ingred2);
void recipeNew3 (const item result, const item ingred1, const item ingred2, const item ingred3);
void recipeNew4 (const item result, const item ingred1, const item ingred2, const item ingred3, const item ingred4);

void ingredientSubstituteAdd (u16 ingredient, u16 substitute);

void explode       (const vec pos, float pwr, int style);
void grenadeNew    (const character *ent, float pwr, int cluster, float clusterPwr);
void beamblast     (character *ent, float beamSize, float damageMultiplier, float recoilMultiplier, int hitsLeft, int shots, float inaccuracyInc, float inaccuracyMult);
void projectileNew (const vec pos, const vec rot, being target, being source, uint style, float speed);
void projectileNewC(const character *c, being target, uint style);

bool characterHP             (      character *c, int addhp);
int  characterGetHP          (const character *c);
int  characterGetMaxHP       (const character *c);
bool characterDamage         (      character *c, int hp);
void characterAddCooldown    (      character *c, int cooldown);
int  characterGetItemAmount  (const character *c, u16 itemID);
int  characterDecItemAmount  (      character *c, u16 itemID, int amount);
void characterAddInaccuracy  (      character *c, float inc);
void characterAddRecoil      (      character *c, float recoil);
bool characterItemReload     (      character *c, item *i, int cooldown);
bool characterTryToShoot     (      character *c, item *i, int cooldown, int bulletcount);
bool characterTryToUse       (      character *c, item *i, int cooldown, int itemcount);
bool characterPlaceBlock     (      character *c, item *i);
void characterStartAnimation (      character *c, int index, int duration);
ivec characterLOSBlock       (const character *c, int returnBeforeBlock);
character *characterGetByBeing(being b);
being      characterGetBeing (const character *c);
int  characterHitCheck       (const vec pos, float mdd, int damage, int cause, u16 iteration, being source);

void fireNew       (u16 x, u16 y, u16 z, i16 strength);
void fireBox       (int x, int y, int z, int w, int h, int d);

item  itemNew      (u16 ID, i16 amount);
item  itemEmpty    ();
void  itemDiscard  (      item *i);
bool  itemIsEmpty  (const item *i);

int   itemCanStack (const item *i, u16 ID);
int   itemIncStack (      item *i, i16 amount);
int   itemDecStack (      item *i, i16 amount);

int   itemGetAmmo  (const item *i);
int   itemIncAmmo  (      item *i, i16 amount);
int   itemDecAmmo  (      item *i, i16 amount);

void itemDropNewP  (const vec pos,const item *itm);

void     worldBox           (int x, int y, int z, int w, int h, int d, u8 block);
void     worldBoxSphere     (int x, int y, int z, int r, u8 block);
uint8_t  worldGetB          (int x, int y, int z);
chungus *worldTryChungus    (int x, int y, int z);
chungus *worldGetChungus    (int x, int y, int z);
bool     worldSetB          (int x, int y, int z, u8 block);
int      checkCollision     (int x, int y, int z);
void     worldBoxMine       (int x, int y, int z, int w,int h,int d);
void     worldBoxMineSphere (int x, int y, int z, int r);

void     sfxPlay   (sfx *b, float volume);
void     sfxPlayPos(sfx *b, float volume, vec pos);
void     sfxLoop   (sfx *b, float volume);

float       rngValf();
u64         rngValR();
u64         rngValM(u64 max);
i64         rngValMM(i64 min,i64 max);

const char   *blockTypeGetName         (u8 b);
int           blockTypeGetHP           (u8 b);
int           blockTypeGetFireDmg      (u8 b);
blockCategory blockTypeGetCat          (u8 b);
bool          blockTypeValid           (u8 b);
u32           blockTypeGetParticleColor(u8 b);
mesh         *blockTypeGetMesh         (u8 b);

void vegShrub              (int x,int y,int z);
void vegDeadTree           (int x,int y,int z);
void vegBigDeadTree        (int x,int y,int z);
void vegSpruce             (int x,int y,int z);
void vegBigSpruce          (int x,int y,int z);
void vegOak                (int x,int y,int z);
void vegBigOak             (int x,int y,int z);
