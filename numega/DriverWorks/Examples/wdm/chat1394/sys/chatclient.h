
#define MAX_ADDRESS_RANGE_BUFFERS 4
#define ADDRESS_RANGE_BUFFER_SIZE 0x200
#define MAX_READ_FIFO_SIZE 0x2000

class Chat1394Device;

class Chat1394Client : public K1394AsyncClient
{
public:
	VOID OnWrite(PMDL pMdl, ULONG Offset, ULONG Length, PADDRESS_FIFO pAddressFifo);
	NTSTATUS Initialize(K1394LowerDevice*	LowerDevice, Chat1394Device *DeviceObject);
	VOID Invalidate();
	~Chat1394Client(){Invalidate();}

protected:
	K1394AddressFifo	m_AddressFifo;
	Chat1394Device		*m_pDeviceObject;
};