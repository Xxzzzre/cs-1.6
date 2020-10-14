#include "client.h"

void NoFlash()
{
	if (cvar.visual_rem_flash && CheckDrawEngine())
	{
		screenfade_t sf;
		g_Engine.pfnGetScreenFade(&sf);
		sf.fader = 0;
		sf.fadeg = 0;
		sf.fadealpha = 128;
		g_Engine.pfnSetScreenFade(&sf);
	}
}