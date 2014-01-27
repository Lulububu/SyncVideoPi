#include "SchedulerMedia.h"

SchedulerMedia::SchedulerMedia()
{
}
SchedulerMedia::~SchedulerMedia(){}

void SchedulerMedia::addMedia(std::string path, std::string start, std::string end, bool loop)
{
	struct tm tm;
	// tm_isdst correspond à l'heure d'été. -1 laisse la fonction choisir.
	tm.tm_isdst = -1;
	strptime(start.c_str(), "%Y/%m/%d/%H:%M:%S", &(tm));
	time_t start_t = mktime(&tm);

	strptime(end.c_str(), "%Y/%m/%d/%H:%M:%S", &(tm));
	time_t end_t = mktime(&tm);

	m_program.push(new SyncVideo(path, start_t, end_t, m_wallWidth, m_wallHeight, m_tileWidth, m_tileHeight, m_tileX, m_tileY, loop));
}

void SchedulerMedia::run()
{
	while(!m_program.empty())
	{
		runNext();
		//deleteOldMedia();
	}
	cout << "program empty" << endl;
	Timer timer;
	if(!m_programOld.empty())
	{
		timer.SleepUntilTime(m_programOld.back()->m_dateEnd + 1);
	}
	deleteOldMedia();
	cout << "ii" << endl;
}

void SchedulerMedia::runNext()
{
	if(!m_program.empty())
	{
		SyncVideo* media = m_program.front();
		m_program.pop();
		Timer timer;
		timer.SleepUntilTime(media->m_dateStart - 5);
		cout << "create" << endl;
		media->Create();

		cout << "push" << endl;
		m_programOld.push(media);

		// timer.SleepUntilTime(media->m_dateEnd);

		// media->stop();
	}
}

void SchedulerMedia::loadScreenConfig(std::string configPath)
{
	

	std::ifstream infile(configPath);

	string key;
	float value;
	while (infile >> key >> value)
	{
		if(key.at(0) != '#')
		{
			cout <<"key " <<  key << " : " << value << endl;
			if(key == "wallWidth")
				m_wallWidth = value;
			else if(key == "wallHeight")
				m_wallHeight = value;
			else if(key == "tileX")
				m_tileX = value;
			else if(key == "tileY")
				m_tileY = value;
			else if(key == "tileWidth")
				m_tileWidth = value;
			else if(key == "tileHeight")
				m_tileHeight = value;
		}
	}
}

void SchedulerMedia::loadProgram(std::string configPath)
{
	std::ifstream infile(configPath);

	string path, dateStart, dateEnd, loop, line;
	while (getline(infile, line))
	{
		istringstream parser(line);
		
		if((parser >> path >> dateStart >> dateEnd >> loop) && path.at(0) != '#')
		{
			cout <<"file " <<  path << " start " << dateStart << " end " << dateEnd << " loop " << loop << endl;
			addMedia(path, dateStart, dateEnd, (loop == "LOOP"));
		}
	}

}


void SchedulerMedia::deleteOldMedia()
{
	cout << "delete " << endl;
	while(!m_programOld.empty())
	{
		cout << "program over" << endl;
		// delete (m_programOld.front());
		cout << "delete ok" << endl;
		m_programOld.pop();
		cout << "pop" << endl;
	}

	cout << "delete over" << endl;
}