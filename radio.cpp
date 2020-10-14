#include "client.h"

void InitRadio()
{
	BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory(bass_dll_image, sizeof(bass_dll_image));
	static bool once;
	if (!once)
	{
		if (BASS_Init(-1, 44100, 0, nullptr, nullptr))
		{
			BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
			BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
			BASS::stream_sounds.headshot = BASS_StreamCreateFile(TRUE, headshot, 0, sizeof(headshot), 0);
			BASS::stream_sounds.doublekill = BASS_StreamCreateFile(TRUE, doublekill, 0, sizeof(doublekill), 0);
			BASS::stream_sounds.triplekill = BASS_StreamCreateFile(TRUE, triplekill, 0, sizeof(triplekill), 0);
			BASS::stream_sounds.monsterkill = BASS_StreamCreateFile(TRUE, monsterkill, 0, sizeof(monsterkill), 0);
			once = true;
		}
	}
}