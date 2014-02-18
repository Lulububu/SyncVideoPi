// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef ILOCKER_H
#define ILOCKER_H

#include <time.h>	// for timespec
#include "../common/mpo_deleter.h"
#include "../common/datatypes.h"

class ILocker
{
public:
	virtual void Lock() = 0;
	virtual void Unlock() = 0;

	// must be locked before calling, returns true if event was received or false on timeout/error
	// 'u32Sec' and 'u32NanoSec' represent the current time as returned by the posix call 	clock_gettime(CLOCK_REALTIME, &...);
	virtual bool WaitForEvent(uint32_t u32Sec, uint32_t u32NanoSec) = 0;

	// must be locked before calling!
	virtual void GenerateEvent() = 0;
};

typedef std::shared_ptr<ILocker> ILockerSPtr;

#endif // ILOCKER_H
