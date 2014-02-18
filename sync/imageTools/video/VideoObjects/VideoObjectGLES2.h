// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef VIDEO_OBJECT_GLES2
#define VIDEO_OBJECT_GLES2

#include "IVideoObject.h"
#include <list>
#include <GLES2/gl2.h>

using namespace std;

struct attrib_loc_s
{
	GLuint uIdx;
	const char *cpszName;
};

typedef list<attrib_loc_s> AttribLocList;
typedef list<GLuint> ShaderList;

class VideoObjectGLES2 : public IVideoObject
{
public:

	VideoType GetType() const;
	void RenderFrame(int wallWidth, int wallHeight, int tileWidth, int tileHeight, int tileX, int tileY);

protected:

	VideoObjectGLES2(ILogger *pLogger);
	~VideoObjectGLES2();

	typedef enum
	{
		TEX_RGBA,
		NUM_TEXTURES,
	} Texture;

	bool Init();

	void Shutdown();

	GLuint m_textures[NUM_TEXTURES];

	int m_imageWidth;
	int m_imageHeight;

	// raspberry pi variables
	// EGL_DISPMANX_WINDOW_T	m_nativewindow;

private:
	// these init methods all called from Init, don't call them directly
	void InitShaders();
	void InitSamplers();
	void InitBuffers();
	void InitBuffersHelper(GLfloat *pSrc, GLuint uSrcSizeBytes, GLuint *puBufID);
	void InitProjectionMatrices();
	void InitProjectionMatricesHelper(GLuint uProgram, GLfloat *pMatrix);
	void InitViewMatrices();
	void InitViewMatricesHelper(GLuint uProgram, GLfloat *pMatrix);
	void InitGLAttributes();

	// Creates/compiles one shader
	// throws exception on error
	GLuint CreateShader(GLenum shaderType, const char *cpszShaderSource);

	// Links a list of shaders into a program and returns the program ID on succcess
	// throws exception on error
	GLuint CreateProgram(const ShaderList &listShaders, const AttribLocList &listAttribLocs);

	// textures
	void InitTextures();

	// draw the RGBA frame
	void DrawRGBA();

	//////////////////////////////////////////////////////////////////////////////////////////////////

	VideoObjectCommon m_Common;

	GLuint m_uVertexShader, m_uFragmentShader;
	GLuint m_ProgramRGBA;
	GLuint m_uVertexBufferFullScreen;
	GLuint m_uTexCoordFlippedBuffer, m_uTexCoordBuffer;

};

//////////////////////////////////////////////////////////////////

#endif // VIDEO_OBJECT_GLES2
