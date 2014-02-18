// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef ICLOCK_H
#define ICLOCK_H

class IClock
{
public:
	// 'u32Sec' and 'u32NanoSec' represent the current time as returned by the posix call 	clock_gettime(CLOCK_REALTIME, &...);
	virtual void GetCurrent(uint32_t *pu32Sec, uint32_t *pu32NanoSec) = 0;
};

#endif // ICLOCK_H
