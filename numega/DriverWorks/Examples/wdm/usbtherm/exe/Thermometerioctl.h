// Thermometerioctl.h
//
// Define control codes for Thermometer driver
//

#ifndef __Thermometerioctl__h_
#define __Thermometerioctl__h_

#define THERMOMETER_READ_DATA CTL_CODE(0,1,METHOD_BUFFERED, 0)
#endif
