/**
 @page USBD_HID_Demo

 @verbatim
 * @file    USBD/HID_Demo/readme.txt
 * @version V1.00
 * @date    2014-06-30
 * @brief   Description of USB Device HID example.
 @endverbatim

@par Example Description:

The USB Device HID example code which shows how to use HT32 USB Device Library, enumeration,
Endpoint interrupt transfer, and HID Get/Set Report. This example simply loopback the HID
output report to the input report. Besides, the set feature report also loopback to the get
feature report. The data flow is as below.
<PRE>
  Host                    Device
  ==========              ==========
  Output Report       ->  Save to gOutputReportBuffer and copy to gInputReportBuffer
  Input Report        <-  Transmit data from gInputReportBuffer
  Set Feature Report  ->  Save to gFeatureReportBuffer
  Get Feature Report  <-  Transmit data from gFeatureReportBuffer
</PRE>

@par Directory Contents:

- USBD/HID_Demo/main.c                                    Main program
- USBD/HID_Demo/ht32fxxxx_it.c                            Interrupt handlers
- USBD/HID_Demo/ht32fxxxx_conf.h                          Firmware library configuration file
- USBD/HID_Demo/system_ht32fxxxx.c                        System configuration file
- USBD/HID_Demo/ht32fxxxx_usbdconf.h                      USB Device Library configuration file
- USBD/HID_Demo/ht32_usbd_class.c                         USB Device Class related function
- USBD/HID_Demo/ht32_usbd_class.h                         Header file of the USB Device Class
- USBD/HID_Demo/ht32_usbd_descriptor.c                    Descriptor of USB Device
- USBD/HID_Demo/ht32_usbd_descriptor.h                    Header file of the USB Device Descriptor
- USBD/HID_Demo/CoIDE_HID_Demo/CoIDE_HID_Demo.cob         Project file of CoIDE
- USBD/HID_Demo/CoIDE_HID_Demo/startup_ht32fxxxx.c        Startup file of COIDE
- USBD/HID_Demo/EWARM/Project.eww                         Project file of EWARM
- USBD/HID_Demo/EWARM/startup_ht32fxxxx.s                 Startup file of EWARM
- USBD/HID_Demo/EWARM/ht32fxxxx_flash.icf                 Link script file of EWARM
- USBD/HID_Demo/MDK_ARM/Project.uvproj                    Project file of MDK-ARM
- USBD/HID_Demo/MDK_ARM/startup_ht32fxxxx.s               Startup file of MDK-ARM
- USBD/HID_Demo/SourceryG++Lite/Project.uvproj            Project file of SourceryG++ Lite
- USBD/HID_Demo/SourceryG++Lite/startup_ht32fxxxx.s       Startup file of SourceryG++ Lite
- USBD/HID_Demo/SourceryG++Lite/ht32fxxxx.ld              Link script file of SourceryG++ Lite

@par Hardware and Software Environment:

This example can be run on HT32 Series development board. This example uses the following endpoints.
<PRE>
  Endpoint    Transfer Type         Buffer Length
  ========    ====================  ==============
  0           Control               64 bytes
  1           Interrupt IN          64 bytes
  2           Interrupt OUT         64 bytes
</PRE>

Please create a project according to the project_template and add the following extra source files into it.
- ht32_usbd_class.c
- ht32_usbd_descriptor.c

@par Low Power mode:

For the convenient during debugging and evaluation stage, the USBDCore_LowPower() in the main.c is map to
a null function by default. In the real product, you must map this function to the low power function of
firmware library by setting USBDCORE_ENABLE_LOW_POWER as 1 (in the ht32fxxxx_usbdconf.h file).

@par USB Library debug mode:

For debug purpose, your can turn on the USB Library debug mode by setting "USBDCORE_DEBUG" definition
value as 1 (which located at ht32_usbd_core.h). The debug message will be shown via the printf function
to the UART or ITM interface according to the retarget setting of firmware library.

NOTE THAT USB DEBUG MODE DEGRADES THE PERFORMANCE AND MAY CAUSE USB ERROR UNDER SOME CONDITIONS.

You shall turn off the USB debug mode on your final application. We strongly recommend turn off the
"Dump USB Debug data (USBDCORE_DEBUG_DATA)" option when you enable the USB debug mode and retarget the
message to UART.

@par Firmware Disclaimer Information

1. The customer hereby acknowledges and agrees that the program technical documentation, including the
   code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
   proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
   other intellectual property laws.

2. The customer hereby acknowledges and agrees that the program technical documentation, including the
   code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
   other than HOLTEK and the customer.

3. The program technical documentation, including the code, is provided "as is" and for customer reference
   only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
   the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
   the warranties of merchantability, satisfactory quality and fitness for a particular purpose.

 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 */
