#include "client.h"

int StudioCheckBBox(void)
{
	if ((cvar.rage_active && cvar.rage_fov > 45) ||
		(cvar.legit[g_Local.weapon.m_iWeaponID].active && cvar.legit[g_Local.weapon.m_iWeaponID].fov > 45) ||
		(cvar.knifebot_active && cvar.knifebot_fov > 45))
		return 1;
	return g_Studio.StudioCheckBBox();
}

void StudioSetRemapColors(int top, int bottom)
{
	cl_entity_s* ent = g_Studio.GetCurrentEntity();
	GetHitboxes(ent);
	SetSkins(ent);
	g_Studio.StudioSetRemapColors(top, bottom);
}

void HookStudioFunctions()
{
	g_pStudio->StudioSetRemapColors = StudioSetRemapColors;
	g_pStudio->StudioCheckBBox = StudioCheckBBox;
}
