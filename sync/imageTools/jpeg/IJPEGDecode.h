// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef IJPEGDECODE_H
#define IJPEGDECODE_H

#include "../common/datatypes.h"
#include "../common/mpo_deleter.h"

class IJPEGDecode
{
public:
	
	virtual int Function() = 0;
	
	// tells the JPEG decoder what the maximum expected JPEG size will be that will be past to DecompressJPEGStart.
	virtual void SetInputBufSizeHint(size_t stInputBufSizeBytes) = 0;

	// starts decompressing JPEG in the background.
	virtual bool DecompressJPEGStart(const uint8_t *p8SrcJpeg, size_t stSizeBytes) = 0;

	// blocks until background JPEG decompression is finished.
	virtual bool WaitJPEGDecompressorReady() = 0;

	int m_uWidth, m_uHeight;
};

typedef std::shared_ptr<IJPEGDecode> IJPEGDecodeSPtr;

#endif // IJPEGDECODE_H
