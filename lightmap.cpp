#include "client.h"

void Lightmap()
{
	static bool runonce = true;
	if (cvar.visual_lightmap && CheckDrawEngine())
	{
		if (runonce)
		{
			g_Engine.OverrideLightmap(1);
			g_Engine.SetLightmapColor(1, 1, 1);
			g_Engine.SetLightmapDarkness(1);
			runonce = false;
		}
	}
	else
	{
		g_Engine.OverrideLightmap(0);
		runonce = true;
	}
}