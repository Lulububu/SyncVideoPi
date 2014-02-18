// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#include "PosixLocker.h"
#include <stdexcept>

using namespace std;

ILockerSPtr PosixLocker::GetInstance()
{
	ILockerSPtr pRes;
	PosixLocker *pInstance = 0;

	try
	{
		pInstance = new PosixLocker();
		pInstance->Init();
		pRes = ILockerSPtr(pInstance, PosixLocker::deleter());
	}
	catch (std::exception &)
	{
		if (pInstance)
		{
			delete pInstance;
		}
	}

	return pRes;
}

void PosixLocker::Lock()
{
	if (pthread_mutex_lock(&m_Mutex) != 0)
	{
		throw runtime_error("Mutex lock failed");
	}
}

void PosixLocker::Unlock()
{
	if (pthread_mutex_unlock(&m_Mutex) != 0)
	{
		throw runtime_error("Mutex unlock failed");
	}
}

// this method can't throw an exception because  we are locked
bool PosixLocker::WaitForEvent(uint32_t u32Sec, uint32_t u32NanoSec)
{
	struct timespec ptsEnd;
	bool bRes = false;

	ptsEnd.tv_sec = (time_t) u32Sec;
	ptsEnd.tv_nsec = (long) u32NanoSec;

	int iRes = pthread_cond_timedwait(&m_Cond, &m_Mutex, &ptsEnd);

	if (iRes == 0)
	{
		bRes = true;
	}

	return bRes;
}

// this method can't throw an exception because  we are locked
void PosixLocker::GenerateEvent()
{
	// assumes we are locked!
	pthread_cond_broadcast(&m_Cond);
}

void PosixLocker::Init()
{
	if (pthread_mutex_init(&m_Mutex, 0) != 0)
	{
		throw runtime_error("Mutex init failed");
	}

	if (pthread_cond_init(&m_Cond, NULL) != 0)
	{
		throw runtime_error("Condition init failed");
	}

	m_bInitialized = true;
}

void PosixLocker::Shutdown()
{
	if (m_bInitialized)
	{
		pthread_mutex_destroy(&m_Mutex);
		pthread_cond_destroy(&m_Cond);
	}
}
