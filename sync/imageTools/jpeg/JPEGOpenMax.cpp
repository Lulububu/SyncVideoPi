// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com


#include "JPEGOpenMax.h"
#include "../common/common.h"
#include <string.h>
#include <stdexcept>
#include <assert.h>

// arbitrary timeout value which is subject to change
#define TIMEOUT_MS 2000

IJPEGDecodeSPtr JPEGOpenMax::GetInstance(IVideoObjectEGLImage *pEGLImage, IOMXComponent *pCompDecode, IOMXComponent *pCompRender, IMemoryAligned *pMemoryAligned, ILogger *pLogger)
{
	IJPEGDecodeSPtr pRes;
	JPEGOpenMax *pInstance = new JPEGOpenMax(pEGLImage, pCompDecode, pCompRender, pMemoryAligned, pLogger);

	if (pInstance->Init())
	{
		pRes = IJPEGDecodeSPtr(pInstance,JPEGOpenMax::deleter());
	}
	else
	{
		delete pInstance;
	}

	return pRes;
}

int JPEGOpenMax::Function()
{
	return 1;
}

void JPEGOpenMax::SetInputBufSizeHint(size_t stInputBufSizeBytes)
{
	m_stMaxJpegSizeBytes = stInputBufSizeBytes;

	// query input buffer requirements
	OMX_PARAM_PORTDEFINITIONTYPE portdef;
	portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	portdef.nVersion.nVersion = OMX_VERSION;
	portdef.nPortIndex = m_iInPortDecode;
	m_pCompDecode->GetParameter(OMX_IndexParamPortDefinition, &portdef);

	// change input buffer size if our size is greater than what is already there
	if (stInputBufSizeBytes > portdef.nBufferSize)
	{
		portdef.nBufferSize = stInputBufSizeBytes;
		m_pCompDecode->SetParameter(OMX_IndexParamPortDefinition, &portdef);

		// again query parms to see what our actual buffer size is
		m_pCompDecode->GetParameter(OMX_IndexParamPortDefinition, &portdef);

		assert(portdef.nBufferSize >= stInputBufSizeBytes);
	}

	// enable input port
	m_pCompDecode->SendCommand(OMX_CommandPortEnable, m_iInPortDecode, NULL);

	int iBufferCount = portdef.nBufferCountActual;
	vector<OMX_BUFFERHEADERTYPE *> vpBufHeaders;	// vector to hold all of the buffer headers
	
	for (int i = 0; i < iBufferCount; i++)
	{
		void *pBuf = 0;

		if (!m_pMemoryAligned->MyMalloc(&pBuf, portdef.nBufferAlignment, portdef.nBufferSize))
		{
			throw runtime_error("posix_memalign failed");
		}

		OMX_BUFFERHEADERTYPE *pHeader = NULL;

		m_pCompDecode->UseBuffer(&pHeader, m_iInPortDecode,
			(void *) i,	// the index will be our private data
			portdef.nBufferSize, (OMX_U8 *) pBuf);

		m_vpBufHeaders.push_back(pHeader);	// add buffer header to our vector (the vector index will match 'i')
	}

	// wait for port enable event to be finished (it should finish once we give it buffers)
	m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortEnable, m_iInPortDecode, TIMEOUT_MS);

	// move component into executing state so it can begin consuming our buffers
	m_pCompDecode->SendCommand(OMX_CommandStateSet, OMX_StateExecuting, NULL);
	m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateExecuting, TIMEOUT_MS);

	m_bInitialized = true;
}

bool JPEGOpenMax::DecompressJPEGStart(const uint8_t *p8SrcJpeg, size_t stSizeBytes)
{
	bool bRes = false;

	try
	{
		// get buffer to fill
		OMX_BUFFERHEADERTYPE *pBufHeader = m_vpBufHeaders[m_uSrcBufVectorIndex];
		m_uSrcBufVectorIndex++;

		// wraparound if necessary
		if (m_uSrcBufVectorIndex >= m_vpBufHeaders.size())
		{
			m_uSrcBufVectorIndex = 0;
		}

		// copy in full jpeg to destination buffer
		memcpy(pBufHeader->pBuffer, p8SrcJpeg, stSizeBytes);

		pBufHeader->nFilledLen = stSizeBytes;
		pBufHeader->nOffset = 0;
		pBufHeader->nFlags = OMX_BUFFERFLAG_EOS;	// notify that we've copied in the entire jpeg

		// start decoding buffer data
		this->EmptyThisBuffer(pBufHeader);

		assert(m_pHeaderOutput != 0);

		// tell openmax to fill destination (in this case a GLES2 texture)
		m_pCompRender->FillThisBuffer(m_pHeaderOutput);

		m_bDecoding = true;

		bRes = true;
	}
	catch (std::exception &ex)
	{
		string s = "JPEGOpenMax::DecompressJPEGStart exception: ";
		s += ex.what();
		m_pLogger->Log(s);
	}

	return bRes;
}

void JPEGOpenMax::EmptyThisBuffer(OMX_BUFFERHEADERTYPE *pBufHeader)
{
	m_pCompDecode->EmptyThisBuffer(pBufHeader);
	bool bGotEmpty = false;
	bool bNeedToSetupRenderer = false;

	// if we don't have an output buffer header, then it means we have not set up the renderer yet
	if (m_pHeaderOutput == NULL)
	{
		bNeedToSetupRenderer = true;
	}
	// prepare to change the texture that we render to
	// (this causes only a minimal performance impact and it is nice to have the flexibility)
	else
	{
		// disable output renderer port
		m_pCompRender->SendCommand(OMX_CommandPortDisable, m_iOutPortRender, NULL);

		// free previous EGL buffer
		m_pCompRender->FreeBuffer(m_iOutPortRender, m_pHeaderOutput);

		// wait for disable to finish
		m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iOutPortRender, TIMEOUT_MS);
	}

	// we need to get an empty ack after calling EmptyThisBuffer,
	//   the only thing that can stand in our way is getting a port settings changed event.
	// We expect and need a port settings changed event if we have no output buffer.
	while ((!bGotEmpty) || bNeedToSetupRenderer)
	{
		// we could get either a "empty done" or a "port settings changed" at this point
		IEventSPtr ev = m_pCompDecode->WaitForEventOrEmpty(OMX_EventPortSettingsChanged, m_iOutPortDecode, 0, pBufHeader, TIMEOUT_MS);

		IEvent *pEv = ev.get();
		OMXEventData *pEvOMX = pEv->ToEvent();
		EmptyBufferDoneData *pEvEmpty = pEv->ToEmpty();

		// if this is an empty event
		if (pEvEmpty != NULL)
		{
			bGotEmpty = true;
		}
		// else if it's a port settings changed event
		else if (pEvOMX != NULL)
		{
			// we don't expect to get a "port settings changed" event after we've already set up the renderer
			assert(m_pHeaderOutput == 0);

			OnDecoderOutputChanged();	// setup output buffer
			bNeedToSetupRenderer = false;
		}
		// else this is unexpected
		else
		{
			throw runtime_error("Unexpected IEvent");
		}
	}

	// assign new texture to render to

	// enable output port of Renderer
	m_pCompRender->SendCommand(OMX_CommandPortEnable, m_iOutPortRender, NULL);

	// tell renderer to use EGL texture
	m_pCompRender->UseEGLImage(&m_pHeaderOutput, m_iOutPortRender, NULL, m_eglImage);

	// wait for output port enable event to be finished (it should finish once we call UseEGLImage)
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortEnable, m_iOutPortRender, TIMEOUT_MS);
}

bool JPEGOpenMax::WaitJPEGDecompressorReady()
{
	bool bRes = false;

	try
	{
		if (!m_bDecoding)
		{
			throw runtime_error("Not decoding");
		}

		// when Fill finishes, it means that JPEG is fully decoded
		m_pCompRender->WaitForFill(m_pHeaderOutput, TIMEOUT_MS);

		// wait for "end of stream" events from decoder and renderer
		m_pCompDecode->WaitForEvent(OMX_EventBufferFlag, m_iOutPortDecode, OMX_BUFFERFLAG_EOS, TIMEOUT_MS);
		m_pCompRender->WaitForEvent(OMX_EventBufferFlag, m_iOutPortRender, OMX_BUFFERFLAG_ENDOFFRAME | OMX_BUFFERFLAG_EOS, TIMEOUT_MS);

		// at this point, we should have no events queued up at all; if we do, we have probably missed one somewhere
		assert(m_pCompDecode->GetPendingEventCount() == 0);
		assert(m_pCompDecode->GetPendingEmptyCount() == 0);
		assert(m_pCompDecode->GetPendingFillCount() == 0);
		assert(m_pCompRender->GetPendingEventCount() == 0);
		assert(m_pCompRender->GetPendingEmptyCount() == 0);
		assert(m_pCompRender->GetPendingFillCount() == 0);

		m_bDecoding = false;
		bRes = true;
	}
	catch (std::exception &ex)
	{
		m_pLogger->Log((string) "JPEGOpenMax::WaitJPEGDecompressorReady exception: " + ex.what());
	}

	return bRes;
}

JPEGOpenMax::JPEGOpenMax(IVideoObjectEGLImage *pIEGLImage, IOMXComponent *pCompDecode, IOMXComponent *pCompRender, IMemoryAligned *pMemoryAligned, ILogger *pLogger) :
m_pIEGLImage(pIEGLImage),
m_pCompDecode(pCompDecode),
m_pCompRender(pCompRender),
m_pMemoryAligned(pMemoryAligned),
m_pLogger(pLogger),
m_bInitialized(false),
m_iInPortDecode(0),
m_iOutPortDecode(0),
m_iInPortRender(0),
m_iOutPortRender(0),
m_uSrcBufVectorIndex(0),
m_bDecoding(false),
m_uWidth(0),
m_uHeight(0),
m_pHeaderOutput(NULL),
m_stMaxJpegSizeBytes(0)
{
	m_eglImage = 0;
}

JPEGOpenMax::~JPEGOpenMax()
{
	Shutdown();
}

bool JPEGOpenMax::Init()
{
	bool bRes = false;

	try
	{
		OMX_PORT_PARAM_TYPE port;
		port.nSize = sizeof(OMX_PORT_PARAM_TYPE);
		port.nVersion.nVersion = OMX_VERSION;

		// get ports for decoder
		m_pCompDecode->GetParameter(OMX_IndexParamImageInit, &port);

		if (port.nPorts != 2)
		{
			throw runtime_error("Unexpected number of ports returned");
		}

		m_iInPortDecode = port.nStartPortNumber;
		m_iOutPortDecode = port.nStartPortNumber+1;

		// get ports for renderer
		m_pCompRender->GetParameter(OMX_IndexParamVideoInit, &port);

		if (port.nPorts != 2)
		{
			throw runtime_error("Unexpected number of ports returned");
		}

		m_iInPortRender = port.nStartPortNumber;
		m_iOutPortRender = port.nStartPortNumber+1;

		// disable all ports to get to a sane state
		m_pCompDecode->SendCommand(OMX_CommandPortDisable, m_iInPortDecode, NULL);
		m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iInPortDecode, TIMEOUT_MS);
		m_pCompDecode->SendCommand(OMX_CommandPortDisable, m_iOutPortDecode, NULL);
		m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iOutPortDecode, TIMEOUT_MS);
		m_pCompRender->SendCommand(OMX_CommandPortDisable, m_iInPortRender, NULL);
		m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iInPortRender, TIMEOUT_MS);
		m_pCompRender->SendCommand(OMX_CommandPortDisable, m_iOutPortRender, NULL);
		m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iOutPortRender, TIMEOUT_MS);

		// move decoder component into idle state
		m_pCompDecode->SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);

		// wait for state change event
		m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle, TIMEOUT_MS);

		// set input format
		OMX_IMAGE_PARAM_PORTFORMATTYPE imagePortFormat;
		memset(&imagePortFormat, 0, sizeof(imagePortFormat));
		imagePortFormat.nSize = sizeof(imagePortFormat);
		imagePortFormat.nVersion.nVersion = OMX_VERSION;
		imagePortFormat.nPortIndex = m_iInPortDecode;
		imagePortFormat.eCompressionFormat = OMX_IMAGE_CodingJPEG;
		m_pCompDecode->SetParameter(OMX_IndexParamImagePortFormat, &imagePortFormat);

		// initialization will not be finished until SetInputBufSizeHint is called

		bRes = true;
	}
	catch (std::exception &ex)
	{
		string s = "JPEGOpenMAX Init exception: ";
		s += ex.what();
		m_pLogger->Log(s);
	}

	return bRes;
}

void JPEGOpenMax::Shutdown()
{
	if (!m_bInitialized)
	{
		return;
	}

	// flush tunnel
	m_pCompDecode->SendCommand(OMX_CommandFlush, m_iOutPortDecode, NULL);
	m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandFlush, m_iOutPortDecode, TIMEOUT_MS);
	m_pCompRender->SendCommand(OMX_CommandFlush, m_iInPortRender, NULL);
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandFlush, m_iInPortRender, TIMEOUT_MS);

	// disable input decoder port
	m_pCompDecode->SendCommand(OMX_CommandPortDisable, m_iInPortDecode, NULL);

	// OMX_FreeBuffer on all input buffers
	for (vector<OMX_BUFFERHEADERTYPE *>::iterator vi = m_vpBufHeaders.begin();
		vi != m_vpBufHeaders.end(); vi++)
	{
		void *pBuffer = (*vi)->pBuffer;
		m_pCompDecode->FreeBuffer(m_iInPortDecode, *vi);
		m_pMemoryAligned->Free(pBuffer);
	}

	// wait for disable to finish
	m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iInPortDecode, TIMEOUT_MS);

	// disable output renderer port
	m_pCompRender->SendCommand(OMX_CommandPortDisable, m_iOutPortRender, NULL);

	// free EGL buffer
	m_pCompRender->FreeBuffer(m_iOutPortRender, m_pHeaderOutput);

	// wait for disable to finish
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iOutPortRender, TIMEOUT_MS);

	// disable the rest of the ports
	m_pCompDecode->SendCommand(OMX_CommandPortDisable, m_iOutPortDecode, NULL);
	m_pCompRender->SendCommand(OMX_CommandPortDisable, m_iInPortRender, NULL);
	m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iOutPortDecode, TIMEOUT_MS);
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortDisable, m_iInPortRender, TIMEOUT_MS);

	// OMX_SetupTunnel with 0's to remove tunnel
	m_pCompDecode->RemoveTunnel(m_iOutPortDecode);
	m_pCompRender->RemoveTunnel(m_iInPortRender);

	// change handle states to IDLE
	m_pCompDecode->SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);
	m_pCompRender->SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);

	// wait for state change complete
	m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle, TIMEOUT_MS);
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle, TIMEOUT_MS);

	// change handle states to LOADED
	m_pCompDecode->SendCommand(OMX_CommandStateSet, OMX_StateLoaded, NULL);
	m_pCompRender->SendCommand(OMX_CommandStateSet, OMX_StateLoaded, NULL);

	// wait for state change complete
	m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateLoaded, TIMEOUT_MS);
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateLoaded, TIMEOUT_MS);

	// free EGL image
	if (m_eglImage != 0)
	{
		m_pIEGLImage->DeleteEGLImage(m_eglImage);
	}
}

void JPEGOpenMax::OnDecoderOutputChanged()
{
	OMX_PARAM_PORTDEFINITIONTYPE portdef;

	// establish tunnel between decoder output and renderer input
	// (this will automatically set up the renderer's input port)
	m_pCompDecode->SetupTunnel(m_iOutPortDecode, m_pCompRender, m_iInPortRender);

	// enable output of decoder and input of Render (ie enable tunnel)
	m_pCompDecode->SendCommand(OMX_CommandPortEnable, m_iOutPortDecode, NULL);
	m_pCompRender->SendCommand(OMX_CommandPortEnable, m_iInPortRender, NULL);

	// put renderer in idle state (this allows the outport of the decoder to become enabled)
	m_pCompRender->SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);

	// wait for state change complete
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle, TIMEOUT_MS);

	// once the state changes, both ports should become enabled and the renderer output should generate a settings changed event
	m_pCompDecode->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortEnable, m_iOutPortDecode, TIMEOUT_MS);
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandPortEnable, m_iInPortRender, TIMEOUT_MS);
	m_pCompRender->WaitForEvent(OMX_EventPortSettingsChanged, m_iOutPortRender, 0, TIMEOUT_MS);

	// NOTE : OpenMAX official spec says that upon receving OMX_EventPortSettingsChanged event, the
	//   port shall be disabled and then re-enabled (see 3.1.1.4.4 of IL v1.2.0 specification),
	//   but since we have not enabled the port, I don't think we need to do anything.

	// move renderer into executing state
	m_pCompRender->SendCommand(OMX_CommandStateSet, OMX_StateExecuting, NULL);
	m_pCompRender->WaitForEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateExecuting, TIMEOUT_MS);

	// for some reason, we get a Port Settings Changed event again when we do this
	m_pCompRender->WaitForEvent(OMX_EventPortSettingsChanged, m_iOutPortRender, 0, TIMEOUT_MS);

	// query output buffer requirements for renderer so we can get the resolution

	portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	portdef.nVersion.nVersion = OMX_VERSION;
	portdef.nPortIndex = m_iOutPortRender;
	m_pCompRender->GetParameter(OMX_IndexParamPortDefinition, &portdef);

	m_uWidth = (unsigned int) portdef.format.image.nFrameWidth;
	m_uHeight = (unsigned int) portdef.format.image.nFrameHeight;
	
	printf("ici %i %i\n", m_uWidth, m_uHeight);
	// create EGL image surfaces
	m_eglImage = m_pIEGLImage->CreateEGLImage(m_uWidth, m_uHeight);
}

