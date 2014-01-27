#ifndef SYNCVIDEO_H
#define SYNCVIDEO_H

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <string.h>

#define AV_NOWARN_DEPRECATED

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};

#include "OMXStreamInfo.h"

#include "utils/log.h"

#include "DllAvUtil.h"
#include "DllAvFormat.h"
#include "DllAvFilter.h"
#include "DllAvCodec.h"
#include "linux/RBP.h"

#include "OMXVideo.h"
#include "OMXAudioCodecOMX.h"
#include "utils/PCMRemap.h"
#include "OMXClock.h"
#include "OMXAudio.h"
#include "OMXReader.h"
#include "OMXPlayerVideo.h"
#include "OMXPlayerAudio.h"
#include "OMXPlayerSubtitles.h"
#include "DllOMX.h"
#include "Srt.h"

#include "sync/Timer.h"


extern "C" {
#include <pwtilemap.h>
#include <pwutil.h>
};

#include <string>
#include <sstream>
#include <utility>

#include "sync/Timer.h"


class SyncVideo: public OMXThread
{
public:
	SyncVideo();

	SyncVideo(const SyncVideo&);
	
	SyncVideo(std::string path, time_t dateStart, time_t dateEnd, float wallWidth = -1, float wallHeight = -1, float tileWidth = -1, float tileHeight = -1, float tileX = -1, float tileY = -1, bool loop = false);
	virtual ~SyncVideo();

	int play();
	void Process();
	void Stop();

	bool isOver();

  	static volatile sig_atomic_t g_abort;

  	time_t m_dateStart;
	time_t m_dateEnd;

private:

	Timer m_timer;

	float m_wallWidth;
	float m_wallHeight;

	float m_tileWidth;
	float m_tileHeight;

	float m_tileX;
	float m_tileY;

	bool m_loop = false;
	bool m_isOver = false;

	void Init(std::string path, time_t dateStart, time_t dateEnd, float wallWidth = -1, float wallHeight = -1, float tileWidth = -1, float tileHeight = -1, float tileX = -1, float tileY = -1, bool loop = false);



	std::string m_filename;

	typedef enum {CONF_FLAGS_FORMAT_NONE, CONF_FLAGS_FORMAT_SBS, CONF_FLAGS_FORMAT_TB } FORMAT_3D_T;
	enum PCMChannels  *m_pChannelMap        = NULL;
	
	bool              m_bMpeg               = false;
	bool              m_passthrough         = false;
	long              m_initialVolume       = 0;
	bool              m_Deinterlace         = false;
	bool              m_HWDecode            = false;
	std::string       deviceString          = "";
	int               m_use_hw_audio        = false;
	std::string       m_external_subtitles_path;
	bool              m_has_external_subtitles = false;
	std::string       m_font_path           = "/usr/share/fonts/truetype/freefont/FreeSans.ttf";
	bool              m_has_font            = false;
	float             m_font_size           = 0.055f;
	bool              m_centered            = false;
	PwTileMap         *m_tilemap            = NULL;
	pthread_mutex_t   m_tilemap_mutex       = PTHREAD_MUTEX_INITIALIZER;
	bool              m_remap               = false;
	unsigned int      m_subtitle_lines      = 3;
	bool              m_Pause               = false;
	OMXReader         m_omx_reader;
	int               m_audio_index_use     = -1;
	int               m_seek_pos            = 0;
	bool              m_buffer_empty        = true;
	bool              m_thread_player       = false;
	OMXClock          *m_av_clock           = NULL;
	COMXStreamInfo    m_hints_audio;
	COMXStreamInfo    m_hints_video;
	OMXPacket         *m_omx_pkt            = NULL;
	bool              m_hdmi_clock_sync     = false;
	bool              m_no_hdmi_clock_sync  = false;
	bool              m_stop                = false;
	int               m_subtitle_index      = -1;
	DllBcmHost        m_BcmHost;
	OMXPlayerVideo    m_player_video;
	OMXPlayerAudio    m_player_audio;
	OMXPlayerSubtitles  m_player_subtitles;
	int               m_tv_show_info        = 0;
	bool              m_has_video           = false;
	bool              m_has_audio           = false;
	bool              m_has_subtitle        = false;
	float             m_display_aspect      = 0.0f;
	bool              m_boost_on_downmix    = false;

	enum{ERROR=-1,SUCCESS,ONEBYTE};

	//#if WANT_KEYS
	static struct termios orig_termios;
	static int orig_fl;


	static void restore_termios();
	static void restore_fl();
	// void sig_handler(int s);
	void print_usage();
	void PrintSubtitleInfo();
	void SetSpeed(int iSpeed);
	static float get_display_aspect_ratio(HDMI_ASPECT_T aspect);
	static float get_display_aspect_ratio(SDTV_ASPECT_T aspect);
	void FlushStreams(double pts);
	void SetVideoMode(int width, int height, int fpsrate, int fpsscale, FORMAT_3D_T is3d, bool dump_mode);
	bool Exists(const std::string& path);
	bool IsURL(const std::string& str);
	bool omxplayer_remap_wanted(void);
 
  
};


#endif