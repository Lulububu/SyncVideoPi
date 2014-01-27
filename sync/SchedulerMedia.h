#ifndef SCHEDULERVIDEO_H
#define SCHEDULERVIDEO_H

#include <string>
#include <queue>
#include <fstream>

#include "sync/Timer.h"
#include "sync/SyncVideo.h"

class SchedulerMedia
{
public:
	SchedulerMedia();
	~SchedulerMedia();

	void addMedia(std::string path, std::string start, std::string end, bool loop);

	void run();
	
	void loadScreenConfig(std::string configPath);
	void loadProgram(std::string configPath);

private:
	std::queue<SyncVideo*> m_program;
	std::queue<SyncVideo*> m_programOld;

	float m_wallWidth;
	float m_wallHeight;

	float m_tileWidth;
	float m_tileHeight;

	float m_tileX;
	float m_tileY;

	void runNext();
	void deleteOldMedia();

};


#endif