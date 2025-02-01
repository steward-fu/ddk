//==========================================================================;
//
//	ATIConfg.CPP
//	WDM MiniDrivers development.
//		ATIHwConfiguration class implementation.
//  Copyright (c) 1996 - 1997  ATI Technologies Inc.  All Rights Reserved.
//
//==========================================================================;

extern"C"
{
#include "wdmdebug.h"
#include "strmini.h"
}

#include "aticonfg.h"
#include "wdmdrv.h"
#include "atigpio.h"

#include "initguid.h"
#include "atiguids.h"


/*^^*
 *		operator new
 * Purpose	: CATIHwConfiguration class overloaded operator new.
 *				Provides placement for a CATIHwConfiguration class object from the PADAPTER_DEVICE_EXTENSION
 *				allocated by the StreamClassDriver for the MiniDriver.
 *
 * Inputs	:	UINT size_t			: size of the object to be placed
 *				PVOID pAllocation	: casted pointer to the CATIHwConfigurationaticonfg.h
 allocated data
 *
 * Outputs	: PVOID : pointer of the CATIHwConfiguration class object
 * Author	: IKLEBANOV
 *^^*/
PVOID CATIHwConfiguration::operator new( UINT size_t,  PVOID pAllocation)
{

	if( size_t != sizeof( CATIHwConfiguration))
	{
		_DebugPrint(( DebugLevelError, "CATIHwConfiguration: operator new() fails\n"));
		return( NULL);
	}
	else
		return( pAllocation);
}



/*^^*
 *		CATIHwConfiguration()
 * Purpose	: CATIHwConfiguration Class constructor
 *				Determines I2CExpander address and all possible hardware IDs and addresses
 *
 * Inputs	: PDEVICE_OBJECT pDeviceObject	: pointer to the creator DeviceObject
 *			  CI2CScript * pCScript			: pointer to the I2CScript class object
 *			  PUINT puiError				: pointer to return Error code
 *
 * Outputs	: none
 * Author	: IKLEBANOV
 *^^*/
CATIHwConfiguration::CATIHwConfiguration( PDEVICE_OBJECT pDeviceObject, CI2CScript * pCScript, PUINT puiError)
{

	ENSURE
	{
		m_gpioProviderInterface.gpioOpen = NULL;
		m_gpioProviderInterface.gpioAccess = NULL;
		m_pdoDriver = NULL;
		
		m_usE2PROMValidation = ( USHORT)-1;

		if( InitializeAttachGPIOProvider( &m_gpioProviderInterface, pDeviceObject))
			// there was no error to get GPIOInterface from the MiniVDD
			m_pdoDriver = pDeviceObject;
		else
		{
			* puiError = WDMMINI_ERROR_NOGPIOPROVIDER;
			FAIL;
		}

		if( !FindI2CExpanderAddress( pCScript))
		{
			* puiError = WDMMINI_NOHARDWARE;
			FAIL;
		}
	
		if( !FindHardwareProperties( pDeviceObject, pCScript))
		{
			* puiError = WDMMINI_NOHARDWARE;
			FAIL;
		}

/*
		// as soon it's a generic class object, we do not check decoder presence
		if( m_uchDecoderAddress == 0)
		{
			* puiError = WDMMINI_UNKNOWNHARDWARE;
			FAIL;
		}
*/

		* puiError = WDMMINI_NOERROR;

	} END_ENSURE;

   	_DebugPrint(( DebugLevelVerbose, "CATIHwConfig:CATIHwConfiguration() uiError=%x\n",
		* puiError));
}


/*^^*
 *		FindHardwareProperties()
 * Purpose	: Determines hardware properties : I2C address and the type
 *
 * Inputs	: PDEVICEOBJECT	pDeviceObject: pointer to device object
 *			  CI2CScript * pCScript	: pointer to the I2CScript object
 *
 * Outputs	: BOOL, TRUE if a valid ATI hardware Configuration was found
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::FindHardwareProperties( PDEVICE_OBJECT pDeviceObject, CI2CScript * pCScript)
{
	ATI_MULTIMEDIAINFO	atiMultimediaInfo;
	ULONG				ulInfoSize;
	UCHAR				uchI2CValue;
	UCHAR				uchORMask = 0x00;
	UCHAR				uchANDMask = 0xFF;
	BOOL				bResult = TRUE;

	m_uchTunerAddress = 0;
	m_usTunerId = 0;
	m_usTunerPowerConfiguration = ATI_TUNER_POWER_CONFIG_0;

	m_uchDecoderAddress = 0;
	m_usDecoderId = VIDEODECODER_TYPE_NOTINSTALLED;
	m_usDecoderConfiguration = 0;

	m_uchAudioAddress = 0;
	m_uiAudioConfiguration = 0;

	switch( m_uchI2CExpanderAddress)
	{
		case 0x70:			// a standard external tuner board

			m_uchTunerAddress = 0xC0;
			m_uchDecoderAddress = 0x88;
			// we need to determine actual Decoder ID, implement later
			m_usDecoderId = VIDEODECODER_TYPE_BT829;

			if( GetI2CExpanderConfiguration( pCScript, &uchI2CValue))
			{
				m_usTunerId = uchI2CValue & 0x0F;

				m_usDecoderConfiguration = ATI_VIDEODECODER_CONFIG_1;

				if (uchI2CValue & 0x10)
				{
					m_uiAudioConfiguration = ATI_AUDIO_CONFIG_4;
					m_uchAudioAddress = 0x82;
				}
				else
					m_uiAudioConfiguration = ATI_AUDIO_CONFIG_3;
			}

			break;

		case 0x78:			// FM tuner
			m_uchTunerAddress = 0xC0;
			m_uchDecoderAddress = 0x88;
			// we need to determine actual Decoder ID, implement later
			m_usDecoderId = VIDEODECODER_TYPE_BT829;

			if( GetI2CExpanderConfiguration( pCScript, &uchI2CValue))
			{
				m_usTunerId = uchI2CValue & 0x0F;

				m_usDecoderConfiguration = ATI_VIDEODECODER_CONFIG_1;
				m_uiAudioConfiguration = ATI_AUDIO_CONFIG_5;
			}
			break;
/*
		case 0x72:		// a standard exeternal tuner board + some hardcoded properties
			m_uchTunerAddress = 0xC6;

			if( GetI2CExpanderConfiguration( pCScript, &uchI2CValue))
				m_usTunerId = uchI2CValue & 0x0F;

			break;
*/

		case 0x76:		// AllInWonder, configuration is in the BIOS
			ulInfoSize =  sizeof( ATI_MULTIMEDIAINFO);
			if( GetATIBIOSMultimediaInfo( pDeviceObject, &atiMultimediaInfo, &ulInfoSize) &&
				( ulInfoSize >= 3))
			{
				// tuner and decoder Info is included
				m_uchTunerAddress = 0xC6;
				m_uchDecoderAddress = 0x8A;
				m_usDecoderConfiguration = ATI_VIDEODECODER_CONFIG_1;
				m_uiAudioConfiguration = ATI_AUDIO_CONFIG_1;

				m_usTunerId = atiMultimediaInfo.MMInfoByte0;
				m_usDecoderId = atiMultimediaInfo.MMInfoByte2 & 0x07;
			}
			else
				bResult = FALSE;
			break;

		case 0xFF:		// AllInWonder PRO, configuration is in the BIOS
			ulInfoSize =  sizeof( ATI_MULTIMEDIAINFO);
			if( GetATIBIOSMultimediaInfo( pDeviceObject, &atiMultimediaInfo, &ulInfoSize) &&
				( ulInfoSize >= 5))
			{
				// OEM Id information is included
				UCHAR nOEMId = atiMultimediaInfo.MMInfoByte4;
				UCHAR nProductType = ( atiMultimediaInfo.MMInfoByte3 >> 4) & 0x0F;

				m_uchDecoderAddress = 0x8A;
				m_uchTunerAddress = 0xC6;
				
				switch( nOEMId)
				{
					case OEM_ID_ATI:
						switch( nProductType)
						{
							case ATI_PRODUCT_TYPE_AIW_PRO_NODVD:
							case ATI_PRODUCT_TYPE_AIW_PRO_DVD:
								m_usDecoderConfiguration = ATI_VIDEODECODER_CONFIG_2;
								m_uiAudioConfiguration = ATI_AUDIO_CONFIG_2;
								m_usTunerPowerConfiguration = ATI_TUNER_POWER_CONFIG_1;

								m_usTunerId = atiMultimediaInfo.MMInfoByte0;
								m_usDecoderId = atiMultimediaInfo.MMInfoByte2 & 0x07;

								m_uchAudioAddress = 0xB4;

								break;

							case ATI_PRODUCT_TYPE_AIW_PLUS:
								m_usDecoderConfiguration = ATI_VIDEODECODER_CONFIG_2;
								m_uiAudioConfiguration = ATI_AUDIO_CONFIG_6;
								m_usTunerPowerConfiguration = ATI_TUNER_POWER_CONFIG_2;

								m_usTunerId = atiMultimediaInfo.MMInfoByte0;
								m_usDecoderId = atiMultimediaInfo.MMInfoByte2 & 0x07;

								m_uchAudioAddress = 0xB6;

								break;

							default:
								bResult = FALSE;
								break;
						}
						break;

					default:
						bResult = FALSE;
						break;
				}
			}
			else
				bResult = FALSE;

			break;
	}

    _DebugPrint(( DebugLevelVerbose, "CATIHwConfig:FindHardwareConfiguration() \n	\
		Tuner:   Id = %d, I2CAddress = %x\n						\
		Decoder: Id = %d, I2CAddress = %x, Configuration = %d\n	\
		Audio:            I2CAddress = %x, Configuration = %d\n",
				   m_usTunerId,		m_uchTunerAddress,
				   m_usDecoderId,	m_uchDecoderAddress,	m_usDecoderConfiguration,
									m_uchAudioAddress,		m_uiAudioConfiguration));

	return( bResult);
}


/*^^*
 *		GetTunerConfiguration()
 * Purpose	: Gets tuner Id and i2C address
 * Inputs	:	PUINT  puiTunerId		: pointer to return tuner Id
 *				PUCHAR puchTunerAddress	: pointer to return tuner I2C address
 *
 * Outputs	: BOOL : returns TRUE
 *				also sets the requested values into the input pointers
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::GetTunerConfiguration( PUINT puiTunerId, PUCHAR puchTunerAddress)
{

	if(( puiTunerId != NULL) && ( puchTunerAddress != NULL))
	{
		* puiTunerId = ( UINT)m_usTunerId;
		* puchTunerAddress = m_uchTunerAddress;

		return( TRUE);
	}
	else
		return( FALSE);
}



/*^^*
 *		GetDecoderConfiguration()
 * Purpose	: Gets decoder Id and i2C address
 *
 * Inputs	:	puiDecoderId		: pointer to return Decoder Id
 *
 * Outputs	: BOOL : returns TRUE
 *				also sets the requested values into the input pointer
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::GetDecoderConfiguration( PUINT puiDecoderId, PUCHAR puchDecoderAddress)
{

	if(( puiDecoderId != NULL) && ( puchDecoderAddress != NULL))
	{
		* puiDecoderId = ( UINT)m_usDecoderId;
		* puchDecoderAddress = m_uchDecoderAddress;

		return( TRUE);
	}
	else
		return( FALSE);
}



/*^^*
 *		GetAudioConfiguration()
 * Purpose	: Gets Audio solution Id and i2C address
 *
 * Inputs	: PUINT puiAudioConfiguration	: pointer to return Audio configuration Id
 *			  PUCHAR puchAudioAddress		: pointer to return audio hardware
 *												I2C address
 *
 * Outputs	: BOOL : returns TRUE
 *				also sets the requested values into the input pointer
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::GetAudioConfiguration( PUINT puiAudioConfiguration, PUCHAR puchAudioAddress)
{

	if(( puiAudioConfiguration != NULL) && ( puchAudioAddress != NULL))
	{
		* puiAudioConfiguration = ( UINT)m_uiAudioConfiguration;
		* puchAudioAddress = m_uchAudioAddress;

		return( TRUE);
	}
	else
		return( FALSE);
}



/*^^*
 *		InitializeAudioConfiguration()
 * Purpose	: Initializes Audio Chip with default / power up values. This function will
 *				be called at Low priority with i2CProvider locked
 *
 * Inputs	:	CI2CScript * pCScript		: pointer to the I2CScript object
 *				UINT uiAudioConfigurationId	: detected Audio configuration
 *				UCHAR uchAudioChipAddress	: detected Audio chip I2C address
 * Outputs	: none
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::InitializeAudioConfiguration( CI2CScript * pCScript, UINT uiAudioConfigurationId, UCHAR uchAudioChipAddress)
{
	I2CPacket i2cPacket;

	switch( uiAudioConfigurationId)
	{
		case ATI_AUDIO_CONFIG_2:
			// TDA9850 has to be initialized with the values from I2C EEPROM, if
			// those answers the CheckSum. If not, take hardcoded default values
			{
				UINT	nIndex, nNumberOfRegs;
				PUCHAR	puchInitializationBuffer = NULL;
				UCHAR	uchWriteBuffer[2];
				BOOL	bResult = FALSE;

				nNumberOfRegs = AUDIO_TDA9850_Reg_Align3 - AUDIO_TDA9850_Reg_Control1 + 1;

				puchInitializationBuffer = ( PUCHAR) \
					::ExAllocatePool( NonPagedPool, nNumberOfRegs * sizeof( PUCHAR));

				if( puchInitializationBuffer == NULL)
					return( bResult);

				// fill in the Initialization buffer with the defaults values
				puchInitializationBuffer[0] = AUDIO_TDA9850_Control1_DefaultValue;
				puchInitializationBuffer[1] = AUDIO_TDA9850_Control2_DefaultValue;
				puchInitializationBuffer[2] = AUDIO_TDA9850_Control3_DefaultValue;
				puchInitializationBuffer[3] = AUDIO_TDA9850_Control4_DefaultValue;
				puchInitializationBuffer[4] = AUDIO_TDA9850_Align1_DefaultValue;
				puchInitializationBuffer[5] = AUDIO_TDA9850_Align2_DefaultValue;
				puchInitializationBuffer[6] = AUDIO_TDA9850_Align3_DefaultValue;

				// we have to see if anything in I2C EEPROM is waiting for us to
				// overwrite the default values
				if( ValidateConfigurationE2PROM( pCScript))
				{
					// The configuration E2PROM kept its integrity. Let's read the
					// initialization values from the device
					ReadConfigurationE2PROM( pCScript, 3, &puchInitializationBuffer[4]);
					ReadConfigurationE2PROM( pCScript, 4, &puchInitializationBuffer[5]);
				}

				// write the power-up defaults values into the chip
				i2cPacket.uchChipAddress = uchAudioChipAddress;
				i2cPacket.cbReadCount = 0;
				i2cPacket.cbWriteCount = 2;
				i2cPacket.puchReadBuffer = NULL;
				i2cPacket.puchWriteBuffer = uchWriteBuffer;
				i2cPacket.usFlags = I2COPERATION_WRITE;

				for( nIndex = 0; nIndex < nNumberOfRegs; nIndex ++)
				{
					uchWriteBuffer[0] = AUDIO_TDA9850_Reg_Control1 + nIndex;
					uchWriteBuffer[1] = puchInitializationBuffer[nIndex];
					if( !( bResult = pCScript->ExecuteI2CPacket( &i2cPacket)))
						break;
				}

				if( puchInitializationBuffer != NULL)
					::ExFreePool( puchInitializationBuffer);

				return( bResult);
			}
			break;

		default:
			break;
	}

	return( TRUE);
}



/*^^*
 *		GetTVAudioSignalProperties()
 * Purpose	: Gets Audio signal properties readable from ATI dependand hardware,
 *				like I2C expander. This call is always synchronous.
 *
 * Inputs	:	CI2CScript * pCScript	: pointer to the I2CScript object
 *				PBOOL pbStereo			: pointer to the Stereo Indicator
 *				PBOOL pbSAP				: pointer to the SAP Indicator
 *
 * Outputs	: BOOL, returns TRUE, if successful
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::GetTVAudioSignalProperties( CI2CScript * pCScript, PBOOL pbStereo, PBOOL pbSAP)
{
	I2CPacket	i2cPacket;
	UCHAR		uchReadValue, uchWriteValue;
	BOOL		bResult;

	switch( m_uiAudioConfiguration)
	{
		case ATI_AUDIO_CONFIG_1:
		case ATI_AUDIO_CONFIG_5:
			// Stereo property is read back from I2C expander
			i2cPacket.uchChipAddress = m_uchI2CExpanderAddress;
			i2cPacket.cbReadCount = 1;
			i2cPacket.cbWriteCount = 1;
			i2cPacket.puchReadBuffer = &uchReadValue;
			i2cPacket.puchWriteBuffer = &uchWriteValue;
			i2cPacket.usFlags = I2COPERATION_READWRITE;
			i2cPacket.uchORValue = 0x40;
			i2cPacket.uchANDValue = 0xFF;

			bResult = FALSE;

			ENSURE
			{
				if( !pCScript->LockI2CProviderEx())
					FAIL;

				pCScript->ExecuteI2CPacket( &i2cPacket);
				if( !( bResult = ( i2cPacket.uchI2CResult == I2C_STATUS_NOERROR)))
					FAIL;

				i2cPacket.puchWriteBuffer = NULL;
				i2cPacket.usFlags = I2COPERATION_READ;

				pCScript->ExecuteI2CPacket( &i2cPacket);
				if( !( bResult = ( i2cPacket.uchI2CResult == I2C_STATUS_NOERROR)))
					FAIL;

				* pbStereo = uchReadValue & 0x40;

				bResult = TRUE;

			} END_ENSURE;

			pCScript->ReleaseI2CProvider();

			break;

		default:
			bResult = FALSE;
			break;
	}

	if( bResult)
		// no case, where SAP property is read back from ATI's hardware
		* pbSAP = FALSE;

	return( bResult);
}



/*^^*
 *		EnableDecoderOutput()
 * Purpose	: Enables/disables decoder chip
 *				AM bus might have multiple users, it's important to have this feature.
 *
 * Inputs	:	CI2CScript * pCScript	: pointer to the I2CScript object
 *				BOOL bEnable			: defines what to do - enable/disable the decoder's outputs
 *
 * Outputs	: none
 * Author	: IKLEBANOV
 *^^*/
void CATIHwConfiguration::EnableDecoderOutput( CI2CScript * pCScript, BOOL bEnable)
{
	UCHAR		uchORMask = 0;
	UCHAR		uchANDMask = 0xFF;
	UCHAR		uchReadValue, uchWriteValue[2];
	I2CPacket	i2cPacket;
	BOOL		bI2CAccess;

	switch( m_usDecoderConfiguration)
	{
		case ATI_VIDEODECODER_CONFIG_1:
			bI2CAccess = TRUE;
			i2cPacket.uchChipAddress = m_uchI2CExpanderAddress;
			i2cPacket.cbReadCount = 1;
			i2cPacket.cbWriteCount = 1;
			if( bEnable)
				uchANDMask &= 0x7F;
			else
				uchORMask |= 0x80;

			break;

		case ATI_VIDEODECODER_CONFIG_2:
			bI2CAccess = TRUE;
			uchWriteValue[0] = 0x16;
			i2cPacket.uchChipAddress = m_uchDecoderAddress;
			i2cPacket.cbReadCount = 1;
			i2cPacket.cbWriteCount = 2;
			uchANDMask &= 0x7F;
			if( bEnable)
				uchORMask |= 0x80;
			else
				uchORMask |= 0x00;
			break;

		default:
			return;
	}

	if( bI2CAccess)
	{
		i2cPacket.puchReadBuffer = &uchReadValue;
		i2cPacket.puchWriteBuffer = &uchWriteValue[0];
		i2cPacket.usFlags = I2COPERATION_READWRITE;
		i2cPacket.uchORValue = uchORMask;
		i2cPacket.uchANDValue = uchANDMask;

		pCScript->PerformI2CPacketOperation( &i2cPacket);
	}
}




/*^^*
 *		GetI2CExpanderConfiguration()
 * Purpose	: Gets board configuration via I2C expander
 *				Reads the configuration registers back
 * Inputs	:	CI2CScript * pCScript	: pointer to CI2CScript object
 *				PUCHAR puchI2CValue		: pointer to read the I2C value into	
 *
 * Outputs	: BOOL : returns TRUE
 *				also sets the requested values into the input pointers
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::GetI2CExpanderConfiguration( CI2CScript * pCScript, PUCHAR puchI2CValue)
{
	I2CPacket	i2cPacket;

	if( puchI2CValue == NULL)
		return( FALSE);

	i2cPacket.uchChipAddress = m_uchI2CExpanderAddress;
	i2cPacket.cbReadCount = 1;
	i2cPacket.cbWriteCount = 0;
	i2cPacket.puchReadBuffer = puchI2CValue;
	i2cPacket.puchWriteBuffer = NULL;
	i2cPacket.usFlags = 0;

	pCScript->ExecuteI2CPacket( &i2cPacket);

	return(( i2cPacket.uchI2CResult == I2C_STATUS_NOERROR) ? TRUE : FALSE);
}



/*^^*
 *		GetATIBIOSMultimediaInfo()
 * Purpose	: Gets board configuration written in the ATI Video BIOS
 *
 * Inputs	: PDEVICE_OBJECT pDeviceObject			: client's DEVICE_OBJECT pointer
 *			  PATI_MULTIMEDIAINFO pMultimediaInfo	: pointer to the BIOS MultimediaInfo structure
 *			  PULONG pulSize						: pointer to the Info size
 *
 * Outputs	: BOOL : returns TRUE
 *				also sets the requested values into the input pointers
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::GetATIBIOSMultimediaInfo( PDEVICE_OBJECT pDeviceObject, PATI_MULTIMEDIAINFO pMultimediaInfo, PULONG pulSize)
{
	ULONG	ulInfoSize;

	// Let's get MultiMedia data using private interfaces exposed by MiniVDD via
	// the standard Microsoft-defined GPIO interface
	ENSURE
	{
		GPIOControl								gpioAccessBlock;
		ATI_QueryPrivateInterface				pfnQueryInterface;
		PATI_PRIVATE_INTERFACE_CONFIGURATION	pInterface = NULL;

		if( !QueryGPIOProvider( &gpioAccessBlock))
			FAIL;

		if( !::IsEqualGUID( ( const struct _GUID &)gpioAccessBlock.PrivateInterfaceType,
							( const struct _GUID &)GUID_ATI_PRIVATE_INTERFACES_QueryInterface))
			FAIL;

		pfnQueryInterface = ( ATI_QueryPrivateInterface)gpioAccessBlock.PrivateInterface;

		if( pfnQueryInterface == NULL)
			FAIL;

		pfnQueryInterface( pDeviceObject,
						   ( const struct _GUID &)GUID_ATI_PRIVATE_INTERFACES_Configuration,
						   ( PVOID *)&pInterface);
		if( pInterface == NULL)
			FAIL;

		//let's query GetMultimediaInfo Interface member first
		if( !( NT_SUCCESS( pInterface->pfnGetMultimediaConfiguration( pDeviceObject,
																	  NULL,
																	  &ulInfoSize))))
			FAIL;

		if( !ulInfoSize)
			FAIL;

		if( !( NT_SUCCESS( pInterface->pfnGetMultimediaConfiguration( pDeviceObject,
																	  pMultimediaInfo,
																	  pulSize))))
			FAIL;

		return( TRUE);

	} END_ENSURE;

    _DebugPrint(( DebugLevelError, "CATIHwConfig:GetATIBIOSMultimediaInfo() fails to access IConfig for pDO = %x\n",
		pDeviceObject));
	
	// Failure to get MultimediaInfo using newly defined private interfaces inside MiniVDD
	// Let's try the older stuff, where everything is inside the Registry
	ENSURE
	{
		CWDDE32BoardIdBuffer atiBoardConfiguration;

		if( !GetATIBIOSMMInfoFromRegistry( pDeviceObject, &atiBoardConfiguration))
			FAIL;

		* pulSize = sizeof( ATI_MULTIMEDIAINFO);

		// if succeeded, we need to massage the data a little bit
		pMultimediaInfo->MMInfoByte0 = ( UCHAR)atiBoardConfiguration.uiTunerType;
		pMultimediaInfo->MMInfoByte1 =
			( UCHAR)(( atiBoardConfiguration.uiVideoInputConnectorType & 0x03)			|
					 (( atiBoardConfiguration.uiVideoOutputConnectorType & 0x03) << 2)	|
					 (( atiBoardConfiguration.uiCDInputConnector & 0x01) << 4)			|
					 (( atiBoardConfiguration.uiCDOutputConnector & 0x01) << 5)			|
					 (( atiBoardConfiguration.uiVideoPassThrough & 0x03) << 6));
		pMultimediaInfo->MMInfoByte2 =
			( UCHAR)(( atiBoardConfiguration.uiVideoDecoderType	& 0x07)					|
					 (( atiBoardConfiguration.uiVideoDecoderCrystals & 0x07) << 3)		|
					 (( atiBoardConfiguration.uiVideoOutCrystalFrequency & 0x02) << 6));
		pMultimediaInfo->MMInfoByte3 =
			( UCHAR)(( atiBoardConfiguration.uiAudioCircuitType & 0x0F)					|
					 (( atiBoardConfiguration.uchATIProdType & 0x0F) << 4));
		pMultimediaInfo->MMInfoByte4 = atiBoardConfiguration.uchOEM;
		pMultimediaInfo->MMInfoByte5 = atiBoardConfiguration.uchOEMVersion;
		pMultimediaInfo->MMInfoByte6 = atiBoardConfiguration.uchReserved3;
		pMultimediaInfo->MMInfoByte7 = atiBoardConfiguration.uchReserved4;

		return( TRUE);

	} END_ENSURE;

    _DebugPrint(( DebugLevelError, "CATIHwConfig:GetATIBIOSMMInfoFromRegistry() fails for pDO = %x\n",
		pDeviceObject));
}



/*^^*
 *		GetATIBIOSMMInfoFromRegistry()
 * Purpose	: Gets Multimedia board configuration written in the ATI Video BIOS
 *				using the Registry.
 *
 * Inputs	: PDEVICE_OBJECT pDeviceObject				: client's DEVICE_OBJECT pointer
 *			  CWDDE32BoardIdBuffer atiBoardConfiguration: pointer to the MMInfo Configuration structure
 *
 * Outputs	: BOOL : returns TRUE
 *				also sets the requested values into the input pointers
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::GetATIBIOSMMInfoFromRegistry( PDEVICE_OBJECT pDeviceObject,
														PCWDDE32BoardIdBuffer patiBoardConfiguration)
{
	HANDLE	hFolder;

	ENSURE 
	{
		WCHAR 			buf[REGISTRY_BUFFER_SIZE];
		NTSTATUS		ntStatus;
		UNICODE_STRING	uiString;
		ULONG			ulValue;

		patiBoardConfiguration->uiSize = sizeof( CWDDE32BoardIdBuffer);

		patiBoardConfiguration->uiVersion = 0;
		patiBoardConfiguration->uiCardNumber = 0;
		patiBoardConfiguration->uiBoardRevision = 0;

		hFolder = ::OpenRegistryFolder( pDeviceObject, UNICODE_WDM_REG_MULTIMEDIA);
		if( hFolder == NULL)
			FAIL;

		// get tuner type registry info
		RtlZeroMemory( &buf, REGISTRY_BUFFER_SIZE);

		ntStatus = ::ReadStringFromRegistryFolder( hFolder, UNICODE_WDM_REG_MM_TUNER, buf, sizeof( buf));

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		::RtlInitUnicodeString( &uiString, buf);

		ntStatus = ::RtlUnicodeStringToInteger( &uiString, 16, &ulValue);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		patiBoardConfiguration->uiTunerType = ( UINT)ulValue;

		// get video decoder type registry info
		::RtlZeroMemory( &buf, REGISTRY_BUFFER_SIZE);

		ntStatus = ::ReadStringFromRegistryFolder( hFolder, UNICODE_WDM_REG_MM_DECODER, buf, sizeof( buf));

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		::RtlInitUnicodeString( &uiString, buf);

		ntStatus = ::RtlUnicodeStringToInteger( &uiString, 16, &ulValue);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		patiBoardConfiguration->uiVideoDecoderType = ( UINT)ulValue;

		// get video decoder crystals type registry info
		::RtlZeroMemory( &buf, REGISTRY_BUFFER_SIZE);

		ntStatus = ::ReadStringFromRegistryFolder( hFolder, UNICODE_WDM_REG_MM_DECODER_CRYSTALS, buf, REGISTRY_BUFFER_SIZE);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		::RtlInitUnicodeString( &uiString, buf);

		ntStatus = ::RtlUnicodeStringToInteger( &uiString, 16, &ulValue);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		patiBoardConfiguration->uiVideoDecoderCrystals = ( UINT)ulValue;

		// get audio  chip registry info
		::RtlZeroMemory( &buf, REGISTRY_BUFFER_SIZE);

		ntStatus = ::ReadStringFromRegistryFolder( hFolder, UNICODE_WDM_REG_MM_AUDIO, buf, REGISTRY_BUFFER_SIZE);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		::RtlInitUnicodeString( &uiString, buf);

		ntStatus = ::RtlUnicodeStringToInteger( &uiString, 16, &ulValue);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		patiBoardConfiguration->uiAudioCircuitType = ( UINT)ulValue;

		// get OEM Id registry info
		::RtlZeroMemory( &buf, REGISTRY_BUFFER_SIZE);

		ntStatus = ::ReadStringFromRegistryFolder( hFolder, UNICODE_WDM_REG_MM_OEM, buf, REGISTRY_BUFFER_SIZE);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		::RtlInitUnicodeString( &uiString, buf);

		ntStatus = ::RtlUnicodeStringToInteger( &uiString, 16, &ulValue);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		patiBoardConfiguration->uchOEM = ( UCHAR)ulValue;

		// get Product revision registry info
		::RtlZeroMemory( &buf, REGISTRY_BUFFER_SIZE);

		ntStatus = ::ReadStringFromRegistryFolder( hFolder, UNICODE_WDM_REG_MM_PRODUCT_REVISION, buf, REGISTRY_BUFFER_SIZE);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		::RtlInitUnicodeString( &uiString, buf);

		ntStatus = ::RtlUnicodeStringToInteger( &uiString, 16, &ulValue);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		patiBoardConfiguration->uchOEMVersion = ( UCHAR)ulValue;

		// get ATI Product type registry info
		::RtlZeroMemory( &buf, REGISTRY_BUFFER_SIZE);

		ntStatus = ::ReadStringFromRegistryFolder( hFolder, UNICODE_WDM_REG_MM_ATI_PRODUCT, buf, REGISTRY_BUFFER_SIZE);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		::RtlInitUnicodeString( &uiString, buf);

		ntStatus = ::RtlUnicodeStringToInteger( &uiString, 16, &ulValue);

    	if( !NT_SUCCESS( ntStatus))
			FAIL;

		patiBoardConfiguration->uchATIProdType = ( UCHAR)ulValue;

		if( hFolder != NULL)
			::ZwClose( hFolder);

		return( TRUE);

	} END_ENSURE;

	if( hFolder != NULL)
		::ZwClose( hFolder);

	return( FALSE);
}



/*^^*
 *		FindI2CExpanderAddress()
 * Purpose	: Determines I2C expander address.
 *
 * Inputs	:	CI2CScript * pCScript	: pointer to the I2CScript class object
 *
 * Outputs	: BOOL : returns TRUE, if no I2C access error;
 *				also sets m_uchI2CExpanderAddress class member. If any was not found, set it as 0xFF
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::FindI2CExpanderAddress( CI2CScript * pCScript)
{
	USHORT		nIndex;
	UCHAR		uchI2CValue;
	I2CPacket	i2cPacket;
	// table of the possible I2C expender addresses
	UCHAR		auchI2CExpenderAddress[] = { 0x70, 0x78, 0x76};

	// unknown I2C expender address
	m_uchI2CExpanderAddress = 0xFF;
	for( nIndex = 0; nIndex < sizeof( auchI2CExpenderAddress); nIndex ++)
	{
		i2cPacket.uchChipAddress = auchI2CExpenderAddress[nIndex];
		i2cPacket.cbReadCount = 1;
		i2cPacket.cbWriteCount = 0;
		i2cPacket.puchReadBuffer = &uchI2CValue;
		i2cPacket.puchWriteBuffer = NULL;
		i2cPacket.usFlags = 0;

		pCScript->ExecuteI2CPacket( &i2cPacket);
		if( i2cPacket.uchI2CResult == I2C_STATUS_NOERROR)
		{
			m_uchI2CExpanderAddress = auchI2CExpenderAddress[nIndex];
			break;
		}
	}

    _DebugPrint(( DebugLevelVerbose, "CATIHwConfig:FindI2CExpanderAddress() exit address = %x\n", m_uchI2CExpanderAddress));
	return( TRUE);
}



/*^^*
 *		GetAudioProperties()
 * Purpose	: Gets numbers of Audio inputs and outputs
 * Inputs	:	PULONG pulNumberOfInputs	: pointer to return number of Audio inputs
 *				PULONG pulNumberOfOutputs	: pointer to return number of Audio outputs
 *
 * Outputs	: BOOL : returns TRUE
 *				also sets the requested values into the input pointers
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::GetAudioProperties( PULONG pulNumberOfInputs, PULONG pulNumberOfOutputs)
{

	if(( pulNumberOfInputs != NULL) && ( pulNumberOfOutputs != NULL))
	{
		// Hardcoded for AIW with no FM support - FM stuff has not been defined by Microsoft yet 
		* pulNumberOfInputs = 2;
		* pulNumberOfOutputs = 1;

		return( TRUE);
	}
	else
		return( FALSE);
}



/*^^*
 *		CanConnectAudioSource()
 * Purpose	: Determines possibility to connect the specified Audio source to the audio output.
 *
 * Inputs	: int nAudioSource	: the audio source the function is asked about
 *
 * Outputs	: BOOL : returns TRUE, the connection is possible;
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::CanConnectAudioSource( int nAudioSource)
{
	BOOL bResult;

	if( nAudioSource != AUDIOSOURCE_MUTE)
		bResult = nAudioSource < AUDIOSOURCE_LASTSUPPORTED;
	else
		switch( m_uiAudioConfiguration)
		{
			case ATI_AUDIO_CONFIG_1:
			case ATI_AUDIO_CONFIG_2:
			case ATI_AUDIO_CONFIG_4:
			case ATI_AUDIO_CONFIG_5:
				bResult = TRUE;
				break;

			case ATI_AUDIO_CONFIG_3:
			default:
				bResult = FALSE;
				break;
		}

	return( bResult);
}



/*^^*
 *		ConnectAudioSource()
 * Purpose	: Connects the specified Audio input to the Audio output.
 *
 * Inputs	:	CI2CScript * pCScript	: pointer to I2CScript class object
 *				int nAudioSource		: the audio source to be connected to the audio output
 *
 * Outputs	: BOOL : returns FALSE, if either unknown audio source or I2C access error;
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::ConnectAudioSource( CI2CScript *	pCScript, 
											  int			nAudioSource)
{
	UCHAR		uchORMask = 0;
	UCHAR		uchANDMask = 0xFF;
	UCHAR		uchReadValue, uchWriteValue[2];
	I2CPacket	i2cPacket;
	BOOL		bI2CAccess, bResult;
	GPIOControl	gpioAccessBlock;
	UCHAR		uchI2CAddr;
	USHORT		cbWRCount;
	USHORT		cbRDCount;
	USHORT		usI2CMode;

	switch( m_uiAudioConfiguration)
	{
		case ATI_AUDIO_CONFIG_1:

			bI2CAccess = TRUE;
			uchI2CAddr = m_uchI2CExpanderAddress;
			cbWRCount = 1;
			cbRDCount = 1;
			usI2CMode = I2COPERATION_READWRITE;

			switch( nAudioSource)
			{
				case AUDIOSOURCE_MUTE:
					uchANDMask &= 0xAF;
					uchORMask |= 0x00;
					break;
			
				case AUDIOSOURCE_TVAUDIO:
					uchANDMask &= 0xAF;
					uchORMask |= 0x10;
					break;
			
				case AUDIOSOURCE_LINEIN:
					uchANDMask &= 0xAF;
					uchORMask |= 0x50;
					break;
			
				case AUDIOSOURCE_FMAUDIO:
					// no FM is supported

				default:
					return( FALSE);
			}
			break;

		case ATI_AUDIO_CONFIG_2:

			bI2CAccess = FALSE;
			switch( nAudioSource)
			{
				case AUDIOSOURCE_MUTE:
					uchANDMask &= 0xFC;
					uchORMask |= 0x02;
					break;
			
				case AUDIOSOURCE_TVAUDIO:
					uchANDMask &= 0xFC;
					uchORMask |= 0x01;
					break;
			
				case AUDIOSOURCE_LINEIN:
					uchANDMask &= 0xFC;
					uchORMask |= 0x00;
					break;
			
				case AUDIOSOURCE_FMAUDIO:
					uchANDMask &= 0xFC;
					uchORMask |= 0x03;

				default:
					return( FALSE);
			}
			break;


		case ATI_AUDIO_CONFIG_3:

			bI2CAccess = TRUE;
			uchI2CAddr = m_uchI2CExpanderAddress;
			cbWRCount = 1;
			cbRDCount = 1;
			usI2CMode = I2COPERATION_READWRITE;

			switch( nAudioSource)
			{
				case AUDIOSOURCE_TVAUDIO:
					uchANDMask &= 0xDF;
					uchORMask |= 0x00;
					break;
			
				case AUDIOSOURCE_LINEIN:
					uchANDMask &= 0xDF;
					uchORMask |= 0x40;
					break;
			
				case AUDIOSOURCE_FMAUDIO:
					// no FM is supported
				case AUDIOSOURCE_MUTE:
					// no mute is supported
				default:
					return( FALSE);
			}
			break;

		case ATI_AUDIO_CONFIG_4:

			bI2CAccess = TRUE;
			uchI2CAddr = m_uchAudioAddress;
			cbWRCount = 2;
			cbRDCount = 0;
			usI2CMode = I2COPERATION_WRITE;

			switch( nAudioSource)
			{
				case AUDIOSOURCE_MUTE:
					uchWriteValue[0] = 0x08;
					uchWriteValue[1] = 0xF7;
					break;

				case AUDIOSOURCE_TVAUDIO:
					uchWriteValue[0] = 0x08;
					uchWriteValue[1] = 0xD7;
					break;
			
				case AUDIOSOURCE_LINEIN:
					uchWriteValue[0] = 0x08;
					uchWriteValue[1] = 0xCE;
					break;
			
				case AUDIOSOURCE_FMAUDIO:
					// no FM is supported
				default:
					return( FALSE);
			}
			break;

		case ATI_AUDIO_CONFIG_5:

			bI2CAccess = TRUE;
			uchI2CAddr = m_uchI2CExpanderAddress;
			cbWRCount = 1;
			cbRDCount = 1;
			usI2CMode = I2COPERATION_READWRITE;

			switch( nAudioSource)
			{
				case AUDIOSOURCE_MUTE:
					uchANDMask &= 0xAF;
					uchORMask |= 0x50;
					break;
			
				case AUDIOSOURCE_TVAUDIO:
					uchANDMask &= 0xAF;
					uchORMask |= 0x00;
					break;
			
				case AUDIOSOURCE_LINEIN:
					uchANDMask &= 0xAF;
					uchORMask |= 0x40;
					break;
			
				case AUDIOSOURCE_FMAUDIO:
					uchANDMask &= 0xAF;
					uchORMask |= 0x10;

				default:
					return( FALSE);
			}
			break;

		default :
			return( FALSE);
	}

	if( bI2CAccess)
	{
		if( pCScript == NULL)
			return( FALSE);

		i2cPacket.uchChipAddress = uchI2CAddr;
		i2cPacket.cbReadCount = cbRDCount;
		i2cPacket.cbWriteCount = cbWRCount; 
		i2cPacket.puchReadBuffer = &uchReadValue;
		i2cPacket.puchWriteBuffer = uchWriteValue;
		i2cPacket.usFlags = usI2CMode;
		i2cPacket.uchORValue = uchORMask;
		i2cPacket.uchANDValue = uchANDMask;				   	

		// synchronous execution
		bResult = pCScript->PerformI2CPacketOperation( &i2cPacket);
		_DebugPrint(( DebugLevelVerbose, "CATIHwConfig: ConnectAudioSource( %d) = %d\n",
			nAudioSource, bResult));

		if( bResult)
			bResult = ( i2cPacket.uchI2CResult == I2CSCRIPT_NOERROR);

		return( bResult);
	}
	else	
	{
		// use GPIO interface to switch Audio source
		bResult = FALSE;

		ENSURE 
		{
			if(( m_gpioProviderInterface.gpioOpen == NULL) ||
				( m_gpioProviderInterface.gpioAccess == NULL))
				FAIL;

			uchReadValue = AUDIO_MUX_PINS;			// use as a PinMask
			gpioAccessBlock.Pins = &uchReadValue;
			gpioAccessBlock.Flags = GPIO_FLAGS_BYTE;
			gpioAccessBlock.nBytes = 1;
			gpioAccessBlock.nBufferSize = 1;
			gpioAccessBlock.AsynchCompleteCallback = NULL;

			// lock GPIO provider
			if( !LockGPIOProviderEx( &gpioAccessBlock))
				FAIL;

			uchReadValue = AUDIO_MUX_PINS;			// use as a PinMask
			gpioAccessBlock.Command = GPIO_COMMAND_READ_BUFFER;
			gpioAccessBlock.Flags = GPIO_FLAGS_BYTE;
			gpioAccessBlock.dwCookie = m_dwGPIOAccessKey;
			gpioAccessBlock.nBytes = 1;
			gpioAccessBlock.nBufferSize = 1;
			gpioAccessBlock.Pins = &uchReadValue;
			gpioAccessBlock.Buffer = uchWriteValue;
			gpioAccessBlock.AsynchCompleteCallback = NULL;

			if( !AccessGPIOProvider( m_pdoDriver, &gpioAccessBlock))
				FAIL;

			uchWriteValue[0] &= uchANDMask;
			uchWriteValue[0] |= uchORMask;

			gpioAccessBlock.Command = GPIO_COMMAND_WRITE_BUFFER;

			if( !AccessGPIOProvider( m_pdoDriver, &gpioAccessBlock))
				FAIL;

			bResult = TRUE;

		}END_ENSURE;

		// nothing bad will happen if we try to release the provider even we
		// have not obtained it at the first place
		uchReadValue = AUDIO_MUX_PINS;			// use as a PinMask
		gpioAccessBlock.Pins = &uchReadValue;
		gpioAccessBlock.Flags = GPIO_FLAGS_BYTE;
		gpioAccessBlock.nBytes = 1;
		gpioAccessBlock.nBufferSize = 1;
		gpioAccessBlock.AsynchCompleteCallback = NULL;

		ReleaseGPIOProvider( &gpioAccessBlock);

		return( bResult);
	}
}



/*^^*
 *		GPIOIoSynchCompletionRoutine()
 * Purpose	: This routine is for use with synchronous IRP processing.
 *			All it does is signal an event, so the driver knows it and can continue.
 *
 * Inputs	:	PDEVICE_OBJECT DriverObject	: Pointer to driver object created by system
 *				PIRP pIrp					: Irp that just completed
 *				PVOID Event					: Event we'll signal to say Irp is done
 *
 * Outputs	: none
 * Author	: IKLEBANOV
 *^^*/
extern "C"
NTSTATUS GPIOIoSynchCompletionRoutine( IN PDEVICE_OBJECT pDeviceObject,
									   IN PIRP pIrp,
									   IN PVOID Event)
{

    KeSetEvent(( PKEVENT)Event, 0, FALSE);
    return( STATUS_MORE_PROCESSING_REQUIRED);
}



/*^^*
 *		InitializeAttachGPIOProvider()
 * Purpose	: determines the pointer to the parent GPIO Provider interface
 *				This function will be called at Low priority
 *
 * Inputs	:	GPIOINTERFACE * pGPIOInterface	: pointer to the Interface to be filled in
 *				PDEVICE_OBJECT pDeviceObject	: MiniDriver device object, which is a child of GPIO Master
 *
 * Outputs	: BOOL  - returns TRUE, if the interface was found
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::InitializeAttachGPIOProvider( GPIOINTERFACE * pGPIOInterface, PDEVICE_OBJECT pDeviceObject)
{
	BOOL bResult;

	bResult = LocateAttachGPIOProvider( pGPIOInterface, pDeviceObject, IRP_MJ_PNP);
	if(( pGPIOInterface->gpioOpen == NULL) || ( pGPIOInterface->gpioAccess == NULL))
	{
		TRAP;
		DebugPrint(( DebugLevelError, "CATIHwConfig(): GPIO interface has NULL pointers\n"));
		bResult = FALSE;
	}

	return( bResult);
}



/*^^*
 *		LocateAttachGPIOProvider()
 * Purpose	: gets the pointer to the parent GPIO Provider interface
 *				This function will be called at Low priority
 *
 * Inputs	:	GPIOINTERFACE * pGPIOInterface	: pointer to the Interface to be filled in
 *				PDEVICE_OBJECT pDeviceObject	: MiniDriver device object, which is a child of I2C Master
 *				int 		nIrpMajorFunction	: IRP major function to query the GPIO Interface
 *
 * Outputs	: BOOL  - returns TRUE, if the interface was found
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::LocateAttachGPIOProvider( GPIOINTERFACE * pGPIOInterface, PDEVICE_OBJECT pDeviceObject, int nIrpMajorFunction)
{
	PIRP	pIrp;
	BOOL	bResult = FALSE;

	ENSURE
	{
		PIO_STACK_LOCATION	pNextStack;
		NTSTATUS			ntStatus;
		KEVENT				Event;
			
			
		pIrp = IoAllocateIrp( pDeviceObject->StackSize, FALSE);
		if( pIrp == NULL)
		{
			TRAP;
			DebugPrint(( DebugLevelError, "CATIHwConfig(): can not allocate IRP\n"));
			FAIL;
		}

		pNextStack = IoGetNextIrpStackLocation( pIrp);
		if( pNextStack == NULL)
		{
			TRAP;
			DebugPrint(( DebugLevelError, "CATIHwConfig(): can not allocate NextStack\n"));
			FAIL;
		}

		pNextStack->MajorFunction = nIrpMajorFunction;
		pNextStack->MinorFunction = IRP_MN_QUERY_INTERFACE;
		KeInitializeEvent( &Event, NotificationEvent, FALSE);

		IoSetCompletionRoutine(	pIrp,
								GPIOIoSynchCompletionRoutine,
								&Event, TRUE, TRUE, TRUE);

		pNextStack->Parameters.QueryInterface.InterfaceType = ( struct _GUID *)&GUID_GPIO_INTERFACE;
		pNextStack->Parameters.QueryInterface.Size = sizeof( GPIOINTERFACE);
		pNextStack->Parameters.QueryInterface.Version = 1;
		pNextStack->Parameters.QueryInterface.Interface = ( PINTERFACE)pGPIOInterface;
		pNextStack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

		ntStatus = IoCallDriver( pDeviceObject, pIrp);

		if( ntStatus == STATUS_PENDING)
			KeWaitForSingleObject(	&Event,
									Suspended, KernelMode, FALSE, NULL);
		if(( pGPIOInterface->gpioOpen == NULL) || ( pGPIOInterface->gpioAccess == NULL))
		{
/*
			TRAP;
			DebugPrint(( DebugLevelError, "CATIHwConfig(): GPIO interface has NULL pointers\n"));
*/
			FAIL;
		}
		bResult = TRUE;

	} END_ENSURE;
 
	if( pIrp != NULL)
		IoFreeIrp( pIrp);

	return( bResult);
}



/*^^*
 *		QueryGPIOProvider()
 * Purpose	: queries the GPIOProvider for the pins supported and private interfaces
 *
 * Inputs	: PGPIOControl pgpioAccessBlock	: pointer to GPIO control structure
 *
 * Outputs	: BOOL : retunrs TRUE, if the query function was carried on successfully
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::QueryGPIOProvider( PGPIOControl pgpioAccessBlock)
{

	ENSURE
	{
		if(( m_gpioProviderInterface.gpioOpen == NULL)		|| 
			( m_gpioProviderInterface.gpioAccess == NULL)	||
			( m_pdoDriver == NULL))
			FAIL;

		pgpioAccessBlock->Status = GPIO_STATUS_NOERROR;
		pgpioAccessBlock->Command = GPIO_COMMAND_QUERY;
		pgpioAccessBlock->AsynchCompleteCallback = NULL;

	    if(( !NT_SUCCESS( m_gpioProviderInterface.gpioOpen( m_pdoDriver, TRUE, pgpioAccessBlock))) ||
			( pgpioAccessBlock->Status != GPIO_STATUS_NOERROR))
			FAIL;

		return( TRUE);

	} END_ENSURE;

	return( FALSE);
}



/*^^*
 *		LockGPIOProviderEx()
 * Purpose	: locks the GPIOProvider for exclusive use
 *
 * Inputs	: PGPIOControl pgpioAccessBlock	: pointer to GPIO control structure
 *
 * Outputs	: BOOL : retunrs TRUE, if the GPIOProvider is locked
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::LockGPIOProviderEx( PGPIOControl pgpioAccessBlock)
{
	NTSTATUS		ntStatus;
	LARGE_INTEGER	liStartTime, liCurrentTime;

	KeQuerySystemTime( &liStartTime);

	ENSURE
	{
		if(( m_gpioProviderInterface.gpioOpen == NULL)		|| 
			( m_gpioProviderInterface.gpioAccess == NULL)	||
			( m_pdoDriver == NULL))
			FAIL;

		pgpioAccessBlock->Status = GPIO_STATUS_NOERROR;
		pgpioAccessBlock->Command = GPIO_COMMAND_OPEN_PINS;

		while( TRUE)
		{
			KeQuerySystemTime( &liCurrentTime);

			if(( liCurrentTime.QuadPart - liStartTime.QuadPart) >= GPIO_TIMELIMIT_OPENPROVIDER)
			{
				// time has expired for attempting to lock GPIO provider
				return (FALSE);
			}

			ntStatus = m_gpioProviderInterface.gpioOpen( m_pdoDriver, TRUE, pgpioAccessBlock);

		    if(( NT_SUCCESS( ntStatus)) && ( pgpioAccessBlock->Status == GPIO_STATUS_NOERROR))
				break;
		}

		// the GPIO Provider has granted access - save dwCookie for further use
		m_dwGPIOAccessKey = pgpioAccessBlock->dwCookie;

		return( TRUE);

	} END_ENSURE;

	return( FALSE);
}



/*^^*
 *		ReleaseGPIOProvider()
 * Purpose	: releases the GPIOProvider for other clients' use
 *
 * Inputs	: PGPIOControl pgpioAccessBlock	: pointer to a composed GPIO access block
 *
 * Outputs	: BOOL : retunrs TRUE, if the GPIOProvider is released
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::ReleaseGPIOProvider( PGPIOControl pgpioAccessBlock)
{
	NTSTATUS	ntStatus;

	ENSURE
	{
		if(( m_gpioProviderInterface.gpioOpen == NULL)		||
			( m_gpioProviderInterface.gpioAccess == NULL)	||
			( m_pdoDriver == NULL))
			FAIL;

		pgpioAccessBlock->Status = GPIO_STATUS_NOERROR;
		pgpioAccessBlock->Command = GPIO_COMMAND_CLOSE_PINS;
		pgpioAccessBlock->dwCookie = m_dwGPIOAccessKey;

		ntStatus = m_gpioProviderInterface.gpioOpen( m_pdoDriver, FALSE, pgpioAccessBlock);

	    if( !NT_SUCCESS( ntStatus)) 
		{
            _DebugPrint(( DebugLevelError, "CATIHwConfig: ReleaseGPIOProvider() NTSTATUS = %x\n", ntStatus));
			FAIL;
		}

		if( pgpioAccessBlock->Status != GPIO_STATUS_NOERROR)
		{
            _DebugPrint(( DebugLevelError, "CATIHwConfig: ReleaseGPIOProvider() Status = %x\n", pgpioAccessBlock->Status));
			FAIL;
		}

		m_dwGPIOAccessKey = 0;
		return ( TRUE);

	} END_ENSURE;

	return( FALSE);
}



/*^^*
 *		AccessGPIOProvider()
 * Purpose	: provide synchronous type of access to GPIOProvider
 *
 * Inputs	:	PDEVICE_OBJECT pdoDriver	: pointer to the client's device object
 *				PGPIOControl pgpioAccessBlock	: pointer to a composed GPIO access block
 *
 * Outputs	: BOOL, TRUE if acsepted by the GPIO Provider
 *
 * Author	: IKLEBANOV
 *^^*/
BOOL CATIHwConfiguration::AccessGPIOProvider( PDEVICE_OBJECT pdoClient, PGPIOControl pgpioAccessBlock)
{
	NTSTATUS	ntStatus;

	ENSURE 
	{

		if(( m_gpioProviderInterface.gpioOpen == NULL)		|| 
			( m_gpioProviderInterface.gpioAccess == NULL)	||
			( m_pdoDriver == NULL))
			FAIL;

		ntStatus = m_gpioProviderInterface.gpioAccess( pdoClient, pgpioAccessBlock);

	    if( !NT_SUCCESS( ntStatus)) 
		{
            _DebugPrint(( DebugLevelError, "CATIHwConfig: AccessGPIOProvider() NTSTATUS = %x\n", ntStatus));
			FAIL;
		}

		if( pgpioAccessBlock->Status != GPIO_STATUS_NOERROR)
		{
            _DebugPrint(( DebugLevelError, "CATIHwConfig: AccessGPIOProvider() Status = %x\n", pgpioAccessBlock->Status));
			FAIL;
		}

		return TRUE;

	} END_ENSURE;

	return( FALSE);
}



/*^^*
 *		SetTunerPowerState
 * Purpose	: Sets Tuner power mode
 * Inputs	: CI2CScript * pCScript	: pointer to the I2C Provider class
 *			  BOOL bPowerState		: TRUE, if turne the power on
 *
 * Outputs	: BOOL, TRUE if successfull
 * Author	: TOM
 *^^*/
BOOL CATIHwConfiguration::SetTunerPowerState( CI2CScript *	pCScript,
											  BOOL			bPowerState)
{
	UCHAR		uchORMask = 0x0;
	UCHAR		uchANDMask = 0xFF;
	UCHAR		uchPinsMask, uchValue;
	BOOL		bResult;
	GPIOControl	gpioAccessBlock;

	switch( m_usTunerPowerConfiguration)
	{
		case ATI_TUNER_POWER_CONFIG_1:

			if( bPowerState)
				uchANDMask &= 0xF7;
			else
				uchORMask |= 0x08;
			break;

		default :
			return( FALSE);
	}

	// use GPIO interface to turn Tuner power ON / OFF
	bResult = FALSE;

	ENSURE 
	{
		if(( m_gpioProviderInterface.gpioOpen == NULL) ||
			( m_gpioProviderInterface.gpioAccess == NULL))
			FAIL;

		uchPinsMask = TUNER_PM_PINS;				// use as a PinMask
		gpioAccessBlock.Pins = &uchPinsMask;
		gpioAccessBlock.Flags = GPIO_FLAGS_BYTE;
		gpioAccessBlock.nBytes = 1;
		gpioAccessBlock.nBufferSize = 1;
		gpioAccessBlock.AsynchCompleteCallback = NULL;

		// try to get GPIO Provider
		if( !LockGPIOProviderEx( &gpioAccessBlock))
			FAIL;

		uchPinsMask = TUNER_PM_PINS;				// use as a PinMask
		gpioAccessBlock.Command = GPIO_COMMAND_READ_BUFFER;
		gpioAccessBlock.Flags = GPIO_FLAGS_BYTE;
		gpioAccessBlock.dwCookie = m_dwGPIOAccessKey;
		gpioAccessBlock.nBytes = 1;
		gpioAccessBlock.nBufferSize = 1;
		gpioAccessBlock.Pins = &uchPinsMask;
		gpioAccessBlock.Buffer = &uchValue;
		gpioAccessBlock.AsynchCompleteCallback = NULL;

		if( !AccessGPIOProvider( m_pdoDriver, &gpioAccessBlock))
			FAIL;

		uchValue &= uchANDMask;
		uchValue |= uchORMask;

		gpioAccessBlock.Command = GPIO_COMMAND_WRITE_BUFFER;

		if( !AccessGPIOProvider( m_pdoDriver, &gpioAccessBlock))
			FAIL;

		bResult = TRUE;

	} END_ENSURE;

	// nothing bad will happen if we try to release the provider even we
	// have not obtained it at the first place
	uchValue = AUDIO_MUX_PINS;			// use as a PinMask
	gpioAccessBlock.Pins = &uchValue;
	gpioAccessBlock.Flags = GPIO_FLAGS_BYTE;
	gpioAccessBlock.nBytes = 1;
	gpioAccessBlock.nBufferSize = 1;
	gpioAccessBlock.AsynchCompleteCallback = NULL;
	
	ReleaseGPIOProvider( &gpioAccessBlock);

	return( bResult);
}



/*^^*
 *		ValidateConfigurationE2PROM
 * Purpose	: Checks the integrity ( check-sum) of I2C driven configuration EEPROM
 * Inputs	: CI2CScript * pCScript	: pointer to the I2C Provider class
 *
 * Outputs	: BOOL, TRUE if the information inside EEPROM is valid
 * Author	: TOM
 *^^*/
BOOL CATIHwConfiguration::ValidateConfigurationE2PROM( CI2CScript * pCScript)
{
	I2CPacket	i2cPacket;
	UCHAR		uchReadValue, uchWriteValue, uchCheckSum;
	UINT		nIndex;
	BOOL		bResult = ( BOOL)m_usE2PROMValidation;

	if( m_usE2PROMValidation == ( USHORT)-1)
	{
		// the validation has not been done yet.
		bResult = FALSE;

		ENSURE
		{
			// Let's always start from byte 0.
			i2cPacket.uchChipAddress = AIWPRO_CONFIGURATIONE2PROM_ADDRESS;
			i2cPacket.cbWriteCount = 1;
			i2cPacket.cbReadCount = 1;
			i2cPacket.puchReadBuffer = &uchCheckSum;
			uchWriteValue = 0;
			i2cPacket.puchWriteBuffer = &uchWriteValue;
			i2cPacket.usFlags = I2COPERATION_READ | I2COPERATION_RANDOMACCESS;

			if( !pCScript->ExecuteI2CPacket( &i2cPacket))
				FAIL;

			for( nIndex = 1; nIndex < AIWPRO_CONFIGURATIONE2PROM_LENGTH; nIndex ++)
			{
				// let's use auto-increment address mode
				i2cPacket.usFlags = I2COPERATION_READ;
				i2cPacket.cbWriteCount = 0;
				i2cPacket.puchWriteBuffer = NULL;
				i2cPacket.puchReadBuffer = &uchReadValue;

				if( !pCScript->ExecuteI2CPacket( &i2cPacket))
					FAIL;

				uchCheckSum ^= uchReadValue;
			}

			if( nIndex != AIWPRO_CONFIGURATIONE2PROM_LENGTH)
				FAIL;

			bResult = ( uchCheckSum == 0);


		} END_ENSURE;

		m_usE2PROMValidation = ( USHORT)bResult;
	}

	return( bResult);
}



/*^^*
 *		ReadConfigurationE2PROM
 * Purpose	: Reads a single byte from I2C driver configuration EEPROM by offset
 * Inputs	: CI2CScript * pCScript	: pointer to the I2C Provider class
 *			  ULONG ulOffset		: byte offset within the EEPROM
 *			  PUCHAR puchValue		: pointer to the buffer to read into
 *
 * Outputs	: BOOL, TRUE if I2C read operation succeeded
 * Author	: TOM
 *^^*/
BOOL CATIHwConfiguration::ReadConfigurationE2PROM( CI2CScript * pCScript, ULONG ulOffset, PUCHAR puchValue)
{
	I2CPacket	i2cPacket;
	UCHAR		uchReadValue, uchWriteValue;

	ENSURE
	{
		if( ulOffset >= AIWPRO_CONFIGURATIONE2PROM_LENGTH)
			FAIL;

		uchWriteValue = ( UCHAR)ulOffset;
		i2cPacket.uchChipAddress = AIWPRO_CONFIGURATIONE2PROM_ADDRESS;
		i2cPacket.cbWriteCount = 1;
		i2cPacket.cbReadCount = 1;
		i2cPacket.puchReadBuffer = &uchReadValue;
		i2cPacket.puchWriteBuffer = &uchWriteValue;
		i2cPacket.usFlags = I2COPERATION_READ | I2COPERATION_RANDOMACCESS;

		if( !pCScript->ExecuteI2CPacket( &i2cPacket))
			FAIL;

		* puchValue = uchReadValue;

		return( TRUE);

	} END_ENSURE;

	return( FALSE);
}

