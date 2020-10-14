#ifndef _SPAWN_
#define _SPAWN_

typedef struct
{
	Vector Origin;
} spawnorigin_t;
extern deque<spawnorigin_t> SpawnOrigin;

extern DWORD DeadTickcount[33];
extern bool WasDead[33];
extern Vector DeadOrigin[33];
void DrawSpawn();
void Spawn();
void SaveSpawn();
void LoadSpawn();
void ResetSpawn();

#endif