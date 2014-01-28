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

	void addMedia(std::string path, std::string start, std::string end, bool loop, string type);

	void run();
	
	void loadScreenConfig(std::string configPath);
	void loadProgram(std::string configPath);

	static volatile sig_atomic_t m_run;
	// static Timer m_timer;


private:
	std::queue<SyncMedia*> m_program;
	std::queue<SyncMedia*> m_programOld;

	float m_wallWidth;
	float m_wallHeight;

	float m_tileWidth;
	float m_tileHeight;

	float m_tileX;
	float m_tileY;

	Timer m_timer;
	
	void runNext();
	void deleteOldMedia();
	void deleteAllMedia();

};


#endif