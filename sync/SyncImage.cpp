
#include "SyncImage.h"

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>




using namespace std;


SyncImage::SyncImage(string imagePath, time_t dateStart, time_t dateEnd, int wallWidth, int wallHeight, int tileWidth, int tileHeight, int tileX, int tileY)
{
	m_imagePath 	= imagePath;
	m_dateStart 	= dateStart;
	m_dateEnd 	= dateEnd;
	m_wallWidth 	= wallWidth;
	m_wallHeight 	= wallHeight;
	m_tileWidth 	= tileWidth;
	m_tileHeight 	= tileHeight;
	m_tileX 		= tileX;
	m_tileY 		= tileY;

	m_extension = m_imagePath.substr(m_imagePath.find_last_of(".") + 1);


    
	
	

    

}

SyncImage::~SyncImage()
{
	delete m_tex;
    delete m_rpi2D;
}


void SyncImage::Load()
{

	if(m_extension == "png")
    {
        cout << "o" << endl;
		m_rpi2D = new Raspberry2D(1920, 1080);
		cout << "q" << endl;
	    // enable alpha channel (transparency)
	    m_rpi2D->AlphaSize = 8;

	    // set background color to blue
	    m_rpi2D->BackgroundColor[0] = 0;
	    m_rpi2D->BackgroundColor[1] = 0;
	    m_rpi2D->BackgroundColor[2] = 0;

	    // attach surface
	    if (!m_rpi2D->Attach()) {
	        delete m_rpi2D;

        	fprintf(stderr, "Could not attach surface\n");
    	}
		// create texture
	    m_tex = new Texture2D(m_imagePath.c_str());
        
    }
    else if(m_extension == "jpg" || m_extension == "jpeg")
    {
    	cout << "jpeg load" << endl;
    	m_platform = PlatformRPI::GetInstance();
		m_pPlatform = (PlatformRPI *) m_platform.get();
		m_logger = ConsoleLogger::GetInstance();

		// init
		m_pPlatform->SetLogger(m_logger.get());
		m_pVideo = m_pPlatform->VideoInit();
		m_pJPEG = m_pPlatform->GetJPEGDecoder();

        byteSA fileJPEG = SyncImage::read_file();	// load in jpeg file
		const uint8_t *pBufJPEG = fileJPEG.data();
		size_t stSizeBytes = fileJPEG.size();

		// tell jpeg decoder what the buffer size needs to be (mandatory)
		m_pJPEG->SetInputBufSizeHint(stSizeBytes);
cout << "fin " << m_pJPEG->m_uWidth<< endl;
		m_pJPEG->DecompressJPEGStart(pBufJPEG, stSizeBytes);
cout << "fin " << m_pJPEG->m_uWidth<< endl;
		m_pJPEG->WaitJPEGDecompressorReady();
cout << "fin " << m_pJPEG->m_uWidth<< endl;


		// cout << "width " << ((JPEGOpenMax*)m_pJPEG)->m_uWidth << " height " << ((JPEGOpenMax*)m_pJPEG)->m_uHeight << endl;
    }
	


}

void SyncImage::Display()
{
	// PNGs are stored vertically
    // rpi2D->DrawVFlip(*tex);
	float x1 = ((float)m_tileX/(float)m_wallWidth);
	float y1 = (float)m_tileY/(float)m_wallHeight;
	float x2 = (float)(m_tileX+m_tileWidth)/(float)m_wallWidth;
	float y2 = (float)(m_tileY+m_tileHeight)/(float)m_wallHeight;

	cout << "x1 " << x1 << " y1 " << y1 << endl;
	cout << "x2 " << x2 << " y2 " << y2 << endl;

	if(m_extension == "png")
	{
	    Rect src = {x1, 1-y2, x2,1-y1 };
	    m_rpi2D->Draw(*m_tex, src, fullVflipDest);

	    m_rpi2D->SwapBuffers();
	}else if(m_extension == "jpg" || m_extension == "jpeg")
	{
		cout << "jpg display" << endl;
		m_pVideo->RenderFrame(m_wallWidth, m_wallHeight, m_tileWidth, m_tileHeight, m_tileX, m_tileY);
		m_pVideo->Flip();
	}
}

void SyncImage::Process()
{
	cout << "begin to load" << endl;
	Load();
	m_timer.SleepUntilTime(m_dateStart);

	cout << "begin to display" << endl;
	Display();

	m_timer.SleepUntilTime(m_dateEnd);

	//erase buffer
	cout << "fin affichage image" << endl;
}


void SyncImage::Run()
{
	Create();
}

SyncMedia::TypeMedia SyncImage::GetType()
{
	return TYPE_IMAGE;
}

void SyncImage::Stop()
{
	// ?
}

	
byteSA SyncImage::read_file()
{
	
	FILE *F = fopen(m_imagePath.c_str(), "rb");
	if (!F)
	{	
		throw runtime_error((string) "File could not be opened: " + m_imagePath);
	}
	fseek(F, 0, SEEK_END);

	off_t stFileSize = ftell(F);

	// back to beginning
	fseek(F, 0, SEEK_SET);

	// allocate buffer
	byteSA buf;
	buf.resize(stFileSize);

	// fill buffer
	fread(buf.data(), 1, stFileSize, F);

	// close file handle
	fclose(F);

	return buf;
}