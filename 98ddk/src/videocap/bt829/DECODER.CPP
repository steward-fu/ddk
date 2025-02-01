//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#include "mytypes.h"
#include "Scaler.h"
#include "decoder.h"
#include "constr.h"
#include "dcdrvals.h"

#include "capmain.h"
#include "device.h"

#define CON_vs_BRI   // HW does contrast incorrectly, try to adjust in SW


//===========================================================================
// Bt848 Decoder Class Implementation
//===========================================================================

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
Decoder::Decoder(HW_DEVICE_EXTENSION *deviceExtension) :
   // init register min, max, default
   m_regHue( HueMin, HueMax, HueDef ),
   m_regSaturationNTSC( SatMinNTSC, SatMaxNTSC, SatDefNTSC ),
   m_regSaturationSECAM( SatMinSECAM, SatMaxSECAM, SatDefSECAM ),
   m_regContrast( ConMin, ConMax, ConDef ),
   m_regBrightness( BrtMin, BrtMax, BrtDef ),
   m_param( ParamMin, ParamMax, ParamDef ),
   CONSTRUCT_REGISTERS
{
   // need to set this to 0x4F
   decRegWC_UP = 0x4F;
   // and this one to 0x7F to make sure CRUSH bit works
   decRegWC_DN = 0x7F;

   // HACTIVE should always be 0
   decFieldHACTIVE = 0;

   decFieldHSFMT = 0;

   // BUGBUG. The following lines were commented out in an attempt to
   // have a picture which closely matches what an ordinary TV would
   // show. However, it should be noted that Brooktree recommended to
   // comment out only the 'SetLowColorAutoRemoval' line. Probably the
   // best solution of all would be to somehow expose these options
   // to the application.

   // Instead of using default values, set some registers fields to optimum values
   SetLumaDecimation( TRUE );
   SetChromaAGC( TRUE );
   SetLowColorAutoRemoval( FALSE );
   SetAdaptiveAGC( FALSE );

   // for contrast adjustment purpose
   regBright = 0x00;     // brightness register value before adjustment
   regContrast = 0xD8;   // contrast register value before adjustment
};

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
Decoder::~Decoder()
{
}


//===== Device Status register ==============================================

/////////////////////////////////////////////////////////////////////////////
// Method:  BYTE Decoder::GetDeviceStatusReg( void )
// Purpose: Obtain device status register value
// Input:   None
// Output:  None
// Return:  value of status register in BYTE
/////////////////////////////////////////////////////////////////////////////
BYTE Decoder::GetDeviceStatusReg( void )
{
    BYTE status = (BYTE)decRegSTATUS;
    decRegSTATUS = 0x00;
   return status;
}

/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsVideoPresent( void )
// Purpose: Detect if video is present
// Input:   None
// Output:  None
// Return:  TRUE if video present; else FALSE
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsVideoPresent( void )
{
  return (BOOL) (decFieldPRES == 1);
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsDeviceInHLock( void )
// Purpose: Detect if device is in H-lock
// Input:   None
// Output:  None
// Return:  TRUE if device in H-lock; else FALSE
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsDeviceInHLock( void )
{
  return (BOOL) (decFieldHLOC == 1);
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsEvenField( void )
// Purpose: Reflect whether an even or odd field is being decoded
// Input:   None
// Output:  None
// Return:  TRUE if even field; else FALSE
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsEvenField( void )
{
  return (BOOL) (decFieldEVENFIELD == 1);
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::Is525LinesVideo( void )
// Purpose: Check to see if we are dealing with 525 lines video signal
// Input:   None
// Output:  None
// Return:  TRUE if 525 lines detected; else FALSE (assume 625 lines)
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::Is525LinesVideo( void )
{
  return (BOOL) (decFieldNUML == 0);  //525
}

/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsCrystal0Selected( void )
// Purpose: Reflect whether XTAL0 or XTAL1 is selected
// Input:   None
// Output:  None
// Return:  TRUE if XTAL0 selected; else FALSE (XTAL1 selected)
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsCrystal0Selected( void )
{
  return (BOOL) (decFieldCSEL == 0);
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsLumaOverflow( void )
// Purpose: Indicates if luma ADC overflow
// Input:   None
// Output:  None
// Return:  TRUE if luma ADC overflow; else FALSE
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsLumaOverflow( void )
{
  return (BOOL) (decFieldLOF == 1);
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::ResetLumaOverflow( void )
// Purpose: Reset luma ADC overflow bit
// Input:   None
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::ResetLumaOverflow( void )
{
  decFieldLOF = 0;  // write to it will reset the bit
}

/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsChromaOverflow( void )
// Purpose: Indicates if chroma ADC overflow
// Input:   None
// Output:  None
// Return:  TRUE if chroma ADC overflow; else FALSE
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsChromaOverflow( void )
{
  return (BOOL) (decFieldCOF == 1);
}

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::ResetChromaOverflow( void )
// Purpose: Reset chroma ADC overflow bit
// Input:   None
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::ResetChromaOverflow( void )
{
  decFieldCOF = 0;  // write to it will reset the bit
}


//===== Input Format register ===============================================

/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetVideoInput( Connector source )
// Purpose: Select which connector as input
// Input:   Connector source - SVideo, Tuner, Composite
// Output:  None
// Return:  Fail if error in parameter, else Success
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetVideoInput( Connector source )
{
  if ( ( source != ConSVideo ) &&
       ( source != ConTuner ) &&
       ( source != ConComposite ) )
    return Fail;

  decFieldMUXSEL = source;

  // set to composite or Y/C component video depends on video source
  SetCompositeVideo( ( source == ConSVideo ) ? FALSE : TRUE );
  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetVideoInput( void )
// Purpose: Get which connector is input
// Input:   None
// Output:  None
// Return:  Video source - SVideo, Tuner, Composite
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetVideoInput( void )
{
  return ((int)decFieldMUXSEL);
}

/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetCrystal( Crystal crystalNo )
// Purpose: Select which crystal as input
// Input:   Crystal crystalNo:
//            XT0         - Crystal_XT0
//            XT1         - Crystal_XT1
//            Auto select - Crystal_AutoSelect
// Output:  None
// Return:  Fail if error in parameter, else Success
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetCrystal( Crystal crystalNo )
{
  if ( ( crystalNo < Crystal_XT0 ) || ( crystalNo >  Crystal_AutoSelect ) )
    return Fail;

  decFieldXTSEL = crystalNo;
  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetCrystal( void )
// Purpose: Get which crystal is input
// Input:   None
// Output:  None
// Return:   Crystal Number:
//            XT0         - Crystal_XT0
//            XT1         - Crystal_XT1
//            Auto select - Crystal_AutoSelect
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetCrystal( void )
{
  return ((int)decFieldXTSEL);
}


/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetVideoFormat( VideoFormat format )
// Purpose: Set video format
// Input:   Video format -
//            Auto format:          VFormat_AutoDetect
//            NTSC (M):             VFormat_NTSC
//            PAL (B, D, G, H, I):  VFormat_PAL_BDGHI
//            PAL (M):              VFormat_PAL_M
//            PAL(N):               VFormat_PAL_N
//            SECAM:                VFormat_SECAM
// Output:  None
// Return:  Fail if error in parameter, else Success
// Notes:   Available video formats are: NTSC, PAL(B, D, G, H, I), PAL(M),
//                                       PAL(N), SECAM
//          This function also sets the AGCDelay (ADELAY) and BrustDelay
//          (BDELAY) registers
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetVideoFormat( VideoFormat format )
{
  if ( (format <  VFormat_AutoDetect)  ||
       (format >  VFormat_SECAM)       ||
       (format == VFormat_Reserved2) )
    return Fail;

  switch (format)
  {
    case VFormat_NTSC:
      decFieldFORMAT = format;
      decRegADELAY = 0x68;
      decRegBDELAY = 0x5D;
      SetChromaComb( TRUE );        // enable chroma comb
      SelectCrystal( 'N' );         // select NTSC crystal
      break;

    case VFormat_PAL_BDGHI:
    case VFormat_PAL_M:
    case VFormat_PAL_N:
      decFieldFORMAT = format;
      decRegADELAY = 0x7F;
      decRegBDELAY = 0x72;
      SetChromaComb( TRUE );        // enable chroma comb
      SelectCrystal( 'P' );         // select PAL crystal
      break;

    case VFormat_SECAM:
      decFieldFORMAT = format;
      decRegADELAY = 0x7F;
      decRegBDELAY = 0xA0;
      SetChromaComb( FALSE );       // disable chroma comb
      SelectCrystal( 'P' );         // select PAL crystal
      break;
      
    default: // VFormat_AutoDetect
      // auto format detect by examining the number of lines
      if ( Decoder::Is525LinesVideo() ) // lines == 525 -> NTSC
        Decoder::SetVideoFormat( VFormat_NTSC );
      else  // lines == 625 -> PAL/SECAM
        Decoder::SetVideoFormat( VFormat_PAL_BDGHI );    // PAL_BDGHI covers most areas 
  }

  SetSaturation( m_satParam );
  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetVideoFormat( void )
// Purpose: Obtain video format
// Input:   None
// Output:  None
// Return:  Video format
//            Auto format:          VFormat_AutoDetect
//            NTSC (M):             VFormat_NTSC
//            PAL (B, D, G, H, I):  VFormat_PAL_BDGHI
//            PAL (M):              VFormat_PAL_M
//            PAL(N):               VFormat_PAL_N
//            SECAM:                VFormat_SECAM
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetVideoFormat( void )
{
   BYTE bFormat = (BYTE)decFieldFORMAT;
   if ( !bFormat ) // autodetection enabled
      return Is525LinesVideo() ? VFormat_NTSC : VFormat_SECAM;
   else
     return bFormat;
}


//===== Temporal Decimation register ========================================

/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetRate( BOOL fields, VidField even, int rate )
// Purpose: Set frames or fields rate
// Input:   BOOL fields   - TRUE for fields, FALSE for frames
//          VidField even - TRUE to start decimation with even field, FALSE odd
//          int  rate     - decimation rate: frames (1-50/60); fields(1-25/30)
// Output:  None
// Return:  Fail if error in parameter, else Success
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetRate( BOOL fields, VidField vf, int rate )
{
  int nMax;
  if ( Is525LinesVideo() == TRUE )
    nMax = 30;  // NTSC
  else
    nMax = 25;  // PAL/SECAM

  // if setting frame rate, double the max value
  if ( fields == FALSE )
    nMax *= 2;

  if ( rate < 0 || rate > nMax )
    return Fail;

  decFieldDEC_FIELD = (fields == FALSE) ? Off : On;
  decFieldDEC_FIELDALIGN = (vf == VF_Even) ? On : Off;
  int nDrop = (BYTE) nMax - rate;
  decFieldDEC_RAT = (BYTE) (fields == FALSE) ? nDrop : nDrop * 2;

  return Success;
}


//===== Brightness Control register =========================================

/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetBrightness( int param )
// Purpose: Set video brightness
// Input:   int param - parameter value (0-255; default 128)
// Output:  None
// Return:  Fail if error in parameter, else Success
// Note:    See IsAdjustContrast() for detailed description of the contrast
//          adjustment calculation
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetBrightness( int param )
{
  if( m_param.OutOfRange( param ) )
    return Fail;

  // perform mapping to our range
  int mapped;
  if ( Mapping( param, m_param, &mapped, m_regBrightness ) == Fail )
    return Fail;

  m_briParam = (WORD)param;

  // calculate brightness value
  int value = (128 * mapped) / m_regBrightness.Max() ;

  // need to limit the value to 0x7F (+50%) because 0x80 is -50%!
  if (( mapped > 0 ) && ( value == 0x80 ))
    value = 0x7F;

  // perform adjustment of brightness register if adjustment is needed
  if ( IsAdjustContrast() )
  {
    regBright = value;   // brightness value before adjustment

    long A = (long)regBright * (long)0xD8;
    long B = 64 * ( (long)0xD8 - (long)regContrast );
    long temp = 0x00;
    if ( regContrast != 0 )  // already limit contrast > zero; just in case here
       temp = ( ( A + B ) / (long)regContrast );
    temp = ( temp < -128 ) ? -128 : ( ( temp > 127 ) ? 127 : temp );
    value = (BYTE)temp;

  }

  decRegBRIGHT = (BYTE)value;

  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetBrightness( void )
// Purpose: Obtain brightness value
// Input:   None
// Output:  None
// Return:  Brightness parameter (0-255)
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetBrightness( void )
{
  return m_briParam;
}


//===== Miscellaneous Control register (E_CONTROL, O_CONTROL) ===============

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetLumaNotchFilter( BOOL mode )
// Purpose: Enable/Disable luma notch filter
// Input:   BOOL mode - TRUE = Enable; FALSE = Disable
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetLumaNotchFilter( BOOL mode )
{
  decFieldLNOTCH = (mode == FALSE) ? On : Off;  // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsLumaNotchFilter( void )
// Purpose: Check if luma notch filter is enable or disable
// Input:   None
// Output:  None
// Return:  TRUE = Enable; FALSE = Disable
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsLumaNotchFilter( void )
{
  return (decFieldLNOTCH == Off) ? TRUE : FALSE;  // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetCompositeVideo( BOOL mode )
// Purpose: Select composite or Y/C component video
// Input:   BOOL mode - TRUE = Composite; FALSE = Y/C Component
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetCompositeVideo( BOOL mode )
{
  if ( mode == TRUE )
  {
    // composite video
    decFieldCOMP = Off;
    Decoder::SetChromaADC( FALSE );  // disable chroma ADC
    Decoder::SetLumaNotchFilter( TRUE );  // enable luma notch filter
  }
  else
  {
    // Y/C Component video
    decFieldCOMP = On;
    Decoder::SetChromaADC( TRUE );  // enable chroma ADC
    Decoder::SetLumaNotchFilter( FALSE );  // disable luma notch filter
  }
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsCompositeVideo( void )
// Purpose: Check if selected composite or Y/C component video
// Input:   None
// Output:  None
// Return:  TRUE = Composite; FALSE = Y/C Component
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsCompositeVideo( void )
{
  return (decFieldCOMP == Off) ? TRUE : FALSE;  // reverse
}

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetLumaDecimation( BOOL mode )
// Purpose: Enable/Disable luma decimation filter
// Input:   BOOL mode - TRUE = Enable; FALSE = Disable
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetLumaDecimation( BOOL mode )
{
   // value of 0 turns the decimation on
   decFieldLDEC = (mode == TRUE) ? 0 : 1;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsLumaDecimation( void )
// Purpose: Check if luma decimation filter is enable or disable
// Input:   None
// Output:  None
// Return:  TRUE = Enable; FALSE = Disable
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsLumaDecimation( void )
{
  return (decFieldLDEC == Off) ? TRUE : FALSE;  // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetCbFirst( BOOL mode )
// Purpose: Control whether the first pixel of a line is a Cb or Cr pixel
// Input:   BOOL mode - TRUE = Normal Cb, Cr order, FALSE = Invert Cb, Cr order
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetCbFirst( BOOL mode )
{
  decFieldCBSENSE = (mode == FALSE) ? On : Off;  // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsCbFirst( void )
// Purpose: Check if the first pixel of a line is a Cb or Cr pixel
// Input:   None
// Output:  None
// Return:  TRUE = Normal Cb, Cr order, FALSE = Invert Cb, Cr order
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsCbFirst( void )
{
  return (decFieldCBSENSE == Off) ? TRUE : FALSE;  // reverse
}


//===== Luma Gain register (CON_MSB, CONTRAST_LO) ===========================

/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetContrast( int param )
// Purpose: Set video contrast
// Input:   int param - parameter value (0-255; default 128)
// Output:  None
// Return:  Fail if error in parameter, else Success
// Note:    See IsAdjustContrast() for detailed description of the contrast
//          adjustment calculation
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetContrast( int param )
{
  if( m_param.OutOfRange( param ) )
    return Fail;

  BOOL adjustContrast = IsAdjustContrast(); // is contrast need to be adjusted

  // if adjust contrast is needed, make sure contrast reg value != 0
  if ( adjustContrast )
    m_regContrast = CRegInfo( 1, ConMax, ConDef );

  // perform mapping to our range
  int mapped;
  if ( Mapping( param, m_param, &mapped, m_regContrast ) == Fail )
    return Fail;

  m_conParam = (WORD)param;

  // calculate contrast
  DWORD value =  (DWORD)0x1FF * (DWORD)mapped;
  value /= (DWORD)m_regContrast.Max();
  if ( value > 0x1FF )
    value = 0x1FF;

  // contrast is set by a 9 bit value; set LSB first
  decRegCONTRAST_LO = value;

  // now set the Miscellaneous Control Register CON_V_MSB to the 9th bit value
  decFieldCON_MSB = ( (value & 0x0100) ? On : Off );

  // perform adjustment of brightness register if adjustment is needed
  if ( adjustContrast )
  {
    regContrast = (WORD)value;    // contrast value

    long A = (long)regBright * (long)0xD8;
    long B = 64 * ( (long)0xD8 - (long)regContrast );
    long temp = 0x00;
    if ( regContrast != 0 )  // already limit contrast > zero; just in case here
       temp = ( ( A + B ) / (long)regContrast );
    temp = ( temp < -128 ) ? -128 : ( ( temp > 127 ) ? 127 : temp );
    decRegBRIGHT = (BYTE)temp;

  }

  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetContrast( void )
// Purpose: Obtain contrast value
// Input:   None
// Output:  None
// Return:  Contrast parameter (0-255)
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetContrast( void )
{
  return m_conParam;
}


//===== Chroma Gain register (SAT_U_MSB, SAT_V_MSB, SAT_U_LO, SAT_V_LO) =====

/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetSaturation( int param )
// Purpose: Set color saturation by modifying U and V values
// Input:   int param - parameter value (0-255; default 128)
// Output:  None
// Return:  Fail if error in parameter, else Success
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetSaturation( int param )
{
  if( m_param.OutOfRange( param ) )
    return Fail;

  // color saturation is controlled by two nine bit values:
  // ChromaU & ChromaV
  // To maintain normal color balance, the ratio between the 2 register
  // values should be kept at the power-up default ratio

  // Note that U & V values for NTSC and PAL are the same, SECAM is different

  WORD nominalNTSC_U = 0xFE;     // nominal value (i.e. 100%) for NTSC/PAL
  WORD nominalNTSC_V = 0xB4;
  WORD nominalSECAM_U = 0x87;    // nominal value (i.e. 100%) for SECAM
  WORD nominalSECAM_V = 0x85;

  CRegInfo regSat;               // selected saturation register; NTSC/PAL or SECAM
  WORD nominal_U, nominal_V;     // selected nominal U and V value; NTSC/PAL or SECAM

  // select U & V values of either NTSC/PAL or SECAM to be used for calculation
  if ( GetVideoFormat() == VFormat_SECAM )
  {
    nominal_U = nominalSECAM_U;
    nominal_V = nominalSECAM_V;
    regSat = m_regSaturationSECAM;
  }
  else
  {
    nominal_U = nominalNTSC_U;
    nominal_V = nominalNTSC_V;
    regSat = m_regSaturationNTSC;
  }

  // perform mapping to our range
  int mapped;
  if ( Mapping( param, m_param, &mapped, regSat ) == Fail )
    return Fail;

  m_satParam = (WORD)param;

  WORD max_nominal = max( nominal_U, nominal_V );

  // calculate U and V values
  WORD Uvalue = (WORD) ( (DWORD)mapped * (DWORD)nominal_U / (DWORD)max_nominal );
  WORD Vvalue = (WORD) ( (DWORD)mapped * (DWORD)nominal_V / (DWORD)max_nominal );

  // set U
  decRegSAT_U_LO = Uvalue;

  // now set the Miscellaneous Control Register SAT_U_MSB to the 9th bit value
  decFieldSAT_U_MSB = ( (Uvalue & 0x0100) ? On : Off );

  // set V
  decRegSAT_V_LO = Vvalue;

  // now set the Miscellaneous Control Register SAT_V_MSB to the 9th bit value
  decFieldSAT_V_MSB = ( (Vvalue & 0x0100) ? On : Off );

  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetSaturation( void )
// Purpose: Obtain saturation value
// Input:   None
// Output:  None
// Return:  Saturation parameter (0-255)
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetSaturation( void )
{
  return m_satParam;
}


//===== Hue Control register (HUE) ==========================================

/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetHue( int param )
// Purpose: Set video hue
// Input:   int param - parameter value (0-255; default 128)
// Output:  None
// Return:  Fail if error in parameter, else Success
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetHue( int param )
{
  if( m_param.OutOfRange( param ) )
    return Fail;

  // perform mapping to our range
  int mapped;
  if ( Mapping( param, m_param, &mapped, m_regHue ) == Fail )
    return Fail;

  m_hueParam = (WORD)param;

  int value = (-128 * mapped) / m_regHue.Max();

  if (value > 127)
    value = 127;
  else if (value < -128)
    value = -128;

  decRegHUE = value;

  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetHue( void )
// Purpose: Obtain hue value
// Input:   None
// Output:  None
// Return:  Hue parameter (0-255)
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetHue( void )
{
  return m_hueParam;
}


//===== SC Loop Control register (E_SCLOOP, O_SCLOOP) =======================


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetChromaAGC( BOOL mode )
// Purpose: Enable/Disable Chroma AGC compensation
// Input:   BOOL mode - TRUE = Enable, FALSE = Disable
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetChromaAGC( BOOL mode )
{
  decFieldCAGC = (mode == FALSE) ? Off : On;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsChromaAGC( void )
// Purpose: Check if Chroma AGC compensation is enable or disable
// Input:   None
// Output:  None
// Return:  TRUE = Enable, FALSE = Disable
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsChromaAGC( void )
{
  return (decFieldCAGC == On) ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetLowColorAutoRemoval( BOOL mode )
// Purpose: Enable/Disable low color detection and removal
// Input:   BOOL mode - TRUE = Enable, FALSE = Disable
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetLowColorAutoRemoval( BOOL mode )
{
  decFieldCKILL = (mode == FALSE) ? Off : On;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsLowColorAutoRemoval( void )
// Purpose: Check if low color detection and removal is enable or disable
// Input:   None
// Output:  None
// Return:  TRUE = Enable, FALSE = Disable
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsLowColorAutoRemoval( void )
{
  return (decFieldCKILL == On) ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetHorizontalFilter( HorizFilter hFilter )
// Purpose: Control the configuration of the optional 6-tap Horizontal Low-Pass filter
// Input:   HoriFilter hFilter:
//            Auto Format - HFilter_AutoFormat
//            CIF         - HFilter_CIF
//            QCIF        - HFilter_QCIF
//            ICON        - HFilter_ICON
// Output:  None
// Return:  Fail if error in parameter, else Success
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetHorizontalFilter( HorizFilter hFilter )
{
  if ( (hFilter < HFilter_AutoFormat) ||
       (hFilter > HFilter_ICON) )
    return Fail;

  decFieldHFILT = hFilter;
  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetHorizontalFilter( void )
// Purpose: Get the configuration of the optional 6-tap Horizontal Low-Pass filter
// Input:   None
// Output:  None
// Return:  Which filter is using:
//            Auto Format - HFilter_AutoFormat
//            CIF         - HFilter_CIF
//            QCIF        - HFilter_QCIF
//            ICON        - HFilter_ICON
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetHorizontalFilter( void )
{
  return ((int)decFieldHFILT);
}


//===== Output Format register (OFORM) ======================================

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetFullOutputRange( BOOL mode )
// Purpose: Enable/Disable full output range
// Input:   BOOL mode - TRUE = Enable, FALSE = Disable
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetFullOutputRange( BOOL mode )
{
  decFieldRANGE = (mode == FALSE) ? Off : On;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsFullOutputRange( void )
// Purpose: Check if full output range is enable or disable
// Input:   None
// Output:  None
// Return:  TRUE = Enable, FALSE = Disable
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsFullOutputRange( void )
{
  return (decFieldRANGE == On) ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetLumaCoring( CoringLevel cLevel )
// Purpose: Set luminance level such that luminance signal is truncated to zero
//          if below this level
// Input:   CoringLevel cLevel -
//            Coring_None: no coring
//            Coring_8:    8
//            Coring_16:   16
//            Coring_32:   32
// Output:  None
// Return:  Fail if error in parameter, else Success
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetLumaCoring( CoringLevel cLevel )
{
  if ( ( cLevel < Coring_None) || ( cLevel > Coring_32 ) )
    return Fail;

  decFieldCORE = cLevel;
  return Success;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetLumaCoring( void )
// Purpose: Get luminance level such that luminance signal is truncated to zero
//          if below this level
// Input:   None
// Output:  None
// Return:  Luma coring level -
//            Coring_None: no coring
//            Coring_8:    8
//            Coring_16:   16
//            Coring_32:   32
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetLumaCoring( void )
{
  return ((int)decFieldCORE);
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetVBIFrameMode( BOOL mode )
// Purpose: Enable/Disable VBI frame output mode
// Input:   BOOL mode - TRUE = Enable, FALSE = Disable
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetVBIFrameMode( BOOL mode )
{
  decFieldVBI_FRAME = (mode == FALSE) ? Off : On;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsVBIFrameMode( void )
// Purpose: Check if VBI frame output mode is enabled
// Input:   None
// Output:  None
// Return:  TRUE = Enable, FALSE = Disable
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsVBIFrameMode( void )
{
  return (decFieldVBI_FRAME == On) ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetCodeInsertionEnabled( BOOL mode )
// Purpose: 
// Input:   BOOL mode - TRUE = Disabled, FALSE = Enabled
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetCodeInsertionEnabled( BOOL mode )
{
  decFieldCODE = (mode == TRUE) ? On : Off;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsCodeInsertionEnabled( void )
// Purpose: Check if code insertion in data stream is enabled
// Input:   None
// Output:  None
// Return:  TRUE = enabled, FALSE = disabled
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsCodeInsertionEnabled( void )
{
  return (decFieldCODE == On) ? TRUE : FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::Set16BitDataStream( BOOL mode )
// Purpose: 8 or 16 bit data stream
// Input:   BOOL mode - TRUE = 16, FALSE = 8
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::Set16BitDataStream( BOOL mode )
{
  decFieldLEN = (mode == TRUE) ? On : Off;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::Is16BitDatastream( void )
// Purpose: Check if 16 bit data stream
// Input:   None
// Output:  None
// Return:  TRUE = 16, FALSE = 8
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::Is16BitDataStream( void )
{
  return (decFieldLEN == On) ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::SetOES( ThreeState tState )
// Purpose: Control which outputs are tri-stated
// Input:   ThreeState tState -
//              TS_Timing_Data          - tri-states timing and data
//              TS_Data                 - tri-states data only
//              TS_Timing_Data_Clock    - tri-states timing, data, clock
//              TS_Clock_Data           - tri-states clocks and data
// Output:  None
// Return:  Fail if error in parameter, else Success
/////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::SetOES( ThreeState tState )
{
  if ( ( tState < TS_Timing_Data) || ( tState > TS_Clock_Data ) )
    return Fail;

  decFieldOES = tState;
  return Success;
}

/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetOES( void )
// Purpose: Get OES value from OFORM register
// Input:   None
// Output:  None
// Returns:   
//              TS_Timing_Data          - tri-states timing and data
//              TS_Data                 - tri-states data only
//              TS_Timing_Data_Clock    - tri-states timing, data, clock
//              TS_Clock_Data           - tri-states clocks and data
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetOES( void )
{
  return ((int)decFieldOES);
}

//===== Vertical Scaling register (E_VSCALE_HI, O_VSCALE_HI) ================

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetChromaComb( BOOL mode )
// Purpose: Enable/Disable chroma comb
// Input:   BOOL mode - TRUE = Enable, FALSE = Disable
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetChromaComb( BOOL mode )
{
  decFieldCOMB = (mode == FALSE) ? Off : On;
}

/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsChromaComb( void )
// Purpose: Check if chroma comb is enable or disable
// Input:   None
// Output:  None
// Return:  TRUE = Enable, FALSE = Disable
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsChromaComb( void )
{
  return (decFieldCOMB == On) ? TRUE : FALSE;
}
   
/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetInterlaced( BOOL mode )
// Purpose: Enable/Disable Interlace
// Input:   BOOL mode - TRUE = Interlaced
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetInterlaced( BOOL mode )
{
  decFieldINT = (mode == FALSE) ? Off : On;
}

/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsInterlaced( void )
// Purpose: Check if interlaced or non-interlaced
// Input:   None
// Output:  None
// Return:  TRUE = Interlaced
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsInterlaced( void )
{
  return (decFieldINT == On) ? TRUE : FALSE;
}
   
//===== VPOLE register ==================================================

BOOL Decoder::IsOutputEnabled (void)
{
    return (decFieldOUT_EN == m_outputEnablePolarity);
}

void Decoder::SetOutputEnabled (BOOL mode)
{
    decFieldOUT_EN = (mode == TRUE) ? m_outputEnablePolarity : !m_outputEnablePolarity;
}

BOOL Decoder::IsHighOdd (void)
{
    return (decFieldFIELD == 0); // 0 enabled; 1 even
}

void Decoder::SetHighOdd (BOOL mode)
{
    decFieldFIELD = (mode == TRUE) ? 0 : 1; // 0 enabled; 1 even
}

//===== AGC Delay register (ADELAY) =========================================

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetAGCDelay( BYTE value )
// Purpose: Set AGC Delay register
// Input:   Value to be set to
// Output:  None
// Return:  None
// NOTE:    This function set the AGC Delay register to the specified value.
//          No calculation is involved.
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetAGCDelay( BYTE value )
{
#pragma message ("IS THIS GOOD?? ")
  decRegADELAY = value;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetAGCDelay( void )
// Purpose: Get AGC Delay register
// Input:   None
// Output:  None
// Return:  Register value
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetAGCDelay( void )
{
  return ((int)decRegADELAY);
}


//===== Burst Delay register (BDELAY) =========================================

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetBurstDelay( BYTE value )
// Purpose: Set Burst Delay register
// Input:   Value to be set to
// Output:  None
// Return:  None
// NOTE:    This function set the Burst Delay register to the specified value.
//          No calculation is involved.
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetBurstDelay( BYTE value )
{
#pragma message ("IS THIS GOOD?? ")
  decRegBDELAY = value;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  int Decoder::GetBurstDelay( void )
// Purpose: Get Burst Delay register
// Input:   None
// Output:  None
// Return:  Register value
/////////////////////////////////////////////////////////////////////////////
int Decoder::GetBurstDelay( void )
{
  return ((int)decRegBDELAY);
}


//===== ADC Interface register (ADC) =========================================

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetAnalogThresholdLow( BOOL mode )
// Purpose: Define high/low threshold level below which SYNC signal can be detected
// Input:   BOOL mode - TRUE = low threshold (~75mV), FALSE = high threshold (~125mV)
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetAnalogThresholdLow( BOOL mode )
{
  decFieldSYNC_T = (mode == FALSE) ? Off : On;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsAnalogThresholdLow( void )
// Purpose: Check if high or low threshold level below which SYNC signal can be detected
// Input:   None
// Output:  None
// Return:  TRUE = low threshold (~75mV), FALSE = high threshold (~125mV)
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsAnalogThresholdLow( void )
{
  return (decFieldSYNC_T == On) ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetAGCFunction( BOOL mode )
// Purpose: Enable/Disable AGC function
// Input:   BOOL mode - TRUE = Enable, FALSE = Disable
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetAGCFunction( BOOL mode )
{
  decFieldAGC_EN = (mode == FALSE) ? On : Off; // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsAGCFunction( void )
// Purpose: Check if AGC function is enable or disable
// Input:   None
// Output:  None
// Return:  TRUE = Enable, FALSE = Disable
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsAGCFunction( void )
{
  return (decFieldAGC_EN == Off) ? TRUE : FALSE;   // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::PowerDown( BOOL mode )
// Purpose: Select normal or shut down clock operation
// Input:   BOOL mode - TRUE = shut down, FALSE = normal operation
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::PowerDown( BOOL mode )
{
  decFieldCLK_SLEEP = (mode == FALSE) ? Off : On;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsPowerDown( void )
// Purpose: Check if clock operation has been shut down
// Input:   None
// Output:  None
// Return:  TRUE = shut down, FALSE = normal operation
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsPowerDown( void )
{
  return (decFieldCLK_SLEEP == On) ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetLumaADC( BOOL mode )
// Purpose: Select normal or sleep Y ADC operation
// Input:   BOOL mode - TRUE = normal, FALSE = sleep
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetLumaADC( BOOL mode )
{
  decFieldY_SLEEP = (mode == FALSE) ? On : Off; // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsLumaADC( void )
// Purpose: Check if Y ADC operation is in normal operation or sleeping
// Input:   None
// Output:  None
// Return:  TRUE = normal, FALSE = sleep
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsLumaADC( void )
{
  return (decFieldY_SLEEP == Off) ? TRUE : FALSE;  // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetChromaADC( BOOL mode )
// Purpose: Select normal or sleep C ADC operation
// Input:   BOOL mode - TRUE = normal, FALSE = sleep
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SetChromaADC( BOOL mode )
{
  decFieldC_SLEEP = (mode == FALSE) ? On : Off; // reverse
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsChromaADC( void )
// Purpose: Check if C ADC operation is in normal operation or sleeping
// Input:   None
// Output:  None
// Return:  TRUE = normal, FALSE = sleep
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsChromaADC( void )
{
  return (decFieldC_SLEEP == Off) ? TRUE : FALSE; // reverse
}


/*^^////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SetAdaptiveAGC( BOOL mode )
// Purpose: Set adaptive or non-adaptive AGC operation
// Input:   BOOL mode - TRUE = Adaptive, FALSE = Non-adaptive
// Output:  None
// Return:  None
*////////////////////////////////////////////////////////////////////////////
void Decoder::SetAdaptiveAGC( BOOL mode )
{
   decFieldCRUSH = (mode == FALSE) ? Off : On;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsAdaptiveAGC( void )
// Purpose: Check if adaptive or non-adaptive AGC operation is selected
// Input:   None
// Output:  None
// Return:  TRUE = Adaptive, FALSE = Non-adaptive
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsAdaptiveAGC( void )
{
  return (decFieldCRUSH == On) ? TRUE : FALSE;
}


//===== Software Reset register (SRESET) ====================================

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SoftwareReset( void )
// Purpose: Perform software reset; all registers set to default values
// Input:   None
// Output:  None
// Return:  None
/////////////////////////////////////////////////////////////////////////////
void Decoder::SoftwareReset( void )
{
  decRegSRESET = 0x00;  // write any value will do
}


//===== Test Control register (TEST) ========================================

/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::AdjustInertialDampener( BOOL mode )
// Purpose: for factory diagnostics only
// Input:   TRUE or FALSE
// Output:  None
// Return:  None
// NOTE:    For factory diagnostics only!!!!!!!
//          John Welch's dirty little secret
/////////////////////////////////////////////////////////////////////////////
void Decoder::AdjustInertialDampener( BOOL mode )
{
#pragma message ("FOR TEST DIAGNOSTICS ONLY!  ")
  decRegTEST = (mode == FALSE) ? 0x00 : 0x01;
}


/////////////////////////////////////////////////////////////////////////////
// Method:  void Decoder::SelectCrystal( char useCrystal )
// Purpose: Select correct crystal for NTSC or PAL
// Input:   char useCrystal - 'N' for NTSC; 'P' for PAL
// Output:  None
// Return:  None
// NOTE:    Assume at most 2 crystals installed in hardware. i.e. 1 for NTSC
//          and the other for PAL/SECAM.
//          If there is only 1 crystal exists (which must be crystal XT0),
//          do nothing since it is already selected.
/////////////////////////////////////////////////////////////////////////////
void Decoder::SelectCrystal( char useCrystal )
{
#pragma message( "do something about registry" )
/*
   // locate crystal information in the registry
   // the keys to look for in registry are:
   //    1. Bt848\NumXTAL - number of crystal installed
   //                       possible values are "1" or "2"
   //    2. Bt848\XT0     - what crystal type is for crystal 0
   //                       possible values are "NTSC", "PAL"
   // There is another key exist which may be useful in the future:
   //    Bt848\XT1        - what crystal type is for crystal 1
   //                       possible values are "NTSC", "PAL", and "NONE"

   VRegistryKey vkey( PRK_CLASSES_ROOT, "Bt848" );

   // make sure the key exists
   if ( vkey.lastError() == ERROR_SUCCESS )
   {
      char * numCrystalKey = "NumXTAL";
      char   nCrystal[5];
      DWORD  nCrystalLen = 2;    // need only first char; '1' or '2'

      // get number of crystal exists
      if ( vkey.getSubkeyValue( numCrystalKey, nCrystal, (DWORD *)&nCrystalLen ) )
      {
         // if there is only 1 crystal, no other crystal to change to
         if ( nCrystal[0] == '2' )
         {
            char * crystalTypeKey = "XT0";    // crystal 0 type
            char   crystalType[10];
            DWORD  crystalTypeLen = 6;    // need only first char: 'N' or 'P'

            // get the crystal 0 information
            if ( vkey.getSubkeyValue( crystalTypeKey, crystalType, (DWORD *)&crystalTypeLen ) )
               // compare with what we want to use
               if ( (  IsCrystal0Selected() && ( crystalType[0] != useCrystal ) ) ||
                    ( !IsCrystal0Selected() && ( crystalType[0] == useCrystal ) ) )
                  // need to change crystal
                  SetCrystal( IsCrystal0Selected() ? Crystal_XT1 : Crystal_XT0 );
         }
      }
   }
*/   
}

/////////////////////////////////////////////////////////////////////////////
// Method:  ErrorCode Decoder::Mapping( int fromValue, CRegInfo fromRange,
//                                           int * toValue, CRegInfo toRange )
// Purpose: Map a value in certain range to a value in another range
// Input:   int fromValue - value to be mapped from
//          CRegInfo fromRange - range of value mapping from
//          CRegInfo toRange   - range of value mapping to
// Output:  int * toValue - mapped value
// Return:  Fail if error in parameter, else Success
// Comment: No range checking is performed here. Assume parameters are in
//          valid ranges.
//          The mapping function does not assume default is always the mid
//          point of the whole range. It only assumes default values of the
//          two ranges correspond to each other.
//          
//          The mapping formula is:
//
//            For fromRange.Min() <= fromValue <= fromRange.Default():
//
//               fromValue (fromRange.Default() - fromRange.Min())
//               ------------------------------------------------ + fromRange.Min()
//                     toRange.Default() - toRange.Min()
//
//            For fromRange.Default() < fromValue <= fromRange.Max():
//
//               (fromValue - fromRange.Default()) (toRange.Max() - toRange.Default())
//               --------------------------------------------------------------------- + toRange.Default()
//                           toRange.Max() - toRange.Default()
//
////////////////////////////////////////////////////////////////////////////
ErrorCode Decoder::Mapping( int fromValue, CRegInfo fromRange,
                                 int * toValue, CRegInfo toRange )
{
   // calculate intermediate values
   DWORD a = toRange.Default() - toRange.Min();
   DWORD b = fromRange.Default() - fromRange.Min();
   DWORD c = toRange.Max() - toRange.Default();
   DWORD d = fromRange.Max() - fromRange.Default();

   // prevent divide by zero
   if ( ( b == 0 ) || ( d == 0 ) )
      return ( Fail );

   // perform mapping
   if ( fromValue <= fromRange.Default() )
      *toValue = (int) (DWORD)fromValue * a / b + (DWORD)toRange.Min();
   else
      *toValue = (int) ( (DWORD)fromValue - (DWORD)fromRange.Default() ) * c / d
                       + (DWORD)toRange.Default();

   return ( Success );
}


/////////////////////////////////////////////////////////////////////////////
// Method:  BOOL Decoder::IsAdjustContrast( void )
// Purpose: Check registry key whether adjust contrast is needed
// Input:   None
// Output:  None
// Return:  TRUE = adjust contrast, FALSE = don't adjust contrast
// Note:    If adjust contrast is turn on, brightness register value will be
//          adjusted such that it remains a constant after the calculation
//          performed by the hardware.
//
//          The formula is:
//             To keep brightness constant (i.e. not affect by changing contrast)
//             set brightness to B/(C/C0)
//             where B is value of brightness before adjustment
//                   C is contrast value
//                   C0 is nominal contrast value (0xD8)
//
//             To adjust the contrast level such that it is at the middle of
//             black and white: set brightness to (B * C0 + 64 * (C0 - C))/C
//             (this is what Intel wants)
//
//             Currently there is still limitation of how much adjustment
//             can be performed. For example, if brightness is already high,
//             (i.e. brightness reg value close to 0x7F), lowering contrast
//             until a certain level will have no adjustment effect on brightness.
//             In fact, it would even bring down brightness to darkness.
//
//             Example 1: if brightness is at nominal value (0x00), contrast can
//                        only go down to 0x47 (brightness adjustment is already
//                        at max of 0x7F) before it starts affecting brightness
//                        which takes it darkness.
//             Example 2: if brightness is at nominal value (0x00), contrast can
//                        go all the way up with brightness adjusted correctly.
//                        However, the max adjustment used is only 0xDC and
//                        the max adjustment we can use is 0x&F.
//             Example 3: if brightness is at max (0x7F), lowering contrast
//                        cannot be compensated by adjusting brightness anymore.
//                        The result is gradually taking brightness to darkness.
//             Example 4: if brightness is at min (0x80), lowering contrast has
//                        no visual effect. Bringing contrast to max is using
//                        0xA5 in brightness for compensation.
//
//             One last note, the center is defined as the middle of the
//             gamma adjusted luminance level. Changing it to use the middle of
//             the linear (RGB) luminance level is possible.
/////////////////////////////////////////////////////////////////////////////
BOOL Decoder::IsAdjustContrast( void )
{
   return FALSE;
/*
   // locate adjust contrast information in the registry
   // the key to look for in registry is:
   //    Bt848\AdjustContrast - 0 = don't adjust contrast
   //                           1 = adjust contrast

   VRegistryKey vkey( PRK_CLASSES_ROOT, "Bt848" );

   // make sure the key exists
   if ( vkey.lastError() == ERROR_SUCCESS )
   {
      char * adjustContrastKey = "AdjustContrast";
      char   key[3];
      DWORD  keyLen = 2;    // need only first char; '0' or '1'

      // get the registry value and check it, if exist
      if ( ( vkey.getSubkeyValue( adjustContrastKey, key, (DWORD *)&keyLen ) ) &&
           ( key[0] == '1' ) )
         return ( TRUE );
   }
   return ( FALSE );
*/   
}


