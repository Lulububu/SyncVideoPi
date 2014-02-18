// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef EVENT_H
#define EVENT_H

#include "IEvent.h"

class OMXEvent : public IEvent
{
public:
	OMXEvent(OMXEventData ev);
	virtual ~OMXEvent() { }
	OMXEventData *ToEvent();
	EmptyBufferDoneData *ToEmpty();
	FillBufferDoneData *ToFill();

private:
	OMXEventData m_ev;
};

class EmptyBufferDoneEvent : public IEvent
{
public:
	EmptyBufferDoneEvent(EmptyBufferDoneData ev);
	virtual ~EmptyBufferDoneEvent() { }
	OMXEventData *ToEvent();
	EmptyBufferDoneData *ToEmpty();
	FillBufferDoneData *ToFill();

private:
	EmptyBufferDoneData m_ev;

};

class FillBufferDoneEvent : public IEvent
{
public:
	FillBufferDoneEvent(FillBufferDoneData ev);
	virtual ~FillBufferDoneEvent() { }
	OMXEventData *ToEvent();
	EmptyBufferDoneData *ToEmpty();
	FillBufferDoneData *ToFill();

private:
	FillBufferDoneData m_ev;

};

#endif // EVENT_H
