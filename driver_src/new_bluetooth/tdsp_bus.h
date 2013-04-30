 /***********************************************************************
  * Copyright(C) 2006 3DSP Corporation. All Rights Reserved.
  
  * FILENAME		:tdsp_bus.h         VERSION:1.0
  * CREATE DATE	:2009/02/05
  * PURPOSE:	
  *
  * AUTHORS:     
  *
  * DECLARATION:  This document contains confidential proprietary information that
  *               is solely for authorized personnel. It is not to be disclosed to
  *               any unauthorized person without prior written consent of 3DSP
  *               Corporation.		
  ***********************************************************************/


#ifndef _TDSP_BUS_H_
#define _TDSP_BUS_H_

int Bus_closeInterface(void *pIntf);
int Bus_openInterface(void *pIntf);
void Bus_set_firmware_version(void *pIntf, unsigned long c8051_version, unsigned long dsp_version);
void *Bus_new_interface(void *dev);
unsigned char Bus_get_hotkey_flag(void *pIntf);
unsigned int tdsp_get_combodrv(void);

#endif
