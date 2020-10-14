extern int gui_radio_channel;
extern bool gui_radio;
extern float gui_radio_volume; 
extern bool channelchange;
extern bool channeltype;
extern char zstationstype[256];
extern bool forwards, backwards;

static string zstations[] = {
			"http://pulseedm.cdnstream1.com:8124/1373_128",
			"http://uk5.internet-radio.com:8270/",
			"http://streams.bigfm.de/bigfm-deutschland-128-mp3",
			"https://streams.bigfm.de/bigfm-deutschrap-128-mp3",
			"http://air2.radiorecord.ru:805/rr_320",
			"http://air.radiorecord.ru:805/dub_320",
			"http://air.radiorecord.ru:805/teo_320",
			"http://air.radiorecord.ru:805/dc_320",
			"http://pool.anison.fm:9000/AniSonFM(320)?nocache=0.752104723294601",
			"http://lyd.nrk.no/nrk_radio_p13_mp3_h"
};

void InitRadio();
void PlayRadio();
