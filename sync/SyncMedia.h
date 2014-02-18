#ifndef SYNCMEDIA_H
#define SYNCMEDIA_H


#include "OMXThread.h"
#include "sync/Timer.h"

class SyncMedia : public OMXThread
{
	
 
public:
	enum TypeMedia
	{
		TYPE_VIDEO,
		TYPE_IMAGE
	};

	virtual void Run() = 0;
	virtual TypeMedia GetType() = 0;
	virtual void Stop() = 0;

	time_t m_dateStart;
	time_t m_dateEnd;

protected:
	Timer m_timer;

	float m_wallWidth;
	float m_wallHeight;

	float m_tileWidth;
	float m_tileHeight;

	float m_tileX;
	float m_tileY;

	std::string m_filename;


};

#endif