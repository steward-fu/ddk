#include <stdarg.h>
#include <stdio.h>

#include <vdw.h>
#include <ntddkbd.h>

#include "wmilib.h"
//
// Define the default number of elements in the class input data queue.
//

#define DATA_QUEUE_SIZE 100
#define MAXIMUM_PORTS_SERVICED 10
#define NAME_MAX 256
#define DUMP_COUNT 4
#define DEFAULT_DEBUG_LEVEL 0

#define MAX(a,b) (((a) < (b)) ? (b) : (a))

#if DBG
#define KbdPrint(x) KbdDebugPrint x
#else
#define KbdPrint(x)
#endif

#define KEYBOARD_POWER_LIGHT_TIME L"PowerLightTime"
#define KEYBOARD_WAIT_WAKE_ENABLE L"WaitWakeEnabled"
#define KEYBOARD_ALLOW_DISABLE L"AllowDisable"

#define IS_TRUSTED_FILE_FOR_READ(x) (&DriverEntry == (x)->FsContext2)
#define SET_TRUSTED_FILE_FOR_READ(x) ((x)->FsContext2 = &DriverEntry)
#define CLEAR_TRUSTED_FILE_FOR_READ(x) ((x)->FsContext2 = NULL)

#define ALLOW_OVERFLOW TRUE

//
// Port description
//
// Used only with the
// allforoneandoneforall turned off (AKA ConnectOneClassToOnePort
// turned on).  This is the file sent to the ports.
//
typedef struct _PORT {
    //
    // The file Pointer to the port;
    //
    PFILE_OBJECT    File;

    //
    // The port itself
    //
    struct _DEVICE_EXTENSION * Port;

    //
    // Port flags
    //
    BOOLEAN     Enabled;
    BOOLEAN     Reserved [2];
    BOOLEAN     Free;
} PORT, *PPORT;

#define PORT_WORKING(port) ((port)->Enabled && !(port)->Free)

//
// Class device extension.
//
typedef struct _DEVICE_EXTENSION {

    //
    // Back pointer to the Device Object created for this port.
    //
    PDEVICE_OBJECT  Self;

    //
    // Pointer to the active Class DeviceObject;
    // If the AFOAOFA (all for one and one for all) switch is on then this
    // points to the device object named as the first keyboard.
    //
    PDEVICE_OBJECT  TrueClassDevice;

    //
    // The Target port device Object to which all IRPs are sent.
    //
    PDEVICE_OBJECT  TopPort;

    //
    // The PDO if applicable.
    //
    PDEVICE_OBJECT  PDO;

    //
    // A remove lock to keep track of outstanding I/Os to prevent the device
    // object from leaving before such time as all I/O has been completed.
    //
    IO_REMOVE_LOCK  RemoveLock;

    //
    // If this port a Plug and Play port
    //
    BOOLEAN         PnP;
    BOOLEAN         Started;
    BOOLEAN         AllowDisable;

    //
    // A lock to say if we have responded true to the a query power setting.
    // if we have then we must succeed a transition to the lower power state.
    // The lock must prevent any driver activity which would prevent going to
    // a lower power state.
    //
    BOOLEAN PowerQueryLock;

    //
    // Registry settable Parameters
    //
    ULONG           PowerLightTime;

    //
    // Flag that indicates whether there is currently a request held pending
    // by StartIo.
    //
    BOOLEAN RequestIsPending;

    //
    // Flag that indicates whether the cleanup routine has been initiated.
    //
    BOOLEAN CleanupWasInitiated;

    //
    // Indicates whether it is okay to log overflow errors.
    //
    BOOLEAN OkayToLogOverflow;

    //
    // Indicates whether it is okay to send wait wake irps down the stack
    // (does NOT reflect if the bus can implement or not)
    //
    BOOLEAN WaitWakeEnabled;

    KSPIN_LOCK WaitWakeSpinLock;

    //
    // Is the Trusted Subsystem Connected
    //
    ULONG TrustedSubsystemCount;

    //
    // Number of input data items currently in the InputData queue.
    //
    ULONG InputCount;

    //
    // A Unicode string pointing to the symbolic link for the Device Interface
    // of this device object.
    //
    UNICODE_STRING  SymbolicLinkName;

    //
    // Start of the class input data queue (really a circular buffer).
    //
    PKEYBOARD_INPUT_DATA InputData;

    //
    // Insertion pointer for InputData.
    //
    PKEYBOARD_INPUT_DATA DataIn;

    //
    // Removal pointer for InputData.
    //
    PKEYBOARD_INPUT_DATA DataOut;

    //
    // Keyboard attributes.
    //
    KEYBOARD_ATTRIBUTES  KeyboardAttributes;

    //
    // A saved state of indicator lights
    //
    KEYBOARD_INDICATOR_PARAMETERS   IndicatorParameters;

    //
    // Spinlock used to synchronize access to the input data queue and its
    // insertion/removal pointers.
    //
    KSPIN_LOCK SpinLock;

    //
    // Mutex used to make sure that only one cleanup routine runs at a time.
    //
    FAST_MUTEX CleanupMutex;

    //
    // Request sequence number (used for error logging).
    //
    ULONG SequenceNumber;

    //
    // The "D" and "S" states of the current device
    //
    DEVICE_POWER_STATE DeviceState;
    SYSTEM_POWER_STATE SystemState;

    ULONG UnitId;

    //
    // WMI Information
    //
    WMILIB_CONTEXT WmiLibInfo;

    //
    // Mapping of system to device states when a wait wake irp is active
    //
    DEVICE_POWER_STATE SystemToDeviceState[PowerSystemHibernate];

    //
    // Minimum amount of power needed to wake the device
    //
    DEVICE_POWER_STATE MinDeviceWakeState;

    //
    // Lowest system state that the machine can be in and have the device wake it up
    //
    SYSTEM_POWER_STATE MinSystemWakeState;

    //
    // Actual wait wake irp
    //
    PIRP WaitWakeIrp;

    //
    // A pre allocated error log entry for Wait Wake Irps
    //
    PIO_ERROR_LOG_PACKET WWErrorLog;

    //
    // Target Device Notification Handle
    //
    PVOID TargetNotifyHandle;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


//
// Global shared data
//

typedef struct _GLOBALS {
    //
    // Declare the global debug flag for this driver.
    //
    ULONG   Debug;

    //
    // If ConnectOneClassToOnePort is off aka we want "All for one and one for
    // all behavior" then we need to create the one Master DO to which all
    // the goods go.
    //
    PDEVICE_EXTENSION   GrandMaster;

    //
    // List of ClassDevices that associated with the same name
    // aka the all for one and one for all flag is set
    //
    PPORT       AssocClassList;
    ULONG       NumAssocClass;
    LONG        Opens;
    ULONG       NumberLegacyPorts;
    FAST_MUTEX  Mutex;

    //
    // Specifies the type of class-port connection to make.  A '1'
    // indicates a 1:1 relationship between class device objects and
    // port device objects.  A '0' indicates a 1:many relationship.
    //
    ULONG ConnectOneClassToOnePort;

    //
    // When kbdclass receives an output command (EG set LEDs) this flag
    // instructs it to transmit that command to all attached ports,
    // regardless of the unit ID which was specified.
    //
    ULONG SendOutputToAllPorts;

    //
    // Number of port drivers serviced by this class driver.
    //
    ULONG PortsServiced;

    //
    //
    // IntialDevice Extension
    //
    DEVICE_EXTENSION    InitExtension;

    //
    // A list of the registry path to the service parameters.
    //
    KUstring            MyRegistryPath;
	//
    // The base name for all class objects created as mice.
    //
    
	KUstring            MyBaseClassName;

} GLOBALS, *PGLOBALS;

#include <hidclass.h>

#include <initguid.h>
#include <kbdmou.h>
#include <kbdlog.h>
#include "kbdclass.h"
#include <poclass.h>
#include <wmistr.h>

#define INITGUID
#include "wdmguid.h"

