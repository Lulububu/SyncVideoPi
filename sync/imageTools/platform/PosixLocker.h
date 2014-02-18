// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef POSIXLOCKER_H
#define POSIXLOCKER_H

#include "../openmax/ILocker.h"
#include <pthread.h>

typedef std::shared_ptr<ILocker> ILockerSPtr;

class PosixLocker : public ILocker, public MpoDeleter
{
public:
        static ILockerSPtr GetInstance();
        void Lock();
        void Unlock();
        bool WaitForEvent(uint32_t u32Sec, uint32_t u32NanoSec);
        void GenerateEvent();

private:
        PosixLocker() : m_bInitialized(false) { Init(); }
	virtual ~PosixLocker() { Shutdown(); }

        void DeleteInstance() { delete this; }

        void Init();
        void Shutdown();

        bool m_bInitialized;
        pthread_mutex_t m_Mutex;
        pthread_cond_t m_Cond;
};

#endif // POSIXLOCKER_H

