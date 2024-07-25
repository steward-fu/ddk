
#ifndef __StreamFormat_h__
#define __StreamFormat_h__


#ifndef mmioFOURCC    
#define mmioFOURCC( ch0, ch1, ch2, ch3 ) 	 	\
 	( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |  \
 	( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif  

//
// FourCC of the YUV formats
// For information about FourCC, go to http://www.webartz.com/fourcc/indexyuv.htm
//
#define FOURCC_Y444 mmioFOURCC('Y', '4', '4', '4')  // TIYUV: 1394 conferencing camera 4:4:4 mode 0
#define FOURCC_UYVY mmioFOURCC('U', 'Y', 'V', 'Y')  // MSYUV: 1394 conferencing camera 4:4:4 mode 1 and 3
#define FOURCC_Y411 mmioFOURCC('Y', '4', '1', '1')  // TIYUV: 1394 conferencing camera 4:1:1 mode 2
#define FOURCC_Y800 mmioFOURCC('Y', '8', '0', '0')  // TIYUV: 1394 conferencing camera 4:1:1 mode 5


extern KS_DATARANGE_VIDEO StreamMode_0;
extern KS_DATARANGE_VIDEO StreamMode_1;
extern KS_DATARANGE_VIDEO StreamMode_2;
extern KS_DATARANGE_VIDEO StreamMode_3;
extern KS_DATARANGE_VIDEO StreamMode_4;
extern KS_DATARANGE_VIDEO StreamMode_5;


// debugging aid
PCHAR KsState(const KSSTATE& s);



#endif