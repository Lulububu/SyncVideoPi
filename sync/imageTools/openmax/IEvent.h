// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef IEVENT_H
#define IEVENT_H

#include <IL/OMX_Component.h>
#include "../common/mpo_deleter.h"

struct OMXEventData
{
	OMX_EVENTTYPE eEvent;
	OMX_U32 nData1;
	OMX_U32 nData2;
	OMX_PTR pEventData;
};

struct EmptyBufferDoneData
{
	const OMX_BUFFERHEADERTYPE* pBuffer;
};

struct FillBufferDoneData
{
	const OMX_BUFFERHEADERTYPE* pBuffer;
};

class IEvent
{
public:
	// These three methods help caller determine which type the event is (alternative to using dynamic_cast)
	// Two of the methods must return NULL for any instantiation.
	virtual OMXEventData *ToEvent() = 0;
	virtual EmptyBufferDoneData *ToEmpty() = 0;
	virtual FillBufferDoneData *ToFill() = 0;
};

typedef std::shared_ptr<IEvent> IEventSPtr;

#endif // IEVENT_H
