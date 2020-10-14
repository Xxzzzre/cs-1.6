#include "client.h"

Vector DeadOrigin[33];
DWORD DeadTickcount[33];
bool WasDead[33];

deque<spawnorigin_t> SpawnOrigin;

void ResetSpawn()
{
	static char CurrentMap[256];
	if (strcmp(CurrentMap, g_Engine.pfnGetLevelName()))
	{
		SpawnOrigin.deque::clear();
		LoadSpawn();
		strcpy(CurrentMap, g_Engine.pfnGetLevelName());
	}
}
		
void Spawn()
{
	for (unsigned int id = 0; id < 33; id++)
	{
		if (WasDead[id])
		{
			if (GetTickCount() - DeadTickcount[id] > 1000)
				WasDead[id] = false;
		}
	}
	if (cvar.visual_spawn_scan && cvar.visual_spawn_points)
	{
		for (playeraim_t Aim : PlayerAim)
		{
			if (WasDead[Aim.ent->index])
			{
				if (g_Player[Aim.ent->index].bAliveInScoreTab && Aim.ent->origin != DeadOrigin[Aim.ent->index])
				{
					Vector vGrondOrigin = Aim.ent->origin;
					vGrondOrigin[2] -= 8192;
					pmtrace_t tr;
					g_pEngine->pEventAPI->EV_SetTraceHull(2);
					g_Engine.pEventAPI->EV_PlayerTrace(Aim.ent->origin, vGrondOrigin, PM_GLASS_IGNORE | PM_STUDIO_BOX, -1, &tr);
					bool bBadOrigin = false;
					for (spawnorigin_t Spawns : SpawnOrigin)
					{
						float distance = Spawns.Origin.Distance(tr.endpos);
						if (Aim.ent->origin[0] == Spawns.Origin[0] && Aim.ent->origin[1] == Spawns.Origin[1])
						{
							bBadOrigin = true;
							break;
						}
						else if (distance < 10)
						{
							bBadOrigin = true;
							break;
						}
					}
					if (!bBadOrigin)
					{
						spawnorigin_t Spawn;
						Spawn.Origin = tr.endpos;
						SpawnOrigin.push_back(Spawn);
					}
					WasDead[Aim.ent->index] = false;
				}
				DeadOrigin[Aim.ent->index] = Aim.ent->origin;
			}
		}
	}
}

void DrawSpawn()
{
	if (cvar.visual_spawn_points)
	{
		for (unsigned int i = 0; i < SpawnOrigin.size(); i++)
		{
			Vector vTop = SpawnOrigin[i].Origin;
			vTop[2] += 32;
			float vSpawnFrom[2];
			float vSpawnTo[2];
			if (WorldToScreen(SpawnOrigin[i].Origin, vSpawnFrom) && WorldToScreen(vTop, vSpawnTo))
			{
				float flRadius = (vSpawnFrom[1] - vSpawnTo[1]) / 10;
				int x = vSpawnFrom[0], y = vSpawnFrom[1], xx = vSpawnTo[0], yy = vSpawnTo[1];
				ImGui::GetCurrentWindow()->DrawList->AddLine({ (float)x, (float)y }, { (float)xx, (float)yy }, Green());
				ImGui::GetCurrentWindow()->DrawList->AddCircleFilled({ (float)xx, (float)yy }, 1 + flRadius, Wheel3());
				if (cvar.visual_spawn_num)
				{
					char str[32];
					sprintf(str, "%d", i);
					int label_size = ImGui::CalcTextSize(str, NULL, true).x;
					ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)xx - label_size / 2 - 2, (float)yy - 15 - flRadius }, { (float)xx - label_size / 2 + label_size + 3, (float)yy - 1 - flRadius }, Wheel1());
					ImGui::GetCurrentWindow()->DrawList->AddText({ (float)xx - label_size / 2, (float)yy - 16 - flRadius }, White(), str);
				}
			}
		}
	}
}

void SaveSpawn()
{
	char filename[256], levelname[256], tfilename[256];
	strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
	int len = strlen(levelname);
	if (len > 4)levelname[len - 4] = (char)0;
	sprintf(tfilename, "spawn/%s.point", levelname);
	sprintf(filename, "%s%s", hackdir, tfilename);
	remove(filename);
	char text[256];
	ofstream ofs(filename, ios::binary | ios::app);
	for (spawnorigin_t Spawn : SpawnOrigin)
	{
		sprintf(text, "Spawn: %f,%f,%f", Spawn.Origin[0], Spawn.Origin[1], Spawn.Origin[2]);
		ofs << text << (char)0x0D << (char)0x0A;
	}
	ofs.close();
}

void LoadSpawn()
{
	char filename[256], levelname[256], tfilename[256];
	strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
	int len = strlen(levelname);
	if (len > 4)levelname[len - 4] = (char)0;
	sprintf(tfilename, "spawn/%s.point", levelname);
	sprintf(filename, "%s%s", hackdir, tfilename);
	ifstream ifs(filename);
	while (ifs.good())
	{
		char buf[1024];
		ifs.getline(buf, sizeof(buf));
		float x, y, z;
		if (sscanf(buf, "Spawn: %f,%f,%f", &x, &y, &z))
		{
			if (x != 0 && y != 0 && z != 0)
			{
				Vector origin;
				origin.x = x;
				origin.y = y;
				origin.z = z;
				bool bBadOrigin = false;
				for (spawnorigin_t Spawns : SpawnOrigin)
				{
					float distance = Spawns.Origin.Distance(origin);
					if (x == Spawns.Origin[0] && y == Spawns.Origin[1])
					{
						bBadOrigin = true;
						break;
					}
					else if (distance < 10)
					{
						bBadOrigin = true;
						break;
					}
				}
				if (!bBadOrigin)
				{
					spawnorigin_t Spawn;
					Spawn.Origin = origin;
					SpawnOrigin.push_back(Spawn);
				}
			}
		}
	}
	ifs.close();
}