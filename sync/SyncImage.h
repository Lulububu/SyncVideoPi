
#ifndef SYNC_IMAGE_H
#define SYNC_IMAGE_H


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <stdexcept>
#include <vector>
#include <time.h>
#include <string>


#include "SyncMedia.h"

#include "pngTools/rpi2d.h"

#include "imageTools/platform/PlatformRPI.h"
#include "imageTools/io/logger_console.h"
#include "imageTools/common/common.h"
#include "imageTools/video/VideoObjects/VideoObjectGLES2_EGL.h"

class SyncImage : public SyncMedia
{
	
public:
	SyncImage(std::string imagePath, time_t dateStart, time_t dateEnd, int wallWidth=-1, int wallHeight=-1, int tileWidth=-1, int tileHeight=-1, int tileX=-1, int tileY=-1);
	~SyncImage();
	void Load();
	void Display();

	void Run();
	TypeMedia GetType();
	void Stop();

	void Process();

private:			
	std::string m_imagePath;
	Raspberry2D *m_rpi2D;
	Texture2D *m_tex;

	std::string m_extension;

	IPlatformSPtr 	m_platform;
	PlatformRPI 	*m_pPlatform;
	IVideoObject 	*m_pVideo;
	IJPEGDecode 	*m_pJPEG;
	ILoggerSPtr m_logger;
	byteSA read_file();

};



#endif