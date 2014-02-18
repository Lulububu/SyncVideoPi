// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef IVIDEOOBJECTEGLIMAGE_H
#define IVIDEOOBJECTEGLIMAGE_H

// used for rendering to texture

class IVideoObjectEGLImage
{
public:
	virtual void *CreateEGLImage(unsigned int uTextureWidth, unsigned int uTextureHeight) = 0;
	virtual void DeleteEGLImage(void *) = 0;
};

#endif
