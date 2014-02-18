// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com


#include "VideoObjectGLES2.h"
#include <string.h>	// for memcpy
#include <stdexcept>
#include <assert.h>
#include <stdio.h>

// It's important to keep these unique so that glEnableVertexAttribArray only needs to be called a minimal amount as it is an expensive call on the raspberry pi
#define MY_VERTEX_ARRAY 0
#define MY_TEXCOORD_ARRAY 1

// this can be replaced with whatever
#define GL_ASSERT(str) assert(glGetError() == GL_NO_ERROR)

IVideoObject::VideoType VideoObjectGLES2::GetType() const
{
	return OpenGLES2;
}

void VideoObjectGLES2::RenderFrame(int wallWidth, int wallHeight, int tileWidth, int tileHeight, int tileX, int tileY)
{
	printf("render frame %i %i\n", m_imageWidth ,m_imageHeight);
	float ratioX = (float)tileX/(float)wallWidth;
	float ratioY = (float)tileY/(float)wallHeight;

	float x = ratioX;
	float y = ratioY;
	
	ratioX = (float)(tileX+tileWidth)/(float)wallWidth;
	ratioY = (float)(tileY+tileHeight)/(float)wallHeight;

	printf("tileHeight %i wallHeight %i\n", tileHeight, wallHeight);

	printf("X %f, Y %f, X2 %f, Y2 %f\n", x, y, ratioX, ratioY);
	// printf("screen %i %i\n", m_nativewindow.width, m_nativewindow.height);
	// glViewport(x, y, width, height);
	// glViewport(x, y, 6000, height);
	// clear to a black frame

	// glTexSubImage2D
	GLfloat texCoordsFlipped[] =
	{
		x, ratioY,	// lower left
		ratioX, ratioY,	// lower right
		ratioX, y,	// upper right

		ratioX, y,	// upper right
		x, y,	// upper left
		x, ratioY,	// lower left
	};

	InitBuffersHelper(texCoordsFlipped, sizeof(texCoordsFlipped), &m_uTexCoordFlippedBuffer);


	glClear( GL_COLOR_BUFFER_BIT );
	
	// draw the RGBA texture
	DrawRGBA();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool VideoObjectGLES2::Init()
{
	try
	{
		InitGLAttributes();
		InitTextures();
		InitShaders();
		InitSamplers();
		InitBuffers();
		InitProjectionMatrices();
		InitViewMatrices();

		// enabling these is expensive (on raspberry pi) so just keep them always enabled
		glEnableVertexAttribArray(MY_VERTEX_ARRAY);
		GL_ASSERT("glEnableVertexAttribArray");

		glEnableVertexAttribArray(MY_TEXCOORD_ARRAY);
		GL_ASSERT("glEnableVertexAttribArray");

		m_Common.m_bInitialized = true;
	}
	catch (std::exception &ex)
	{
		string s = ex.what();
		m_Common.m_pLogger->Log(s);
	}

	return m_Common.m_bInitialized;
}

void VideoObjectGLES2::InitShaders()
{
	// create shaders/programs

	const char *cpszGenericVertexShader =
		"attribute vec4 myVertex;\n"
		"attribute vec2 inTexCoord;\n"
		"uniform mat4 matProjection;\n"
		"uniform mat4 matView;\n"
		"varying vec2 fragTexCoord;\n"
		"void main(void)\n"
		"{\n"
		"   gl_Position = matProjection * matView * myVertex;\n"
		"	fragTexCoord = inTexCoord;\n"
		"}";

	m_uVertexShader = CreateShader(GL_VERTEX_SHADER, cpszGenericVertexShader);

	const char *cpszStandardFragmentShader =
		"uniform sampler2D StandardTex;\n"
		"varying vec2 fragTexCoord;\n"
		"void main(void)\n"
		"{\n"
		"   gl_FragColor = texture2D(StandardTex,fragTexCoord);\n"
		"}";

	m_uFragmentShader = CreateShader(GL_FRAGMENT_SHADER, cpszStandardFragmentShader);

	ShaderList lstShadersRGBA;
	lstShadersRGBA.push_back(m_uVertexShader);
	lstShadersRGBA.push_back(m_uFragmentShader);

	attrib_loc_s loc;

	AttribLocList lstAttribLocs;
	loc.uIdx = MY_VERTEX_ARRAY;
	loc.cpszName = "myVertex";
	lstAttribLocs.push_back(loc);
	loc.uIdx = MY_TEXCOORD_ARRAY;
	loc.cpszName = "inTexCoord";
	lstAttribLocs.push_back(loc);

	m_ProgramRGBA = CreateProgram(lstShadersRGBA, lstAttribLocs);

}

void VideoObjectGLES2::InitSamplers()
{
	int i = 0;

	// now set up sampler(s)

	glUseProgram(m_ProgramRGBA);
	i=glGetUniformLocation(m_ProgramRGBA,"StandardTex");
	GL_ASSERT("glGetUniformLocation StandardTex");
	glUniform1i(i,0);	// texture unit 0 to correspond with "StandardTex" inside shader
	GL_ASSERT("glUniform1i");

}

void VideoObjectGLES2::InitBuffers()
{
	// texture coordinates using opengl convention (bottom to top)
	GLfloat texCoords[] =
	{
		0, 0,	// lower left
		1, 0,	// lower right
		1, 1,	// upper right

		1, 1,	// upper right
		0, 1,	// upper left
		0, 0,	// lower left
	};

	InitBuffersHelper(texCoords, sizeof(texCoords), &m_uTexCoordBuffer);

	////////////////////////////////////////

	// the vertical coordinate is flipped so that it matches my mental model of how a texture is laid in memory
	// (top to bottom)
	// GLfloat texCoordsFlipped[] =
	// {
	// 	0, 1,	// lower left
	// 	1, 1,	// lower right
	// 	1, 0,	// upper right

	// 	1, 0,	// upper right
	// 	0, 0,	// upper left
	// 	0, 1,	// lower left
	// };

	// InitBuffersHelper(texCoordsFlipped, sizeof(texCoordsFlipped), &m_uTexCoordFlippedBuffer);

	/////////////////////////////////////////

	GLfloat verticesFullScreen[] =
	{
		-1, -1,	// lower left
		1, -1,
		1, 1,

		1, 1,
		-1, 1,
		-1, -1,
	};

	InitBuffersHelper(verticesFullScreen, sizeof(verticesFullScreen), &m_uVertexBufferFullScreen);
}

void VideoObjectGLES2::InitBuffersHelper(GLfloat *pSrc, GLuint uSrcSizeBytes, GLuint *puBufID)
{
	// store vertices in a buffer for fast access
	glGenBuffers(1, puBufID);
	GL_ASSERT("glGenBuffersOverlay");

	glBindBuffer(GL_ARRAY_BUFFER, *puBufID);
	GL_ASSERT("glBindBufferOverlay");

	glBufferData(GL_ARRAY_BUFFER, uSrcSizeBytes, pSrc, GL_STATIC_DRAW);
	GL_ASSERT("glBufferDataOverlay");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VideoObjectGLES2::InitProjectionMatrices()
{
	// NOW setup projection matrices!

	// this is a 2D projection that changes nothing from the normalized device coordinates.
	// I used to have a near/far plane but decided to disable depth testing for speed improvement so there is no need for a projection matrix at all.
	// I am just leaving the code here so that others have a starting point if they want to use a projection matrix.

	// column major order
	GLfloat mProjection[16] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	InitProjectionMatricesHelper(m_ProgramRGBA, mProjection);
}

void VideoObjectGLES2::InitProjectionMatricesHelper(GLuint uProgram, GLfloat *pMatrix)
{
	glUseProgram(uProgram);

	int i = glGetUniformLocation(uProgram, "matProjection");
	GL_ASSERT("glGetUniformLocation matProjection");
	glUniformMatrix4fv(i,
		1,	// 1 matrix
		GL_FALSE,	// column major order
		pMatrix);
	GL_ASSERT("glUniformMatrix4fv (matProjection)");
}

void VideoObjectGLES2::InitViewMatrices()
{
	// NOW setup view matrices

	// "column major order" which means that the rows and columns are swapped.
	// fZ is actually in the 4th columm, and 2nd row (not the 3rd column , 4th row as it may appear)
	GLfloat mView[16] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	InitViewMatricesHelper(m_ProgramRGBA, mView);
}

void VideoObjectGLES2::InitViewMatricesHelper(GLuint uProgram, GLfloat *pMatrix)
{
	glUseProgram(uProgram);

	int i = glGetUniformLocation(uProgram, "matView");
	GL_ASSERT("glGetUniformLocation matView");
	glUniformMatrix4fv(i,
		1,	// 1 matrix
		GL_FALSE,	// column major order
		pMatrix);
	GL_ASSERT("glUniformMatrix4fv");
}

void VideoObjectGLES2::InitGLAttributes()
{
	// We do not use the depth buffer at all (for speed)
	glDisable(GL_DEPTH_TEST);

	// use culling for rendering speed
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);	// this should give us some optimization for free

	//glEnable(GL_SCISSOR_TEST);
	// glScissor(0,0,400,540);
	
	
	// disable blending (for speed)
	glDisable(GL_BLEND);
	
	// default blending options to uncomment if you want to use blending in the future
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// clear to black
	glClearColor( 0, 0, 0, 1 );

	// Setting the viewport on OMAP3 (Beagleboard) seems to not work for some reason
	// If we don't set this on Raspberry Pi, it gets the right res by default, so it seems better to not call glViewPort on the raspberry pi
}

void VideoObjectGLES2::Shutdown()
{
	// enabling these is expensive (on raspberry pi) so they are always enabled and now I am disabling them when shutting down
	glDisableVertexAttribArray(MY_VERTEX_ARRAY);
	GL_ASSERT("glEnableVertexAttribArray");

	glDisableVertexAttribArray(MY_TEXCOORD_ARRAY);
	GL_ASSERT("glEnableVertexAttribArray");

	// TODO : free shaders, buffers, etc
}

VideoObjectGLES2::VideoObjectGLES2(ILogger *pLogger)
{
	m_Common.m_pLogger = pLogger;
}

VideoObjectGLES2::~VideoObjectGLES2()
{
	// child class must call our Shutdown() to ensure it's done in the proper order
}

GLuint VideoObjectGLES2::CreateShader(GLenum shaderType, const char *cpszShaderSource)
{
	GLuint uRes = 0;
	char s[32768];
	GLint i = 0;
	GLsizei iCharsWritten = 0;

	uRes = glCreateShader(shaderType);

	const GLchar *sources[3] =
	{
    // Define GLSL version
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"
#else
    "#version 120\n"
#endif
    ,
    // GLES2 precision specifiers
#ifdef GL_ES_VERSION_2_0
    // Define default float precision for fragment shaders:
    (shaderType == GL_FRAGMENT_SHADER) ?
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "precision highp float;           \n"
    "#else                            \n"
    "precision mediump float;         \n"
    "#endif                           \n"
    : ""
    // Note: OpenGL ES automatically defines this:
    // #define GL_ES
#else
    // Ignore GLES 2 precision specifiers:
    "#define lowp   \n"
    "#define mediump\n"
    "#define highp  \n"
#endif
    ,
	cpszShaderSource };

	/* Compile the shader. */
	glShaderSource(uRes, 3, sources, NULL);
	glCompileShader(uRes);

	// see if compile succeeded
	glGetShaderiv(uRes,GL_COMPILE_STATUS,&i);
	if (i != GL_TRUE)
	{
		glGetShaderiv(uRes, GL_INFO_LOG_LENGTH, &i);
		glGetShaderInfoLog(uRes,i,&iCharsWritten,s);
		throw runtime_error((string) "OpenGL Shader Compile Failed: " + s);
	}

	return uRes;
}

GLuint VideoObjectGLES2::CreateProgram(const ShaderList &lstShaders, const AttribLocList &lstAttribLocs)
{
	char s[32768];
	GLint i = 0;
	GLsizei iCharsWritten = 0;

	// create the program
	GLuint uRes = glCreateProgram();

	for (ShaderList::const_iterator li = lstShaders.begin();
		li != lstShaders.end(); li++)
	{
		glAttachShader(uRes, *li);
	}

	// bind any attributes
	for (AttribLocList::const_iterator li1 = lstAttribLocs.begin();
		li1 != lstAttribLocs.end(); li1++)
	{
		glBindAttribLocation(uRes, li1->uIdx, li1->cpszName);
	}

	glLinkProgram(uRes);

	// see if link succeeded
	glGetProgramiv(uRes, GL_LINK_STATUS, &i);

	// if link succeeded
	if (i != GL_TRUE)
	{
		glGetProgramiv(uRes, GL_INFO_LOG_LENGTH, &i);
		glGetProgramInfoLog(uRes,i,&iCharsWritten,s);
		throw runtime_error((string) "OpenGL Shader Link Failed: " + s);
	}

	return uRes;
}

void VideoObjectGLES2::InitTextures()
{
	glGenTextures(NUM_TEXTURES, m_textures);
	GL_ASSERT("InitTextures");

	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		GL_ASSERT("InitYUVTextures");
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		GL_ASSERT("InitYUVTextures");
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		GL_ASSERT("InitYUVTextures");
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GL_ASSERT("InitYUVTextures");
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GL_ASSERT("InitYUVTextures");
	}
}

void VideoObjectGLES2::DrawRGBA()
{
	glUseProgram(m_ProgramRGBA);
	// setting active texture and binding the current texture only needs to be done once for this demo,
	//  however typically it needs to be done regularly, so I am leaving this code in here to that people can build on it if they want.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textures[TEX_RGBA]);
	GL_ASSERT("DrawRGBA");

	// indicate vertex buffer to use for rendering

	glBindBuffer(GL_ARRAY_BUFFER, m_uVertexBufferFullScreen);
	GL_ASSERT("glBindBuffer");
	// indicate that each vertex has 2 elements
	glVertexAttribPointer(MY_VERTEX_ARRAY, 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_ASSERT("glVertexAttribPointer");
	// indicate texture coordinate buffer to use for rendering
	// (note we used flipped texture coordinates because OpenMAX loads our texture this way)

	glBindBuffer(GL_ARRAY_BUFFER, m_uTexCoordFlippedBuffer);
	GL_ASSERT("glBindBuffer");

	// indivate the each texture coordinate has 2 elements
	glVertexAttribPointer(MY_TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_ASSERT("glVertexAttribPointer (texcoord)");

	// draw the 6 vertices that make up our full-screen rectangle
	glDrawArrays(GL_TRIANGLES, 0, 6);
	GL_ASSERT("glDrawArrays");
}

//////////////////////////////////////////////////////////////////

