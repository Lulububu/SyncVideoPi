
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

#include "sync/SyncImage.h"

int main(int argc, char *argv[])
{
    
    SchedulerMedia sch;
    sch.loadScreenConfig(argv[3]);
    sch.loadProgram(argv[4]);
    sch.run();
    cout << "OVER" << endl;
    sleep(10);

    // SyncImage img(argv[3], 0, 0);
    // img.Load();
    // img.Display();

    // sleep(10);

    // struct timeval now;
    // gettimeofday(&now, NULL); 

    // cout << "file " << argv[3] << endl;
    // // cout << "wallW " << stoi(argv[4]) << "wallH " << stoi(argv[5])  << endl;
    // SyncImage media(argv[3], now.tv_sec, now.tv_sec+15, 100, 100, 100, 100, 0, 0);
    
    // media.Load();
    // media.Display();

    // // media.Create();
    // // media.StopThread();
    
    // sleep(10);
    // cout << "fin" << endl;

}


