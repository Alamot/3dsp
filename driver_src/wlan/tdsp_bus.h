 /***********************************************************************
  * Copyright(C) 2006 3DSP Corporation. All Rights Reserved.
  
  * FILENAME		:tdsp_bus.h         VERSION:1.0
  * CREATE DATE	:2009/03/11
  * PURPOSE:	
  *
  * AUTHORS:     
  *
  * DECLARATION:  This document contains confidential proprietary information that
  *               is solely for authorized personnel. It is not to be disclosed to
  *               any unauthorized person without prior written consent of 3DSP
  *               Corporation.		
  ***********************************************************************/
#define MAX_VERSIONID_LEN    64

#ifndef _TDSP_BUS_H_
#define _TDSP_BUS_H_

#define WB_PROXY_FLAG

void *Bus_new_interface(void *dev);
int Bus_openInterface(void *pIntf);
int  Bus_closeInterface(void *pIntf);

void Bus_set_firmware_version(void *pIntf, unsigned long c8051_version, unsigned long dsp_version);

unsigned char Bus_get_hotkey_flag(void *pIntf);
unsigned char Bus_get_antenna_flag(void *pIntf);

#endif
typedef struct _TDSPBUS_INTERFACE_DATA 
{
	// A back pointer to the bus
	void *ParentFdo;

	void *pPciDev; //pci_dev
	void *pBtDev; //PBT_DEVICE_EXT
	
	// An array of (zero terminated wide character strings).
	// The array itself also null terminated
	unsigned char version[MAX_VERSIONID_LEN]; // send to bus driver
	
	unsigned long c8051_version;

	unsigned long dsp_version;
	
	// Unique serail number of the device on the bus
	unsigned char SerialNo; // send to bus driver
	
	unsigned char ResetFlag;	// Reset Flag
	unsigned char ParaFlag;	/*from wb config.
			bit0~bit7
			bit0:		hotkey
			bit12:	antenna	1: 0      2: 1        other:auto
			*/
	
	// Device State
	unsigned long devstate; //set by bus driver.
	
	// Callback operation
	int (*open)(struct _TDSPBUS_INTERFACE_DATA *dev);
	int (*close)(struct _TDSPBUS_INTERFACE_DATA *dev);
	int (*suspend)(struct _TDSPBUS_INTERFACE_DATA *dev, unsigned int state);
	int (*resume)(struct _TDSPBUS_INTERFACE_DATA *dev);
	int (*notify_reset)(struct _TDSPBUS_INTERFACE_DATA *dev, int resetevent);
	int (*notify_initreset)(struct _TDSPBUS_INTERFACE_DATA *dev, int resetevent);
	int (*notify_deatch)(struct _TDSPBUS_INTERFACE_DATA *dev, int reason);
	void *pUsbDev; // usb_device
} TDSPBUS_INTERFACE, *PTDSPBUS_INTERFACE;
