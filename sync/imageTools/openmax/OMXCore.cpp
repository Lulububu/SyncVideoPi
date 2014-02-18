// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com


#include "OMXCore.h"
#include <stdexcept>

using namespace std;

IOMXCoreSPtr OMXCore::GetInstance(ILogger *pLogger, IClock *pClock)
{
	return IOMXCoreSPtr(new OMXCore(pLogger, pClock), OMXCore::deleter());
}

IOMXComponent *OMXCore::GetHandle(const char *cpszComponentName, ILocker *pLocker)
{
	OMX_HANDLETYPE hComponent = 0;

	// instantiate OMXComponent with handle value
	IOMXComponentSPtr comp = OMXComponent::GetInstance(m_pLogger, pLocker, m_pClock);
	OMXComponent *pComponent = (OMXComponent *) comp.get();	// need to access private SetHandle method

#ifndef WIN32
	OMX_ERRORTYPE err = OMX_GetHandle(&hComponent, (char *) cpszComponentName, pComponent, &m_callbacks);
	if (err != OMX_ErrorNone)
	{
		throw runtime_error("OMX_GetHandle failed");
	}
#endif

	// set handle on target component now that we have it
	pComponent->SetHandle(hComponent);

	m_mapComponents[hComponent] = comp;

	return pComponent;
}

OMXCore::OMXCore(ILogger *pLogger, IClock *pClock) :
m_pLogger(pLogger),
m_pClock(pClock)
{
	Init();
}

OMXCore::~OMXCore()
{
	Shutdown();
}

void OMXCore::Init()
{
	m_callbacks.EventHandler = OMXComponent::EventHandlerCallback;
	m_callbacks.EmptyBufferDone = OMXComponent::EmptyBufferDoneCallback;
	m_callbacks.FillBufferDone = OMXComponent::FillBufferDoneCallback;

#ifndef WIN32
	if (OMX_Init() != OMX_ErrorNone)
	{
		throw runtime_error("OMX_Init failed");
	}
#endif
}

void OMXCore::Shutdown()
{
#ifndef WIN32
	// free all handles
	for (map<OMX_HANDLETYPE, IOMXComponentSPtr>::iterator mi = m_mapComponents.begin();
		mi != m_mapComponents.end(); mi++)
	{
		OMX_FreeHandle(mi->first);
	}
	m_mapComponents.clear();

	if (OMX_Deinit() != OMX_ErrorNone)
	{
		throw runtime_error("OMX_Deinit failed");
	}
#endif
}

