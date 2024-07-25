// VCom1394Common.h - shared structs for VCOM1394

typedef struct _VCOM1394_OHCI_ASYNC_PACKET {

    USHORT              OHCI_Reserved3:4;      
    USHORT              OHCI_tCode:4;        
    USHORT              OHCI_rt:2;            
    USHORT              OHCI_tLabel:6;        

    union {
        struct {
           NODE_ADDRESS        OHCI_Destination_ID; // 1st quadlet
        } Rx;

        struct {
            USHORT              OHCI_spd:3;         // 1st quadlet
            USHORT              OHCI_Reserved2:4;
            USHORT              OHCI_srcBusId:1;
            USHORT              OHCI_Reserved:8;          
        } Tx;
    } u;
    
    union {
        USHORT          OHCI_Offset_High;     

        struct {
            USHORT      OHCI_Reserved2:8;
            USHORT      OHCI_Reserved1:4;
            USHORT      OHCI_Rcode:4;
        } Response;
    } u2;

    union {
        struct {
            NODE_ADDRESS        OHCI_Destination_ID;// 2nd quadlet
        } Tx;

        struct {
            NODE_ADDRESS        OHCI_Source_ID;     // 2nd quadlet
        } Rx;
    } u1;
    
    ULONG               OHCI_Offset_Low;            // 3rd quadlet

    union {
        struct {
            USHORT      OHCI_Extended_tCode;  
            USHORT      OHCI_Data_Length;           // 4th quadlet
        } Block;

        ULONG           OHCI_Quadlet_Data;          // 4th quadlet
    } u3;
} VCOM1394_OHCI_ASYNC_PACKET, *PVCOM1394_OHCI_ASYNC_PACKET;

// response packet structure
typedef struct _VCOM1394_RESPONSE {
    KEVENT                      ResponseEvent;
    VCOM1394_OHCI_ASYNC_PACKET  ResponsePacket;
    VCOM_PORT                   PortInfo[64];
    LIST_ENTRY                  ListEntry;
} VCOM1394_RESPONSE, *PVCOM1394_RESPONSE;


class VCom1394Bus;

class VCom1394BusAsyncClient : public K1394AsyncClient
{
public:
    VOID SetParent(KPnpDevice *pParent) { m_pParent = pParent; }

    ULONG GetRequestPacketSpeed(PVCOM1394_OHCI_ASYNC_PACKET pRequestPacket);

    KPnpDevice            *m_pParent;
};

/////////////////////////////////////////////////////////////////////////////
// VCom1394BusAsyncClient::GetRequestPacketSpeed - gets the speed of a 
//          request packet
//
inline ULONG VCom1394BusAsyncClient::GetRequestPacketSpeed(PVCOM1394_OHCI_ASYNC_PACKET pRequestPacket)
{
    ULONG   step = 0;       // index used for speed calculation
    ULONG   temp;

    // make sure we have a valid packet
    ASSERT(pRequestPacket != NULL);

    // check for quadlet or block request.
    switch (pRequestPacket->OHCI_tCode) {
      case TCODE_WRITE_REQUEST_QUADLET:
      case TCODE_READ_REQUEST_BLOCK:
        step = 5 * sizeof(ULONG);
        break;
      case TCODE_WRITE_REQUEST_BLOCK:
        step = pRequestPacket->u3.Block.OHCI_Data_Length + (5 * sizeof(ULONG));
        break;
      case TCODE_READ_REQUEST_QUADLET:
      case TCODE_SELFID:
        step = 4 * sizeof(ULONG);
        break;
      default:
        break;
    }

    // quad align the step
    if(step & 0x03) {
        step &= 0xFFFC;
        step +=4;
    }

    temp = *(PULONG)((PUCHAR)pRequestPacket + (step - 4));

    return ((ULONG)(temp >> 21) & 0x0007);
}
