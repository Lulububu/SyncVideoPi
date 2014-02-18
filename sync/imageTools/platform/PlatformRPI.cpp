// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com



#include "../video/VideoObjects/VideoObjectGLES2_EGL.h"
#include <bcm_host.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include "PlatformRPI.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IPlatformSPtr PlatformRPI::GetInstance()
{
	IPlatformSPtr pRes;

	PlatformRPI *pInstance = new PlatformRPI();

	if (pInstance->Init())
	{
		pRes = IPlatformSPtr(pInstance, PlatformRPI::deleter());
	}
	else
	{
		delete pInstance;
	}

	return pRes;
}

IVideoObject *PlatformRPI::VideoInit()
{
	m_video = VideoObjectGLES2_EGL::GetInstance(m_pLogger);
	m_pVideo = m_video.get();
	return(m_pVideo);
}

void PlatformRPI::SetLogger(ILogger *pLogger)
{
	m_pLogger = pLogger;
}

IJPEGDecode *PlatformRPI::GetJPEGDecoder()
{
	// if openmax has not been initialized yet
	if (!m_pCore)
	{
		InitOMX();
	}

	// if we need to instantiate JPEG decoder
	// (this is deferred so we get an instance of logger and so we can get a video object)
	if (m_pJPEG == 0)
	{
		m_lockerDecode = PosixLocker::GetInstance();
		m_lockerRender = PosixLocker::GetInstance();
		m_pCompDecode = m_pCore->GetHandle("OMX.broadcom.image_decode", m_lockerDecode.get());
		m_pCompRender = m_pCore->GetHandle("OMX.broadcom.egl_render", m_lockerRender.get());
		m_jpeg = JPEGOpenMax::GetInstance(m_pVideo->ToEGLImage(), m_pCompDecode, m_pCompRender, this, m_pLogger);
		m_pJPEG = m_jpeg.get();
	}

	return m_pJPEG;
}

bool PlatformRPI::MyMalloc(void **memptr, size_t alignment, size_t size)
{
	return (posix_memalign(memptr, alignment, size) == 0);
}

void PlatformRPI::Free(void *pMem)
{
	free(pMem);
}

void PlatformRPI::GetCurrent(uint32_t *pu32Sec, uint32_t *pu32NanoSec)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	*pu32Sec = ts.tv_sec;
	*pu32NanoSec = ts.tv_nsec;
}

PlatformRPI::PlatformRPI() :
m_pLogger(NULL),
m_pVideo(NULL),
m_pJPEG(NULL),
m_pCompDecode(NULL),
m_pCompRender(NULL)
{
}

PlatformRPI::~PlatformRPI()
{
	// de-initialize components before de-initalizing core
	m_jpeg.reset();

	m_core.reset();

	bcm_host_deinit();
}

void PlatformRPI::DeleteInstance()
{
	delete this;
}

bool PlatformRPI::Init()
{
	bool bRes = false;

	bcm_host_init();

	bRes = true;
	return bRes;
}

void PlatformRPI::InitOMX()
{
	assert(m_pCore == NULL);
	assert(m_pLogger != NULL);

	m_core = OMXCore::GetInstance(m_pLogger, this);
	m_pCore = m_core.get();
}

///////////////////////////////////////
