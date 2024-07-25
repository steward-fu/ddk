// HwcapImage.h: interface for the HwcapImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWCAPIMAGE_H__887F31FD_D912_406F_8735_821A09E0FE96__INCLUDED_)
#define AFX_HWCAPIMAGE_H__887F31FD_D912_406F_8735_821A09E0FE96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// HwcapImage
//
// Trivial Base Image Generator Class - abstract
//
class HwcapImage  
{
public:
	HwcapImage(PKS_VIDEOINFOHEADER VideoHeader, bool bAnimate); 
	virtual ~HwcapImage();

	virtual ULONG GetImage(OUT PVOID Data, IN ULONG ByteCount) = 0;

protected:

	UINT m_biWidth;
    UINT m_biHeight;
    UINT m_biSizeImage;
    UINT m_biWidthBytes;
    UINT m_biBitCount;
    DWORD m_biCompression;

	UCHAR m_LineBuffer[720*3];

	bool m_bAnimate;
};


//////////////////////////////////////////////////////////////////////
// HwcapImageRgb24
//
// Generates NTSC color bars
//
class HwcapImageRgb24 : public HwcapImage  
{
public:
	HwcapImageRgb24(PKS_VIDEOINFOHEADER VideoHeader, bool bAnimate=TRUE);
	~HwcapImageRgb24() {}

	virtual ULONG GetImage(OUT PVOID Data, IN ULONG ByteCount);
};

//////////////////////////////////////////////////////////////////////
// HwcapImageYUV
//
// Generates NTSC color bars
//
class HwcapImageYUV : public HwcapImage  
{
public:
	HwcapImageYUV(PKS_VIDEOINFOHEADER VideoHeader, bool bAnimate=TRUE);
	~HwcapImageYUV() {}

	virtual ULONG GetImage(OUT PVOID Data, IN ULONG ByteCount);

};



#endif // !defined(AFX_HWCAPIMAGE_H__887F31FD_D912_406F_8735_821A09E0FE96__INCLUDED_)
