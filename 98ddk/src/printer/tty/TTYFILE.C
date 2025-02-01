// TTYFILE.C

#pragma code_seg("_CODESEG")

#include "tty.h"

TTYDevice   Device                        = {0};
char        DriverFileName[PATHFILELEN]   = {0};
int         DeviceNumber                  =  0,
	    numdevices,
	    actdevice;
BOOL        DataFileUpdated = FALSE;
OFSTRUCT    os;

int FAR LoadTTYDataFile(VOID);
int FAR GetNumDevices(VOID);
int FAR GetCurDevice(VOID);
int FAR SetCurDevice(int act);
int FAR GetDevice(int number);
int FAR SetDevice(int number);
int FAR DeleteDevice(int number);
VOID FAR InitialiseNewTTYData(VOID);
int NEAR ConvertFileFormat(VOID);
VOID NEAR GetTTYDataFileDirectory(VOID);

int     fh = -1;

int FAR LoadTTYDataFile(VOID)
{
  int  fh, filever;
  char szMessage[MESSAGE_TEXT_LENGTH];
  char szCaption[CAPTION_TEXT_LENGTH];

  GetTTYDataFileDirectory(); 
  
  if ((fh = OpenFile(DriverFileName, &os, OF_EXIST)) != -1) // File exists
  {  
   int iModel;
   
   if ((iModel = GetCurDevice()) != DATAFILEERROR) // File is OK.
   {
    iModel = GetDevice(iModel); 
    if (iModel == DEVICEOK)    // We can read current device's data.
      return TRUE;
    
    else 
    if (hTTYInst != NULL)
    { 
     LoadString(hTTYInst, IDS_MSG_FILECORRUPT, (LPSTR)szMessage, sizeof(szMessage));
     LoadString(hTTYInst, IDS_MSG_CAPTION, (LPSTR)szCaption, sizeof(szCaption));
     
     if (MessageBox(NULL, (LPSTR)szMessage, (LPSTR)szCaption, MB_ICONEXCLAMATION | MB_OK) != IDOK)
       return FALSE;
     OpenFile(DriverFileName, &os, OF_DELETE);
    }  
   }
  
   if ((iModel == DATAFILEERROR) && (hTTYInst != NULL)) // File is not OK.
   {
    LoadString(hTTYInst, IDS_MSG_NORETAINDATA, (LPSTR)szMessage, sizeof(szMessage));
    LoadString(hTTYInst, IDS_MSG_CAPTION, (LPSTR)szCaption, sizeof(szCaption));
     
    if (MessageBox(NULL, (LPSTR)szMessage, (LPSTR)szCaption, MB_ICONINFORMATION | MB_OK) != IDOK)
      return FALSE;
    OpenFile(DriverFileName, &os, OF_DELETE);
   }
  } 
   // We can't create a new file.
   
   if (((fh = _lcreat(DriverFileName, 0)) == CREATE_ERROR) && (hTTYInst != NULL))
   {
    LoadString(hTTYInst, IDS_MSG_NOCREATE, (LPSTR)szMessage, sizeof(szMessage));
    LoadString(hTTYInst, IDS_MSG_CAPTION, (LPSTR)szCaption, sizeof(szCaption));
    return CREATE_FAILED;
   }  
    
  // New file created OK, display a message, write default data. 
  
   if (hTTYInst != NULL)
   {
    LoadString(hTTYInst, IDS_MSG_NEWTTYFILE, (LPSTR)szMessage, sizeof(szMessage));
    LoadString(hTTYInst, IDS_MSG_CAPTION, (LPSTR)szCaption, sizeof(szCaption));
     
    if (MessageBox(NULL, (LPSTR)szMessage, (LPSTR)szCaption, MB_ICONEXCLAMATION | MB_OK) != IDOK)
      return FALSE;
   }
   InitialiseNewTTYData(); // Fill new data file with NULL data - Use default model name

   if (hTTYInst != NULL)
	LoadString(hTTYInst, IDS_MSG_NEWMODELID, Device.DeviceName, MODELNAME);

   numdevices = actdevice = 1;
   filever = TTY_FILEVER;

   // Create a private header consisting of the number of devices supported,
   // the number of the currently selected device and the file version #.
   _lwrite(fh, (LPSTR)&numdevices, sizeof(int));
   _lwrite(fh, (LPSTR)&actdevice, sizeof(int));
   _lwrite(fh, (LPSTR)&filever, sizeof(int));

   if (_lwrite(fh, (LPSTR)&Device, sizeof(Device)) < sizeof(Device))
   {
    _lclose(fh);
    return WRITE_FAILED;
   }
   _lclose(fh);
 
  return CREATE_OK;
}

int FAR GetDevice(int number)
{        
  int  fd;
  LONG offs;

  if (number <=0)
    return DEVICENOTFOUND;
	
  if ((fd = OpenFile((LPSTR)DriverFileName,
		     (LPOFSTRUCT)&os,OF_READWRITE)) == -1)
    return OPEN_FAILED;
  
  if (_llseek(fd, 0L, SEEK_SET) == -1L)
  {
   _lclose(fd);
   return SEEK_FAILED;
  }
    
  if (_lread(fd, (LPSTR)&numdevices, sizeof(int)) < sizeof(int))
  {
   _lclose(fd);
   return READ_FAILED;
  }
    
  if (number > numdevices)
  {
   _lclose(fd);
   return DEVICENOTFOUND;
  }
    
  number--; // We have 3 private entries in the data file.
  offs = (LONG)(number * sizeof(Device) + PRIVATE_ENTRIES * sizeof(int));
    
  if (_llseek(fd, offs, SEEK_SET) == -1L)
  {
   _lclose(fd);
   return DATAFILEERROR;
  }
    
  if (_lread(fd, (LPSTR)&Device, sizeof(Device)) < sizeof(Device))
  {
   _lclose(fd);  // Can't read whole record for device
   return READ_FAILED;
  }
   
  if (_lclose(fd) == -1)
    return CLOSE_FAILED;

 return DEVICEOK;
}   

int FAR SetDevice(int number)
{
  int  fd;
  LONG offs;

  if ((fd = OpenFile(
      (LPSTR)DriverFileName, (LPOFSTRUCT)&os, OF_EXIST)) != -1)
  {
   if ((fd = OpenFile(
       (LPSTR)DriverFileName, (LPOFSTRUCT)&os, OF_READWRITE)) == -1)
      return DATAFILEERROR;
  }
  
  else
    
  if ((fd = OpenFile((LPSTR)DriverFileName,
		       (LPOFSTRUCT)&os, OF_CREATE|OF_READWRITE)) == -1)
    return CREATE_FAILED;
	    
  if (_llseek(fd, NUMDEVICES, SEEK_SET)== -1L)
  {
   _lclose(fd);
   return SEEK_FAILED;
  }
    
  if (_lread(fd, (LPSTR)&numdevices, sizeof(int)) < sizeof(int))
  {
   _lclose(fd);
   return READ_FAILED;
  }
    
  if (number > numdevices)
  {
   _llseek(fd, NUMDEVICES, SEEK_SET);
   numdevices = number; // ds!=ss 
   _lwrite(fd, (LPSTR)&numdevices, sizeof(int));
  }
    
  number--;
  offs = (LONG)(number * sizeof(Device) + PRIVATE_ENTRIES * sizeof(int));
    
  if (_llseek(fd, offs, SEEK_SET) == -1L)
  {
   _lclose(fd);
   return SEEK_FAILED;
  }
    
  if (_lwrite(fd, (LPSTR)&Device, sizeof(Device)) < sizeof(Device))
  {
   _lclose(fd);
   return WRITE_FAILED;
  }
    
  if (_lclose(fd) == -1)
    return CLOSE_FAILED;
      
 return DEVICEOK;
}

int FAR GetCurDevice()
{
  LONG lOffset, lSeekResult;
  WORD buffer;
  
  if ((fh = OpenFile((LPSTR)DriverFileName,
		     (LPOFSTRUCT)&os,OF_READWRITE)) == -1)
    return OPEN_FAILED;

  // Check to see if we're dealing with a 3.x TTY.DAT file.
  // If so, display a message that we're going to copy the old
  // strings over to the new file.
  
  if (!DataFileUpdated) // This code will be removed once working OK.
  {
   lOffset = (LONG)((PRIVATE_ENTRIES -1 ) * sizeof(WORD));
   lSeekResult = _llseek(fh, lOffset, SEEK_SET);
   _lread(fh, (LPSTR)&buffer, sizeof(WORD));
    
   if (buffer != TTY_FILEVER) // Need to update 3.1 data file to 4.0 format
   {
    ConvertFileFormat();
    DataFileUpdated = TRUE;
   }
  }
  
  if (_llseek(fh, ACTDEVICE, SEEK_SET) == -1L)
  {
   _lclose(fh);
   return SEEK_FAILED;
  }
    
  if (_lread(fh, (LPSTR)&actdevice, sizeof(int)) < sizeof(int))
  {
   _lclose(fh);
   return READ_FAILED;
  }

  if (_lclose(fh) == -1)
    return CLOSE_FAILED;
      
 return actdevice;
}       

int FAR SetCurDevice(int act)
{
  int fd;
  actdevice = act;   // if ds != ss

  if ((fd = OpenFile((LPSTR)DriverFileName, (LPOFSTRUCT)&os, OF_EXIST)) != -1)
  { 
   if ((fd = OpenFile((LPSTR)DriverFileName,
		      (LPOFSTRUCT)&os, OF_READWRITE)) == -1)
     return READ_FAILED;
  }
   
  else
    
  if ((fd = OpenFile((LPSTR)DriverFileName,
		     (LPOFSTRUCT)&os, OF_CREATE | OF_READWRITE)) == -1)
    return CREATE_FAILED;
	    
  if (_llseek(fd, ACTDEVICE, SEEK_SET) == -1L)
  {
   _lclose(fd);
   return SEEK_FAILED;
  }
    
  if (_lwrite(fd, (LPSTR)&actdevice, sizeof(int)) < sizeof(int))
  {
   _lclose(fd);
   return WRITE_FAILED;
  }
    
  if (_lclose(fd) == -1)
    return CLOSE_FAILED;
	
 return DEVICEOK;
}       

int FAR GetNumDevices()
{
  int     fd;

  if ((fd = OpenFile((LPSTR)DriverFileName,
		      (LPOFSTRUCT)&os, OF_READWRITE)) == -1)
    return OPEN_FAILED;
	
  if (_llseek(fd, NUMDEVICES, SEEK_SET)== -1L)
  {
   _lclose(fd);
   return SEEK_FAILED;
  }
    
  if (_lread(fd, (LPSTR)&actdevice, sizeof(int)) < sizeof(int))
  {
   _lclose(fd);
   return READ_FAILED;
  }
    
  if (_lclose(fd) == -1)
    return CLOSE_FAILED;
	
  return actdevice;
}   

int FAR DeleteDevice(int number)
{
  int          fd;
  HGLOBAL      hglb;
  LONG         offset;
  WORD         readed;
  LPSTR        lpBuf;
  static DWORD dwStructSize = 0;

  if (number <=0)
    return DEVICENOTFOUND;

  if ((fd = OpenFile((LPSTR)DriverFileName,
		     (LPOFSTRUCT)&os, OF_READWRITE)) == -1)
    return OPEN_FAILED;
	
  if (_llseek(fd, NUMDEVICES, SEEK_SET)== -1L)
  {
   _lclose(fd);
   return SEEK_FAILED;
  }

  if (_lread(fd, (LPSTR)&numdevices, sizeof(int)) < sizeof(int))
  {
   _lclose(fd);
   return READ_FAILED;
  }

  if (number > numdevices)
  {
   _lclose(fd);
   return DEVICENOTFOUND;
  }

  if (number < numdevices)
  {
   offset = (LONG)( number * sizeof(Device) + PRIVATE_ENTRIES * sizeof(int));
   dwStructSize = (numdevices - number) * sizeof(Device);      
   
   for (hglb = NULL; dwStructSize >= sizeof(Device); dwStructSize /= 2 )
      if (hglb = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwStructSize))
	break;

   if (!hglb)
   {
    _lclose(fd);
    return DATAFILEERROR;
   }

   lpBuf = GlobalLock(hglb);

   do
   {
    if (_llseek(fd, offset, SEEK_SET) == -1L)
    {
     _lclose(fd);
     GlobalUnlock(hglb);
     GlobalFree(hglb);
     return SEEK_FAILED;
    }

    if ((readed = _lread(fd, lpBuf, (unsigned)dwStructSize)) <= 0)
      break;

    if (_llseek(fd, offset-sizeof(Device), SEEK_SET) == -1L)
    {
     _lclose(fd);
     GlobalUnlock(hglb);
     GlobalFree(hglb);
     return DATAFILEERROR;
    }

    if (_lwrite(fd, lpBuf, readed) < readed )
    {
     _lclose(fd);
     GlobalUnlock(hglb);
     GlobalFree(hglb);
     return WRITE_FAILED;
    }

    offset += dwStructSize;

   } while(TRUE);

   GlobalUnlock(hglb);
   GlobalFree(hglb);
  }

  if (_llseek(fd, NUMDEVICES, SEEK_SET)== -1L)
  {
   _lclose(fd);
   return SEEK_FAILED;
  }

  numdevices--;

  if (_lwrite(fd, (LPSTR)&numdevices, sizeof(int)) < sizeof(int))
  {
   _lclose(fd);
   return WRITE_FAILED;
  }

  if (_lclose(fd) == -1)
    return CLOSE_FAILED;

  return DEVICEOK;
}  

VOID NEAR  GetTTYDataFileDirectory(VOID)    
{
  
  GetWindowsDirectory((LPSTR)DriverFileName, PATHFILELEN);

  // Append '\', if the Windows dir. isn't the root.
  if ('\\' != DriverFileName[lstrlen((LPSTR)DriverFileName) - 1])
    lstrcat((LPSTR)DriverFileName, (LPSTR) "\\");

  // Append datafile name to the returned Windows path
  lstrcat((LPSTR)DriverFileName, (LPSTR)TTYDATAFILE); 
}

VOID FAR InitialiseNewTTYData(VOID)
{    
  int i;

  // We create a default table for one Device.
  
  Device.DeviceData.CodePage        = RES_DB_CUSTOM;
  Device.DeviceData.BeginJob[0]     = '\0';
  Device.DeviceData.EndJob[0]       = '\0';
  Device.DeviceData.PaperSize[0]    = '\0';
  Device.DeviceData.PaperSource[0]  = '\0';
  Device.DeviceData.f10Pitch[0]      = '\0';
  Device.DeviceData.f12Pitch[0]      = '\0';
  Device.DeviceData.f17Pitch[0]      = '\0';
  Device.DeviceData.DoubleOn[0]     = '\0';
  Device.DeviceData.DoubleOff[0]    = '\0';
  Device.DeviceData.BoldOn[0]       = '\0';
  Device.DeviceData.BoldOff[0]      = '\0';
  Device.DeviceData.UnderlineOn[0]  = '\0';
  Device.DeviceData.UnderlineOff[0] = '\0';
   
  for (i = 0; i <= HI_ANSIEND-HI_ANSISTART; i++)
  {
   Device.CTTCode[i][0] = DefaultTTYChars[i];
   Device.CTTCode[i][1] = '\0';
  } 

}

// Function to copy the 3.x data file ESCapes to 4.0 file format.
// We don't copy the CTT escapes yet.
// Placeholder - dblspace corrupted the old file so I lost all the nice
//  tidy code that was this function. i.e. no low level DOS calls.

int NEAR ConvertFileFormat(VOID)
{
  char szMessage[MESSAGE_TEXT_LENGTH];
  char szCaption[CAPTION_TEXT_LENGTH];
  BYTE Modelbuffer[OLDMODELNAMELEN + 1], ESCbuffer [OLDESCLEN + 1]; 
  int i, filever;
   
   if (hTTYInst != NULL)
   {
    LoadString(hTTYInst, IDS_MSG_OLDTTYFILE, (LPSTR)szMessage, sizeof(szMessage));
    LoadString(hTTYInst, IDS_MSG_CAPTION, (LPSTR)szCaption, sizeof(szCaption));
   
    if (MessageBox(NULL, (LPSTR)szMessage, (LPSTR)szCaption, MB_ICONEXCLAMATION | MB_OK) != IDOK)
      return FALSE;
   }
  // 1st - Open 3.x .dat file, read the current model and number of models
  // in the data file.
  // Set up a counter which will copy the escapes into each of the data
  // structures and write out the entire file.
  
  if ((fh = OpenFile((LPSTR)DriverFileName,
		     (LPOFSTRUCT)&os, OF_READWRITE)) == -1)
    return OPEN_FAILED;
  
  if (_llseek(fh, NUMDEVICES, SEEK_SET)== -1L)
  {
   _lclose(fh);
   return SEEK_FAILED;
  }

  if (_lread(fh, (LPSTR)&numdevices, sizeof(int)) < sizeof(int))
  {
   _lclose(fh);
   return READ_FAILED;
  }
  
  if (_llseek(fh, ACTDEVICE, SEEK_SET) == -1L)
  {
   _lclose(fh);
   return SEEK_FAILED;
  }
    
  if (_lread(fh, (LPSTR)&actdevice, sizeof(int)) < sizeof(int))
  {
   _lclose(fh);
   return READ_FAILED;
  }
  
  // Now we have the number of models and active model in the data file
  // Seek to the beginning of the Model Name
  
  if (_llseek(fh, sizeof(DWORD), SEEK_SET)== -1L)
  {
   _lclose(fh);
   return SEEK_FAILED;
  }

  // We're now at the first letter of the Model name which is 16 BYTES
  // long including the NULL terminator.
  
  _lread(fh, (LPBYTE)&Modelbuffer, OLDMODELNAMELEN + 1);
  _fmemcpy((LPBYTE)(&(Device.DeviceName)), Modelbuffer, OLDMODELNAMELEN + 1);
  _llseek(fh, sizeof(WORD), SEEK_CUR); // Move ptr 2 bytes (ptr to carraige option)
  Device.DeviceData.CodePage        = RES_DB_CUSTOM;
  
  _lread(fh, (LPBYTE)&ESCbuffer, OLDESCLEN + 1);
  _fmemcpy((LPBYTE)(&(Device.DeviceData.BeginJob)), ESCbuffer, OLDESCLEN + 1);
  ESCbuffer[0] = '\0';
  Device.DeviceData.EndJob[0]       = '\0';
  Device.DeviceData.PaperSize[0]    = '\0';
  Device.DeviceData.PaperSource[0]  = '\0';
  _lread(fh, (LPSTR)&ESCbuffer, OLDESCLEN + 1); 
  _fmemcpy((LPBYTE)(&(Device.DeviceData.f10Pitch)), ESCbuffer, OLDESCLEN + 1);
  ESCbuffer[0] = '\0';
  _lread(fh, (LPSTR)&ESCbuffer, OLDESCLEN + 1);    
  _fmemcpy((LPBYTE)(&(Device.DeviceData.f12Pitch)), ESCbuffer, OLDESCLEN + 1);
  ESCbuffer[0] = '\0';
  _lread(fh, (LPSTR)&ESCbuffer, OLDESCLEN + 1);
  _fmemcpy((LPBYTE)(&(Device.DeviceData.f17Pitch)), ESCbuffer, OLDESCLEN + 1);
  ESCbuffer[0] = '\0';
  _lread(fh, (LPSTR)&ESCbuffer, OLDESCLEN + 1);
  _fmemcpy((LPBYTE)(&(Device.DeviceData.DoubleOn)), ESCbuffer, OLDESCLEN + 1);
  ESCbuffer[0] = '\0';
  _lread(fh, (LPSTR)&ESCbuffer, OLDESCLEN + 1);
  _fmemcpy((LPBYTE)(&(Device.DeviceData.DoubleOff)), ESCbuffer, OLDESCLEN + 1);
  Device.DeviceData.BoldOn[0]       = '\0';
  Device.DeviceData.BoldOff[0]      = '\0';
  Device.DeviceData.UnderlineOn[0]  = '\0';
  Device.DeviceData.UnderlineOff[0] = '\0';
  
  // We don't copy over the CTT strings. Start with New ones.
  
  for (i = 0; i <= HI_ANSIEND-HI_ANSISTART; i++)
  {
   Device.CTTCode[i][0] = DefaultTTYChars[i];
   Device.CTTCode[i][1] = '\0';
  } 

  // We must close the file, delete it and create a new one.
  
  _lclose(fh);
  OpenFile(DriverFileName, &os, OF_DELETE); 
  if ((fh = _lcreat(DriverFileName, 0)) == CREATE_ERROR) 
  {
   LoadString(hTTYInst, IDS_MSG_NOCREATE, (LPSTR)szMessage, sizeof(szMessage));
   LoadString(hTTYInst, IDS_MSG_CAPTION, (LPSTR)szCaption, sizeof(szCaption));
   return CREATE_FAILED;
  }  
  

  // We have a new converted file - Mark the file as a 4.0 file.
  filever = TTY_FILEVER;

  // Create a private header consisting of the number of devices supported,
  // the number of the currently selected device and the file version #.
  _lwrite(fh, (LPSTR)&numdevices, sizeof(int));
  _lwrite(fh, (LPSTR)&actdevice, sizeof(int));
  _lwrite(fh, (LPSTR)&filever, sizeof(int));

  if (_lwrite(fh, (LPSTR)&Device, sizeof(Device)) < sizeof(Device))
  {
   _lclose(fh);
   return WRITE_FAILED;
  }
}   
  

