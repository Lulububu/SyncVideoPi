#include "SchedulerMedia.h"

// volatile sig_atomic_t SchedulerMedia::m_run = false;
// // Timer SchedulerMedia::m_timer;

// void sig_handler(int s)
// {
//   printf("strg-c catched\n");
//   signal(SIGINT, SIG_DFL);
//   SchedulerMedia::m_run = false;
//   // SchedulerMedia::m_timer.Stop();
// }

SchedulerMedia::SchedulerMedia()
{
	// signal(SIGINT, sig_handler);
}

SchedulerMedia::~SchedulerMedia(){}

void SchedulerMedia::addMedia(std::string path, std::string start, std::string end, bool loop, string type)
{
	struct tm tm;
	// tm_isdst correspond à l'heure d'été. -1 laisse la fonction choisir.
	tm.tm_isdst = -1;
	cout << "tempo " << start << endl;
	strptime(start.c_str(), "%Y/%m/%d/%H:%M:%S", &(tm));
	time_t start_t = mktime(&tm);

	strptime(end.c_str(), "%Y/%m/%d/%H:%M:%S", &(tm));
	time_t end_t = mktime(&tm);

	if(type == "VIDEO")
		m_program.push(new SyncVideo(path, start_t, end_t, m_wallWidth, m_wallHeight, m_tileWidth, m_tileHeight, m_tileX, m_tileY, loop));
	else if(type == "IMAGE")
		cout << "creation image" << end;
}

void SchedulerMedia::run()
{
	// SchedulerMedia::m_run = true;

	while(!m_program.empty())
	{
		runNext();
		//deleteOldMedia();
	}
	cout << "program empty" << endl;
	Timer m_timer;
	if(!m_programOld.empty())
	{
		m_timer.SleepUntilTime(m_programOld.back()->m_dateEnd + 1);
	}
	//deleteAllMedia();
}

void SchedulerMedia::runNext()
{
	if(!m_program.empty())
	{
		Timer m_timer;
		SyncMedia* media = m_program.front();
		m_program.pop();
		m_timer.SleepUntilTime(media->m_dateStart - 5);
		cout << "create" << endl;
		media->Run();

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

	string path, dateStart, dateEnd, loop, line, type;
	while (getline(infile, line))
	{
		istringstream parser(line);
		
		if((parser >> type >> path >> dateStart >> dateEnd >> loop) && path.at(0) != '#')
		{
			cout <<"type " << type << " file " <<  path << " start " << dateStart << " end " << dateEnd << " loop " << loop << endl;
			addMedia(path, dateStart, dateEnd, (loop == "LOOP"), type);
		}
	}

}


void SchedulerMedia::deleteOldMedia()
{
	cout << "delete Old" << endl;
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

void SchedulerMedia::deleteAllMedia()
{
	cout << "delete All" << endl;
	while(!m_programOld.empty())
	{
		m_programOld.front()->Stop();
		m_programOld.front()->StopThread();
		// delete (m_programOld.front());
		m_programOld.pop();
	}

	while(!m_program.empty())
	{
		m_program.front()->Stop();
		m_program.front()->StopThread();
		// delete (m_programOld.front());
		m_program.pop();
	}

	cout << "delete over" << endl;
}