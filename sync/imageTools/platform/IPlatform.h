// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef IPLATFORM_H
#define IPLATFORM_H

#include "../video/VideoObjects/IVideoObject.h"
#include "../io/logger.h"
#include "../common/mpo_deleter.h"
#include "../jpeg/IJPEGDecode.h"

class IPlatform
{
public:

	virtual IVideoObject *VideoInit() = 0;

	// so platform functions can use the generic logger
	virtual void SetLogger(ILogger *pLogger) = 0;

	// returns platform-specific implementation for JPEG decoding.
	virtual IJPEGDecode *GetJPEGDecoder() = 0;
};

typedef std::shared_ptr<IPlatform> IPlatformSPtr;

#endif // IPLATFORM_H
