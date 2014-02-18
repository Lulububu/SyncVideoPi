// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef PLATFORMRPI_H
#define PLATFORMRPI_H


#include "IPlatform.h"
#include "../io/IMemoryAligned.h"
#include "../openmax/IClock.h"
#include "../openmax/OMXCore.h"
#include "../jpeg/JPEGOpenMax.h"
#include "PosixLocker.h"
#include <list>
using namespace std;

class PlatformRPI : public MpoDeleter, public IPlatform, public IClock, public IMemoryAligned
{
public:
	static IPlatformSPtr GetInstance();

	IVideoObject *VideoInit();

	void SetLogger(ILogger *pLogger);

	IJPEGDecode *GetJPEGDecoder();

	/////////////
	// IMemoryAligned methods
	bool MyMalloc(void **memptr, size_t alignment, size_t size);

	void Free(void *pMem);

	////////////
	// IClock method
	void GetCurrent(uint32_t *pu32Sec, uint32_t *pu32NanoSec);

private:
	PlatformRPI();
	virtual ~PlatformRPI();

	void DeleteInstance();

	bool Init();

	// this must be deferred since it requires an instantiated logger
	void InitOMX();

	////////////////////////////////////////

	ILogger *m_pLogger;

	IVideoObjectSPtr m_video;
	IVideoObject *m_pVideo;

	IJPEGDecodeSPtr m_jpeg;
	IJPEGDecode *m_pJPEG;

	IOMXCoreSPtr m_core;
	IOMXCore *m_pCore;
	IOMXComponent *m_pCompDecode, *m_pCompRender;

	ILockerSPtr m_lockerDecode, m_lockerRender;
};

#endif // PLATFORMRPI_H
