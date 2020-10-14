#ifndef _ESP_
#define _ESP_

typedef struct
{
	Vector HitboxMulti[8];
} playeresphitboxmulti_t;
extern deque<playeresphitboxmulti_t> PlayerEspHitboxMulti[33];

typedef struct
{
	cl_entity_s* ent;
} playeresp_t;
extern deque<playeresp_t> PlayerEsp;

void DrawPlayerEsp();
void DrawPlayerSoundIndexEsp();
void DrawPlayerSoundNoIndexEsp();

#endif