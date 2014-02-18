// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef JPEGOPENMAX_H
#define JPEGOPENMAX_H

#include "IJPEGDecode.h"
#include "../openmax/OMXComponent.h"
#include "../io/IMemoryAligned.h"
#include "../io/logger.h"
#include "../video/VideoObjects/IVideoObjectEGLImage.h"

#include <vector>

using namespace std;

class JPEGOpenMax : public IJPEGDecode, public MpoDeleter
{
public:
	
	static IJPEGDecodeSPtr GetInstance(IVideoObjectEGLImage *pEGLImage, IOMXComponent *pCompDecode, IOMXComponent *pCompRender, IMemoryAligned *pMemoryAligned, ILogger *pLogger);
	
	void SetInputBufSizeHint(size_t stInputBufSizeBytes);

	bool DecompressJPEGStart(const uint8_t *p8SrcJpeg, size_t stSizeBytes);

	bool WaitJPEGDecompressorReady();
	
	int Function();

	// // width and height of image(s) we are decoding
	// unsigned int m_uWidth, m_uHeight;

private:
	void EmptyThisBuffer(OMX_BUFFERHEADERTYPE *pBufHeader);

	JPEGOpenMax(IVideoObjectEGLImage *pIEGLImage, IOMXComponent *pCompDecode, IOMXComponent *pCompRender, IMemoryAligned *pMemoryAligned, ILogger *pLogger);
	virtual ~JPEGOpenMax();

	void DeleteInstance() { delete this; }

	bool Init();

	void Shutdown();

	void OnDecoderOutputChanged();

	void OnDecoderOutputChangedAgain();

	IVideoObjectEGLImage *m_pIEGLImage;
	IOMXComponent *m_pCompDecode, *m_pCompRender;
	IMemoryAligned *m_pMemoryAligned;
	ILogger *m_pLogger;
	bool m_bInitialized;

	// openmax ports (in is for source jpeg, out is for decoded buffer)
	int m_iInPortDecode, m_iOutPortDecode;
	int m_iInPortRender, m_iOutPortRender;

	vector<OMX_BUFFERHEADERTYPE *> m_vpBufHeaders;	// vector to hold all of the buffer headers

	// the next index to use in the vector of source buffers when decoding a picture
	unsigned int m_uSrcBufVectorIndex;

	// whether we are currently waiting for a decode to finish
	bool m_bDecoding;

	int m_uWidth, m_uHeight;

	// pointer to struct containing info about output buffer
	OMX_BUFFERHEADERTYPE *m_pHeaderOutput;

	// allocated EGL Image
	void *m_eglImage;

	// maximum size of Jpeg that we will decode
	size_t m_stMaxJpegSizeBytes;
};

#endif // JPEGOPENMAX_H
