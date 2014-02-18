// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef IVIDEOOBJECT_H
#define IVIDEOOBJECT_H

#include "IVideoObjectPublic.h"
#include "../../common/mpo_deleter.h"

class IVideoObject : public IVideoObjectPublic
{
public:

	virtual void RenderFrame(int, int, int, int, int, int) = 0;

	// makes back buffer the front buffer (this must be called after RenderFrame)
	virtual void Flip() = 0;
};

typedef std::shared_ptr<IVideoObject> IVideoObjectSPtr;

#endif // IVIDEOOBJECT_H
