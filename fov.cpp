#include "client.h"

void CustomFOV()
{
	int fov = cvar.visual_custom_fov;
	if (CheckDrawEngine() && bAliveLocal())
	{
		if (g_Local.iFOV == 90)(*pSetFOV)("SetFOV", 1, &fov);
	}
	else
	{
		fov = 90;
		if (g_Local.iFOV == 90)(*pSetFOV)("SetFOV", 1, &fov);
	}
}