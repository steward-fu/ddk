// HwcapImage.cpp: implementation of the HwcapImage class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

#include "Hwcap.h"

#include "HwcapImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HwcapImage::HwcapImage(PKS_VIDEOINFOHEADER VideoHeader, bool bAnimate)
{
	m_biWidth        =   VideoHeader->bmiHeader.biWidth;
    m_biHeight       =   VideoHeader->bmiHeader.biHeight;
    m_biSizeImage    =   VideoHeader->bmiHeader.biSizeImage;
    m_biWidthBytes   =   KS_DIBWIDTHBYTES( VideoHeader->bmiHeader );
    m_biBitCount     =   VideoHeader->bmiHeader.biBitCount;
    m_biCompression =   VideoHeader->bmiHeader.biCompression;
	m_bAnimate = bAnimate;
}

HwcapImage::~HwcapImage()
{
}

HwcapImageRgb24::HwcapImageRgb24(PKS_VIDEOINFOHEADER VideoHeader, bool bAnimate)
	:  HwcapImage(VideoHeader, bAnimate)
{
	ASSERT((m_biBitCount == 24) && (m_biCompression == KS_BI_RGB)); 
}

HwcapImageYUV::HwcapImageYUV(PKS_VIDEOINFOHEADER VideoHeader, bool bAnimate)
	:  HwcapImage(VideoHeader, bAnimate)
{
	ASSERT((m_biBitCount == 16) && (m_biCompression == FOURCC_YUV422));
}




///////////////////////////////////////////////////////////////////////
// HwcapImageRgb24::GetImage
//
// Parameters:
// 	Data                 - Data buffer to put the frame image in
// 	ByteCount            - Size of the buffer
// Returns:
// 	Nuber of actually copied bytes
// Comments:
// 	This method is called from the filter's OnProcess() to fill the 
//  available pin's buffer with a synthesized image.
//  This obe produces NTSC color bars.
//	To generate some other type of image, override this method in
//  a derived pin class.
ULONG HwcapImageRgb24::GetImage(OUT PVOID Data, IN ULONG ByteCount)
{
	static UCHAR ColorBarsRGB24 [3][8] = 
	{
	//  Whi Yel Cya Grn Mag Red Blu Blk
		255,  0,255,  0,255,  0,255,  0,    // Blue
		255,255,255,255,  0,  0,  0,  0,    // Green
		255,255,  0,  0,255,255,  0,  0,    // Red
	};

	ASSERT(Data && ByteCount);
	ASSERT(ByteCount >= m_biSizeImage);
	
    // 
    // Synthesize a single line of image data, which will then be replicated
    //

    UINT		Line;
    PUCHAR		Image = (PUCHAR)Data;
	PUCHAR		p = m_LineBuffer;

    for (UINT x = 0; x < m_biWidth; x++) {
        UINT col = (x * 8) / m_biWidth;
        
        *p++ = ColorBarsRGB24[0][col]; // Red
        *p++ = ColorBarsRGB24[1][col]; // Green
        *p++ = ColorBarsRGB24[2][col]; // Blue
    }

	if (m_bAnimate)
		for (UINT x=0; x<8; x++)  {ColorBarsRGB24[1][x]++;}


	// fill the rest of the image

	for (Line = 0; Line < m_biHeight; Line++, Image += m_biWidthBytes) 
		memcpy (Image, m_LineBuffer, m_biWidthBytes);


	return m_biSizeImage;

}

///////////////////////////////////////////////////////////////////////////
// HwcapImageYuv::GetImage
//
// Parameters:
// 	Data                 - Data buffer to put the frame image in
// 	ByteCount            - Size of the buffer
// Returns:
// 	Nuber of actually copied bytes
// Comments:
ULONG HwcapImageYUV::GetImage(OUT PVOID Data, IN ULONG ByteCount)
{
	static UCHAR ColorBarsYUV [4][8] = 
	{
	//  Whi Yel Cya Grn Mag Red Blu Blk
		128, 16,166, 54,202, 90,240,128,    // U
		235,211,170,145,106, 81, 41, 16,    // Y
		128,146, 16, 34,222,240,109,128,    // V
		235,211,170,145,106, 81, 41, 16     // Y
	};

	ASSERT(Data && ByteCount);
	ASSERT(ByteCount >= m_biSizeImage);


    // 
    // Synthesize a single line of image data, which will then be replicated
    //

    UINT		Line;
    PUCHAR		Image = (PUCHAR)Data;
	PUCHAR		p = m_LineBuffer;

    for (UINT x = 0; x < (m_biWidth / 2); x++) {
            UINT col = (x * 8) / (m_biWidth / 2);

            *p++ = ColorBarsYUV[0][col]; // U
            *p++ = ColorBarsYUV[1][col]; // Y
            *p++ = ColorBarsYUV[2][col]; // V
            *p++ = ColorBarsYUV[3][col]; // Y
    }

	if (m_bAnimate)
		for (UINT x=0; x<8; x++) {ColorBarsYUV[1][x]++;ColorBarsYUV[3][x]++;}


	// fill the rest of the image

	for (Line = 0; Line < m_biHeight; Line++, Image += m_biWidthBytes) 
		memcpy (Image, m_LineBuffer, m_biWidthBytes);


	return m_biSizeImage;

}
