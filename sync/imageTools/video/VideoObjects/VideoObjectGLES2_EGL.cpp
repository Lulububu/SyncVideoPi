// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com



// This code is written for the raspberry pi

#include <stdio.h>
#include <stdexcept>
#include <assert.h>
#include "VideoObjectGLES2_EGL.h"

#define EGL_ASSERT() { EGLint i = eglGetError(); if (i != EGL_SUCCESS) { fprintf(stderr, "Error code 0x%x\n", i); assert(false); } }

////////////////////////////////////////////////////////////////

bool VideoObjectGLES2_EGL::InitPlatform()
{
	bool bRes = false;
	EGLBoolean result;

	try
	{
		m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		assert(m_eglDisplay != EGL_NO_DISPLAY);
		EGL_ASSERT();

		if (!eglInitialize(m_eglDisplay, NULL, NULL))
		{
			throw runtime_error("Error: eglInitialize() failed.");
		}
		EGL_ASSERT();

		static const EGLint pi32ConfigAttribs[] =
		{
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_NONE
		};

		EGLint num_config;
		if (!eglChooseConfig(m_eglDisplay, pi32ConfigAttribs, &m_eglConfig, 1, &num_config))
		{
			throw runtime_error("Error: eglChooseConfig() failed.");
		}
		EGL_ASSERT();

		// get an appropriate EGL frame buffer configuration
		result = eglBindAPI(EGL_OPENGL_ES_API);
		assert(EGL_FALSE != result);
		EGL_ASSERT();

		static const EGLint context_attributes[] = 
		{
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
		};

		m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, context_attributes);
		assert(m_eglContext != EGL_NO_CONTEXT);
		EGL_ASSERT();

		uint32_t uScreenWidth, uScreenHeight;
		int32_t iSuccess = graphics_get_display_size(0, &uScreenWidth, &uScreenHeight);

		if (iSuccess < 0)
		{
			throw runtime_error("Error, unable to determine screen size");
		}

		VC_RECT_T dst_rect;
		VC_RECT_T src_rect;
		printf("w %i, h %i \n",uScreenWidth, uScreenHeight);
		dst_rect.x = 0;
		dst_rect.y = 0;
		dst_rect.width = uScreenWidth;
		dst_rect.height = uScreenHeight;

		src_rect.x = 0;
		src_rect.y = 0;
		src_rect.width = uScreenWidth << 16;
		src_rect.height = uScreenHeight << 16;

		DISPMANX_DISPLAY_HANDLE_T dispman_display;
		DISPMANX_UPDATE_HANDLE_T dispman_update;
		DISPMANX_ELEMENT_HANDLE_T dispman_element;

		dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
		dispman_update = vc_dispmanx_update_start( 0 );

		dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
			0/*layer*/, &dst_rect, 0/*src*/,
			&src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/,
			(DISPMANX_TRANSFORM_T) 0/*transform*/);

		m_nativewindow.element = dispman_element;
		m_nativewindow.width = uScreenWidth;
		m_nativewindow.height = uScreenHeight;
		vc_dispmanx_update_submit_sync( dispman_update );

		EGL_ASSERT();

		m_eglSurface = eglCreateWindowSurface( m_eglDisplay, m_eglConfig, &m_nativewindow, NULL );
		assert(m_eglSurface != EGL_NO_SURFACE);
		EGL_ASSERT();

		if (eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) == EGL_FALSE)
		{
			throw runtime_error("eglMakeCurrent failed");
		}
		EGL_ASSERT();

		// enable (or disable) wait for vsync
		EGLint interval = 1;
		if (!m_bWaitForVsync)
		{
			interval = 0;
		}

		if (eglSwapInterval(m_eglDisplay, interval) != EGL_TRUE)
		{
			throw runtime_error("eglSwapInterval failed");
		}

		// common init here
		bRes = Init();
	}
	catch (std::exception &)
	{
		// TODO log
		bRes = false;
	}

	return bRes;
}

void VideoObjectGLES2_EGL::Flip()
{
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
	EGL_ASSERT();
}

void *VideoObjectGLES2_EGL::CreateEGLImage(unsigned int uTextureWidth, unsigned int uTextureHeight)
{
	void *pRes = 0;
	GLuint uTexID = 0;

	// map field index to texture ID
	uTexID = m_textures[TEX_RGBA];
	
	glBindTexture(GL_TEXTURE_2D, uTexID);
	
	/*	
	int xoffset = 100;
	int yoffset = 100;
	int subWidth = uTextureWidth-xoffset;
	int subHeight = uTextureHeight-yoffset;
	
	glPixelStorei(0x0CF2, uTextureWidth);
	glPixelStorei(0x0CF4, xoffset);
	glPixelStorei(0x0CF3, yoffset);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	//glTexImage2D(GL_TEXTURE_2D, GL_RGB, 0, subimg.width, subimg.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata)
	// glReadPixels
	// This call is necessary or else the eglCreateImage fails
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, subWidth, subHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, subWidth, subHeight, GL_RGBA, GL_UNSIGNED_BYTE, (EGLClientBuffer) uTexID);
		
	glPixelStorei(0x0CF2, 0);
	glPixelStorei(0x0CF4, 0);
	glPixelStorei(0x0CF3, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
	
	*/

	// float ratioX = 
	m_imageWidth = uTextureWidth;
	m_imageHeight = uTextureHeight;
	printf("ttttt %i\n", uTextureWidth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, uTextureWidth, uTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	printf("texH : %i, textW : %i \n", uTextureWidth, uTextureHeight);
	
	// make egl image
	pRes = eglCreateImageKHR(
              		m_eglDisplay,
               	m_eglContext,
               	EGL_GL_TEXTURE_2D_KHR,
               	(EGLClientBuffer) uTexID,
		0);
	
	if (pRes == EGL_NO_IMAGE_KHR)
	{
		throw runtime_error("eglCreateImageKHR failed");
	}
	
	return pRes;
}

void VideoObjectGLES2_EGL::DeleteEGLImage(void *eglImage)
{
	
	if (eglDestroyImageKHR(m_eglDisplay, eglImage) != EGL_TRUE)
	{
		throw runtime_error("eglDestroyImageKHR failed");
	}
	
}

//////////////////////////////////////////////

VideoObjectGLES2_EGL::VideoObjectGLES2_EGL(ILogger *pLogger) :
VideoObjectGLES2(pLogger)
{
	// EGL variables
	m_eglDisplay	= 0;
	m_eglConfig	= 0;
	m_eglSurface	= 0;
	m_eglContext	= 0;

	// vsync off for benchmarking purposes
	m_bWaitForVsync = false;
}

IVideoObjectSPtr VideoObjectGLES2_EGL::GetInstance(ILogger *pLogger)
{
	IVideoObjectSPtr pRes;
	VideoObjectGLES2_EGL *pInstance = new VideoObjectGLES2_EGL(pLogger);

	if (pInstance)
	{
		if (!pInstance->InitPlatform())
		{
			delete pInstance;
			pInstance = NULL;
		}
		else
		{
			pRes = IVideoObjectSPtr(pInstance, VideoObjectGLES2_EGL::deleter());
		}
	}

	return pRes;
}

VideoObjectGLES2_EGL::~VideoObjectGLES2_EGL()
{
	Shutdown();
	
	eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
	eglDestroySurface( m_eglDisplay, m_eglSurface );
	eglDestroyContext( m_eglDisplay, m_eglContext );
	eglTerminate(m_eglDisplay);
}

void VideoObjectGLES2_EGL::DeleteInstance()
{
	delete this;
}
