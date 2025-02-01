//==========================================================================;
//
//	TSndHdw.CPP
//	WDM TVAudio MiniDriver. 
//		AIW / AIWPro hardware platform. 
//			WDM Properties required hardware settings.
//  Copyright (c) 1996 - 1997  ATI Technologies Inc.  All Rights Reserved.
//
//==========================================================================;

extern "C"
{
#include "strmini.h"
#include "ksmedia.h"

#include "wdmdebug.h"
}

#include "wdmdrv.h"
#include "atitvsnd.h"
#include "aticonfg.h"



/*^^*
 *		GetAudioOperationMode()
 * Purpose	: Called when SRB_GET_PROPERTY SRB SetMode is received. Sets the requested
 *				audio operation mode ( Stereo/SAP). The function will always try to carry the
 *				request on in asynchronous mode. It fails, it will response synchronous mode
 *				of execution.
 *
 * Inputs	:	PULONG pulMode		: the pointer to return current Mode
 *
 * Outputs	: BOOL : returns FALSE, if it is not a XBar property
 *				it also sets the required property
 * Author	: IKLEBANOV
 *^^*/
BOOL CWDMTVAudio::GetAudioOperationMode( PULONG pulAudioMode)
{
	I2CPacket	i2cPacket;
	UCHAR		auchReadValue[2];
	BOOL		bResult, bStereoIndicator, bSAPIndicator;

	if( pulAudioMode == NULL)
		return( FALSE);

	switch( m_uiAudioConfiguration)
	{
		case ATI_AUDIO_CONFIG_1:
		case ATI_AUDIO_CONFIG_5:
			// stereo indication is read back from I2C expander
			if( m_CATIConfiguration.GetTVAudioSignalProperties( m_pI2CScript, 
															    &bStereoIndicator,
															    &bSAPIndicator))
			{
				* pulAudioMode = ( bStereoIndicator) ?
					KS_TVAUDIO_MODE_STEREO : KS_TVAUDIO_MODE_MONO;
				if( bSAPIndicator)
					* pulAudioMode |= KS_TVAUDIO_MODE_LANG_B;

				bResult = TRUE;
			}
			else
				bResult = FALSE;

			break;

		case ATI_AUDIO_CONFIG_2:
			// Signal properties are read back from the Audio chip itself
			i2cPacket.uchChipAddress = m_uchAudioChipAddress;
			i2cPacket.puchWriteBuffer = NULL;
			i2cPacket.puchReadBuffer = auchReadValue;
			i2cPacket.cbWriteCount = 0;
			i2cPacket.cbReadCount = 2;
			i2cPacket.usFlags = I2COPERATION_READ;

			m_pI2CScript->PerformI2CPacketOperation( &i2cPacket);
			if( i2cPacket.uchI2CResult == I2C_STATUS_NOERROR)
			{
				* pulAudioMode = ( auchReadValue[0] & AUDIO_TDA9850_Indicator_Stereo) ?
					KS_TVAUDIO_MODE_STEREO : KS_TVAUDIO_MODE_MONO;
				if( auchReadValue[0] & AUDIO_TDA9850_Indicator_SAP)
					* pulAudioMode |= KS_TVAUDIO_MODE_LANG_B;
					
				bResult = TRUE;
			}
			else
				bResult = FALSE;

			break;

		case ATI_AUDIO_CONFIG_3:
		case ATI_AUDIO_CONFIG_4:
			// Stereo nor SAP are supported
			* pulAudioMode = KS_TVAUDIO_MODE_MONO;
			bResult = TRUE;
			break;

		default:
			bResult = FALSE;
			break;
	}

	return( bResult);
}



/*^^*
 *		SetAudioOperationMode()
 * Purpose	: Called when SRB_SET_PROPERTY SRB SetMode is received. Sets the requested
 *				audio operation mode ( Stereo/SAP). The function will always try to carry the
 *				request on in asynchronous mode. It fails, it will response synchronous mode
 *				of execution.
 *
 * Inputs	:	ULONG ulModeToSet	: the requested mode to set
 *
 * Outputs	: BOOL : returns FALSE, if it is not a XBar property
 *				it also sets the required property
 * Author	: IKLEBANOV
 *^^*/
BOOL CWDMTVAudio::SetAudioOperationMode( ULONG ulModeToSet)
{
	I2CPacket	i2cPacket;
	USHORT		cbWriteLength;
	UCHAR		auchI2CBuffer[2];
	UCHAR		uchDeviceMode = 0;
	
	switch( m_uiAudioConfiguration)
	{
		case ATI_AUDIO_CONFIG_5:
			// TEA5571
		case ATI_AUDIO_CONFIG_1:
			// TEA5582 can not be forced in mono mode; nothing to do
			m_ulTVAudioMode = ulModeToSet;
			return( TRUE);

		case ATI_AUDIO_CONFIG_2:
			// TDA9850
			if( ulModeToSet & KS_TVAUDIO_MODE_STEREO)
				uchDeviceMode |= AUDIO_TDA9850_Control_Stereo;
			if( ulModeToSet & KS_TVAUDIO_MODE_LANG_B)
				uchDeviceMode |= AUDIO_TDA9850_Control_SAP;

			auchI2CBuffer[0] = AUDIO_TDA9850_Reg_Control3;
			auchI2CBuffer[1] = uchDeviceMode;
			cbWriteLength = 2;		// SubAddress + Control Register

			break;

		default:
			return( FALSE);
	}

	i2cPacket.uchChipAddress = m_uchAudioChipAddress;
	i2cPacket.cbReadCount = 0;
	i2cPacket.cbWriteCount = cbWriteLength;
	i2cPacket.puchReadBuffer = NULL;
	i2cPacket.puchWriteBuffer = auchI2CBuffer;
	i2cPacket.usFlags = 0;

	// synchronous mode of operation
	return( m_pI2CScript->PerformI2CPacketOperation( &i2cPacket));
}
