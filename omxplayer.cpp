
//  * 
//  *      Copyright (C) 2012 Edgar Hucek
//  *
//  * This program is free software; you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License as published by
//  * the Free Software Foundation; either version 2 of the License, or
//  * (at your option) any later version.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program; if not, write to the Free Software
//  * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 


#include "sync/SyncVideo.h"
#include "sync/Timer.h"
#include "sync/SchedulerMedia.h"

#include "sync/TestThread.h"

int main(int argc, char *argv[])
{
    
    SchedulerMedia sch;
    sch.loadScreenConfig(argv[3]);
    sch.loadProgram(argv[4]);
    sch.run();
    cout << "OVER" << endl;

    // struct timeval now;
    // gettimeofday(&now, NULL); 

    // struct tm * timeinfo;
    // char buffer [80];

    // timeinfo = localtime (&(now.tv_sec));
    // strftime (buffer,80,"Now it's %I:%M%p.",timeinfo);
    // cout << buffer << endl;

    // int tempsinit = 10;
    // int temps1 = tempsinit+15;
    // int temps2 = temps1 + 15;

    // SyncVideo media("videos/Castle.2009.S06E12.FASTSUB.VOSTFR.720p.HDTV.x264-PROTEiGON.mkv", now.tv_sec+tempsinit, now.tv_sec + temps1);
    // media.Create();
    // cout << "launch 1" << endl;

    // SyncVideo media2("videos/Castle.2009.S06E12.FASTSUB.VOSTFR.720p.HDTV.x264-PROTEiGON.mkv", now.tv_sec+temps1, now.tv_sec + temps2);
    // media2.Create();
    // cout << "launch 2" << endl;

    // Timer timer;
    // cout << "sleep 1" << endl;
    // timer.SleepUntilTime(now.tv_sec + temps1);
    // cout << "fin sleep 1" << endl;
    // media.stop();
    // cout << "stop 1" << endl;


    // cout << "sleep 2" << endl;
    // timer.SleepUntilTime(now.tv_sec + temps2);
    // cout << "fin sleep 2" << endl;
    // media2.stop();
    // cout << "stop 2" << endl;

    // media.StopThread();
    // media2.StopThread();

 // //  SyncVideo vid;
 //  struct tm tm;
 // //  // tm_isdst correspond à l'heure d'été. -1 laisse la fonction choisir.
 //  tm.tm_isdst = -1;
  
 // //  //printf("%s", argv[4]);
 //  strptime(argv[4], "%Y/%m/%d/%H:%M:%S", &(tm));
 //  time_t target = mktime(&tm);
  
	// // vid.play(argv[3], target);
	// // // play(argc, argv, "test.h264");
 // //  // vid.test();
 //  SyncVideo vid1, vid2;
 //  vid1.init(argv[3], target);
 //  vid2.init(argv[3], target+10);

 //  vid1.Create();
 //  vid2.Create();
 //  sleep(10);

 //  vid1.stop();


 //  // sleep(5);
 //  vid1.StopThread();
 //  vid2.StopThread();
 //  // sleep(2);
// CLog::Init("./");
// CLog::Init("./");
//     struct timeval now;
//     gettimeofday(&now, NULL); 

//     cout << "file " << argv[3] << endl;
//     SyncVideo media(argv[3], now.tv_sec+15, now.tv_sec+30);
//     media.Create();

//     media.StopThread();
//     cout << "fin" << endl;
}


