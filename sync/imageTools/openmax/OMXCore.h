// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef OMXCORE_H
#define OMXCORE_H

#include <IL/OMX_Component.h>	// khronos header file that includes everything else
#include "OMXComponent.h"
#include "../io/logger.h"
#include "../common/mpo_deleter.h"
#include <map>

using namespace std;

class IOMXCore
{
public:
	virtual IOMXComponent *GetHandle(const char *cpszComponentName, ILocker *) = 0;
};

typedef std::shared_ptr<IOMXCore> IOMXCoreSPtr;

class OMXCore : public IOMXCore, public MpoDeleter
{
public:
	static IOMXCoreSPtr GetInstance(ILogger *pLogger, IClock *);

	IOMXComponent *GetHandle(const char *cpszComponentName, ILocker *);

private:
	OMXCore(ILogger *pLogger, IClock *);
	virtual ~OMXCore();

	void DeleteInstance() { delete this; }

	void Init();
	void Shutdown();

//////////////////////////////////////////////////////////////////

	OMX_CALLBACKTYPE m_callbacks;
	map<OMX_HANDLETYPE, IOMXComponentSPtr> m_mapComponents;
	ILogger *m_pLogger;
	IClock *m_pClock;
};

#endif // OMXCORE_H
