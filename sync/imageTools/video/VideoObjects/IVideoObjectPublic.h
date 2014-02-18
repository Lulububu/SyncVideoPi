// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef IVIDEOOBJECTPUBLIC_H
#define IVIDEOOBJECTPUBLIC_H

#include "VideoObjectCommon.h"
#include "IVideoObjectEGLImage.h"

class IVideoObjectPublic
{
public:

	typedef enum
	{
		OpenGLES2
	} VideoType;

	// returns which type of video object this is.
	// This makes it so we don't have to do dynamic_cast which is costly and messy.
	virtual VideoType GetType() const = 0;

	// cast this class to any one of the following interfaces.
	// If class implements interface, it will return 'this' otherwise it will return NULL.
	virtual IVideoObjectEGLImage *ToEGLImage() = 0;

};

#endif // IVIDEOOBJECTPUBLIC_H
