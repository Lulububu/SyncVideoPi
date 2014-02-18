// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef VIDEO_OBJECT_COMMON_H
#define VIDEO_OBJECT_COMMON_H

#include "../../io/logger.h"
#include <list>

using namespace std;

class VideoObjectCommon
{
public:
	VideoObjectCommon();

	ILogger *m_pLogger;	// for logging
	bool m_bInitialized;
};

#endif // VIDEO_OBJECT_COMMON_H
