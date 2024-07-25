DWORD VCom1394ClassInstaller(DI_FUNCTION         InstallFunction,
                             HDEVINFO            DeviceInfoSet,
                             PSP_DEVINFO_DATA    DeviceInfoData);

HANDLE OpenByInterface(GUID* pClassGuid, DWORD instance, PDWORD pError);
