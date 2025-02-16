/*++

Copyright (c) 1997-2000  Microsoft Corporation All Rights Reserved

Module Name:

    wavtable.h

Abstract:

    Declaration of wave miniport tables.

--*/

#ifndef _MSVAD_WAVTABLE_H_
#define _MSVAD_WAVTABLE_H_

//=============================================================================
static
KSDATARANGE_AUDIO PinDataRangesStream[] =
{
    {
        {
            sizeof(KSDATARANGE_AUDIO),
            0,
            0,
            0,
            STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
            STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
            STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        },
        MAX_CHANNELS_PCM,           
        MIN_BITS_PER_SAMPLE_PCM,    
        MAX_BITS_PER_SAMPLE_PCM,    
        MIN_SAMPLE_RATE,            
        MAX_SAMPLE_RATE             
    },
};

static
PKSDATARANGE PinDataRangePointersStream[] =
{
    PKSDATARANGE(&PinDataRangesStream[0])
};

//=============================================================================
static
KSDATARANGE PinDataRangesBridge[] =
{
    {
        sizeof(KSDATARANGE),
        0,
        0,
        0,
        STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
        STATICGUIDOF(KSDATAFORMAT_SUBTYPE_ANALOG),
        STATICGUIDOF(KSDATAFORMAT_SPECIFIER_NONE)
    }
};

static
PKSDATARANGE PinDataRangePointersBridge[] =
{
    &PinDataRangesBridge[0]
};

//=============================================================================
static
PCPIN_DESCRIPTOR MiniportPins[] =
{
    // Wave In Streaming Pin (Capture) KSPIN_WAVE_CAPTURE_SINK
    {
        MAX_OUTPUT_STREAMS,
        MAX_OUTPUT_STREAMS,
        0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersStream),
            PinDataRangePointersStream,
            KSPIN_DATAFLOW_OUT,
            KSPIN_COMMUNICATION_SINK,
            &KSCATEGORY_AUDIO,
            &KSAUDFNAME_RECORDING_CONTROL,  
            0
        }
    },
    
    // Wave In Bridge Pin (Capture - From Topology) KSPIN_WAVE_CAPTURE_SOURCE
    {
        0,
        0,
        0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersBridge),
            PinDataRangePointersBridge,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_NONE,
            &KSCATEGORY_AUDIO,
            NULL,
            0
        }
    },
  
    // Wave Out Streaming Pin (Renderer) KSPIN_WAVE_RENDER_SINK
    {
        MAX_INPUT_STREAMS,
        MAX_INPUT_STREAMS, 
        0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersStream),
            PinDataRangePointersStream,
            KSPIN_DATAFLOW_IN,
            KSPIN_COMMUNICATION_SINK,
            &KSCATEGORY_AUDIO,
            &KSAUDFNAME_VOLUME_CONTROL,
            0
        }
    },
  
    // Wave Out Bridge Pin (Renderer) KSPIN_WAVE_RENDER_SOURCE
    {
        0,
        0,
        0,
        NULL,
        {
            0,
            NULL,
            0,
            NULL,
            SIZEOF_ARRAY(PinDataRangePointersBridge),
            PinDataRangePointersBridge,
            KSPIN_DATAFLOW_OUT,
            KSPIN_COMMUNICATION_NONE,
            &KSCATEGORY_AUDIO,
            NULL,
            0
        }
    },
};

//=============================================================================
static
PCPROPERTY_ITEM PropertiesVolume[] =
{
  {
    &KSPROPSETID_Audio,
    KSPROPERTY_AUDIO_VOLUMELEVEL,
    KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_BASICSUPPORT,
    PropertyHandler_Wave
  },
  {
    &KSPROPSETID_Audio,
    KSPROPERTY_AUDIO_CPU_RESOURCES,
    KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
    PropertyHandler_Wave
  }
};

DEFINE_PCAUTOMATION_TABLE_PROP(AutomationVolume, PropertiesVolume);

//=============================================================================
// TODO This needs more properties.

static
PCPROPERTY_ITEM PropertiesSuperMix[] =
{
  {
    &KSPROPSETID_Audio,
    KSPROPERTY_AUDIO_CPU_RESOURCES,
    KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
    PropertyHandler_Wave
  }
};

DEFINE_PCAUTOMATION_TABLE_PROP(AutomationSuperMix, PropertiesSuperMix);

//=============================================================================
// TODO This needs more properties.

static
PCPROPERTY_ITEM PropertiesSrc[] =
{
  {
    &KSPROPSETID_Audio,
    KSPROPERTY_AUDIO_SAMPLING_RATE,
    KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
    PropertyHandler_Wave
  },
  {
    &KSPROPSETID_Audio,
    KSPROPERTY_AUDIO_CPU_RESOURCES,
    KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
    PropertyHandler_Wave
  }
};

DEFINE_PCAUTOMATION_TABLE_PROP(AutomationSrc, PropertiesSrc);

//=============================================================================
static
PCPROPERTY_ITEM PropertiesSum[] =
{
  {
    &KSPROPSETID_Audio,
    KSPROPERTY_AUDIO_CPU_RESOURCES,
    KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
    PropertyHandler_Wave
  }
};

DEFINE_PCAUTOMATION_TABLE_PROP(AutomationSum, PropertiesSum);


//=============================================================================
static
PCNODE_DESCRIPTOR MiniportNodes[] =
{
    // KSNODE_WAVE_ADC
    {
        0,                      // Flags
        NULL,                   // AutomationTable
        &KSNODETYPE_ADC,        // Type
        NULL                    // Name
    },
    // KSNODE_WAVE_DAC
    {
        0,                      // Flags
        NULL,                   // AutomationTable
        &KSNODETYPE_DAC,        // Type
        NULL                    // Name
    },
    // KSNODE_WAVE_VOLUME1
    {
        0,                      // Flags
        &AutomationVolume,      // AutomationTable
        &KSNODETYPE_VOLUME,     // Type
        NULL                    // Name
    },
    // KSNODE_WAVE_SUPERMIX
    {
        0,                      // Flags
        &AutomationSuperMix,    // AutomationTable
        &KSNODETYPE_SUPERMIX,   // Type
        NULL                    // Name
    },
    // KSNODE_WAVE_VOLUME2
    {
        0,                      // Flags
        &AutomationVolume,      // AutomationTable
        &KSNODETYPE_VOLUME,     // Type
        NULL                    // Name
    },
    // KSNODE_WAVE_SRC
    {
        0,                      // Flags
        &AutomationSrc,         // AutomationTable
        &KSNODETYPE_SRC,        // Type
        NULL                    // Name
    },
    // KSNODE_WAVE_SUM
    {
        0,                      // Flags
        &AutomationSum,         // AutomationTable
        &KSNODETYPE_SUM,        // Type
        NULL                    // Name
    },
};


//=============================================================================
static
PCCONNECTION_DESCRIPTOR MiniportConnections[] =
{
    { PCFILTER_NODE,        KSPIN_WAVE_CAPTURE_SOURCE,  KSNODE_WAVE_ADC,     1 },    
    { KSNODE_WAVE_ADC,      0,                          PCFILTER_NODE,       KSPIN_WAVE_CAPTURE_SINK },    

    { PCFILTER_NODE,        KSPIN_WAVE_RENDER_SINK,     KSNODE_WAVE_VOLUME1, 1 },
    { KSNODE_WAVE_VOLUME1,  0,                          KSNODE_WAVE_SUPERMIX,1 },
    { KSNODE_WAVE_SUPERMIX, 0,                          KSNODE_WAVE_VOLUME2, 1 },
    { KSNODE_WAVE_VOLUME2,  0,                          KSNODE_WAVE_SRC,     1 },
    { KSNODE_WAVE_SRC,      0,                          KSNODE_WAVE_SUM,     1 },
    { KSNODE_WAVE_SUM,      0,                          KSNODE_WAVE_DAC,     1 },    
    { KSNODE_WAVE_DAC,      0,                          PCFILTER_NODE,       KSPIN_WAVE_RENDER_SOURCE },    
};

//=============================================================================
static
PCFILTER_DESCRIPTOR MiniportFilterDescriptor =
{
    0,                                  // Version
    NULL,                               // AutomationTable
    sizeof(PCPIN_DESCRIPTOR),           // PinSize
    SIZEOF_ARRAY(MiniportPins),         // PinCount
    MiniportPins,                       // Pins
    sizeof(PCNODE_DESCRIPTOR),          // NodeSize
    SIZEOF_ARRAY(MiniportNodes),        // NodeCount
    MiniportNodes,                      // Nodes
    SIZEOF_ARRAY(MiniportConnections),  // ConnectionCount
    MiniportConnections,                // Connections
    0,                                  // CategoryCount
    NULL                                // Categories - NULL->use defaults (AUDIO RENDER CAPTURE)
};

#endif

