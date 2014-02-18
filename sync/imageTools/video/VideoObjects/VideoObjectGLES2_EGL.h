// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef VIDEO_OBJECT_GLES2_EGL_H
#define VIDEO_OBJECT_GLES2_EGL_H


#include "VideoObjectGLES2.h"
#include "IVideoObjectEGLImage.h"

#include <GLES2/gl2.h>
#include <bcm_host.h>	// for dispmanx schlop
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglext.h>
#include "../../common/mpo_deleter.h"

class VideoObjectGLES2_EGL : public VideoObjectGLES2, public MpoDeleter, public IVideoObjectEGLImage
{
	// since bcm_init(?) needs to be called, we only allow PlatformRPI to instantiate us
	friend class PlatformRPI;

public:
	void Flip();

	IVideoObjectEGLImage *ToEGLImage() { return this; }
	void *CreateEGLImage(unsigned int uTextureWidth, unsigned int uTextureHeight);
	void DeleteEGLImage(void *);



private:

	VideoObjectGLES2_EGL(ILogger *pLogger);
	virtual ~VideoObjectGLES2_EGL();

	// only let PlatformRPI instantiate
	static IVideoObjectSPtr GetInstance(ILogger *pLogger);

	void DeleteInstance();

	bool InitPlatform();

	/////
	EGL_DISPMANX_WINDOW_T	m_nativewindow;
	
	// EGL variables
	EGLDisplay			m_eglDisplay;
	EGLConfig				m_eglConfig;
	EGLSurface			m_eglSurface;
	EGLContext			m_eglContext;

	

	bool m_bWaitForVsync;

};


#endif // VIDEO_OBJECT_GLES2_EGL_H
