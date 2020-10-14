#include "main.h"

int gui_radio_channel;
bool gui_radio;
float gui_radio_volume;
bool channelchange = false;
bool channeltype = false;
char zstationstype[256] = "http://";
bool forwards, backwards;

void InitRadio()
{
	static bool once = false;
	if (!once) BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory(bass_dll_image, sizeof(bass_dll_image));
	once = true;
}

bool radioInit = false;

void PlayRadio()
{
	static DWORD stream;
	if (forwards)
	{
		gui_radio_channel++;
		if (gui_radio_channel > ARRAYSIZE(zstations) - 1)
			gui_radio_channel = 0;

		BASS_ChannelStop(stream);
		stream = NULL;
		stream = BASS_StreamCreateURL(zstations[gui_radio_channel].c_str(), 0, 0, NULL, 0);
		forwards = false;
	}
	if (backwards)
	{
		if (gui_radio_channel == 0)
			gui_radio_channel = ARRAYSIZE(zstations);

		gui_radio_channel--;
		BASS_ChannelStop(stream);
		stream = NULL;
		stream = BASS_StreamCreateURL(zstations[gui_radio_channel].c_str(), 0, 0, NULL, 0);
		backwards = false;
	}
	if (channelchange)
	{
		BASS_ChannelStop(stream);
		stream = NULL;
		stream = BASS_StreamCreateURL(zstations[gui_radio_channel].c_str(), 0, 0, NULL, 0);
		channelchange = false;
	}
	if (channeltype)
	{
		BASS_ChannelStop(stream);
		stream = NULL;
		stream = BASS_StreamCreateURL(zstationstype, 0, 0, NULL, 0);
		if (!radioInit)
		{
			BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
			stream = BASS_StreamCreateURL(zstationstype, 0, 0, NULL, 0);
			radioInit = true;
		}
		gui_radio = true;
		channeltype = false;
	}
	static bool stop_radio = gui_radio;
	if (stop_radio != gui_radio)
	{
		if (!gui_radio)
		{
			BASS_ChannelStop(stream);
			stream = NULL;
		}
		else
		{
			if (radioInit)
				stream = BASS_StreamCreateURL(zstations[gui_radio_channel].c_str(), 0, 0, NULL, 0);
		}
		stop_radio = gui_radio;
	}
	if (gui_radio)
	{
		if (!radioInit)
		{
			BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
			stream = BASS_StreamCreateURL(zstations[gui_radio_channel].c_str(), 0, 0, NULL, 0);
			radioInit = true;
		}
		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, gui_radio_volume / 100);
		BASS_ChannelPlay(stream, false);
	}
}