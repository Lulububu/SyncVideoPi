#include "Timer.h"

#include <iostream>
using namespace std;

Timer::Timer()
{}

Timer::~Timer()
{}

bool Timer::SleepUntilTime(time_t target)
{
	running = true;

    struct timeval now;
    gettimeofday(&now, NULL); 

	struct tm * timeinfo;
  	char buffer [80];

    timeinfo = localtime (&(now.tv_sec));
  	strftime (buffer,80,"Now it's %I:%M:%S%p.",timeinfo);
  	cout << buffer << endl;

  	timeinfo = localtime (&(target));
  	strftime (buffer,80,"target is %I:%M:%S%p.",timeinfo);
  	cout << buffer << endl;

    int diff = difftime(target, now.tv_sec);
    cout << "time to wait " << diff << endl;
    if(diff > 1)
    {
        while(diff > 2 && running)
        {
            gettimeofday(&now, NULL); 
            diff = difftime(target, now.tv_sec);
            cout << "wait " << diff << endl;
            sleep(1);
        }

        gettimeofday(&now, NULL); 
        diff = difftime(target, now.tv_sec);

        while(diff >= 0 && running)
        {
            gettimeofday(&now, NULL); 
            diff = difftime(target, now.tv_sec);
        }

        cout << "time (micro "<< now.tv_usec<<")  " << endl;
        return running;
    }else
    {
        return false;
    }
}


bool Timer::Stop()
{
	bool tmp = running;
	running = false;

	return tmp;
}