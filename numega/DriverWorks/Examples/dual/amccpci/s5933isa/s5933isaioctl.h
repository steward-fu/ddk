// S5933Isaioctl.h
//
// Define control codes for S5933Isa driver
//

#ifndef __S5933Isaioctl__h_
#define __S5933Isaioctl__h_

#define S5933ISA_IOCTL_800_POST_MSG CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define S5933ISA_IOCTL_801_RETRIEVE_MSG CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define S5933ISA_IOCTL_802_READ_PASSTHRU CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define S5933ISA_IOCTL_803_DUMP_REGISTERS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Structure used to send or retrieve a message to the AMCC S5933
typedef struct _KS5933ISA_MAILBOX_MESSAGE {
	ULONG MessageValue;				// Message value
	ULONG MessageMailBox;			// Mailbox number (1-4)
} KS5933ISA_MAILBOX_MESSAGE, *PKS5933ISA_MAILBOX_MESSAGE;
        

// Structure used to read/write data through the pass-thru to the AMCC S5933
typedef struct _KS5933ISA_PASSTHRU {
	ULONG Data;						// Data to read
} KS5933ISA_PASSTHRU, *PKS5933ISA_PASSTHRU;


#endif
