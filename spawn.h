#ifndef _SPAWN_
#define _SPAWN_

typedef struct
{
	Vector Origin;
} spawnorigin_t;
extern deque<spawnorigin_t> SpawnOrigin;

typedef struct
{
	int index;
	Vector Origin;
	DWORD Tickcount;
} spawndeath_t;
extern deque<spawndeath_t> SpawnDeath;

typedef struct
{
	int index;
	Vector Origin;
} spawn_t;
extern deque<spawn_t> Spawn;

void DrawSpawn();
void FindSpawn();
void SaveSpawn();
void LoadSpawn();
void ResetSpawn();

#endif