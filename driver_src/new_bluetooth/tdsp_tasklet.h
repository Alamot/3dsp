 /***********************************************************************
  * Copyright(C) 2006 3DSP Corporation. All Rights Reserved.
  
  * FILENAME		:tdsp_tasklet.h         VERSION:1.0
  * CREATE DATE	:2009/01/20
  * PURPOSE:	This file includes functions related to workqueue(worklet) and tasklet.
  *
  * AUTHORS:
  *
  * DECLARATION:  This document contains confidential proprietary information that
  *               is solely for authorized personnel. It is not to be disclosed to
  *               any unauthorized person without prior written consent of 3DSP
  *               Corporation.		
  ***********************************************************************/

#ifndef __TDSP_TASKLET_H__
#define __TDSP_TASKLET_H__

#include <linux/version.h>
#include "tdsp_memory.h"
#include "usbbt_lib.h"
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#include <linux/mm.h>
#endif

#include <asm/irq.h>
#include <linux/interrupt.h> 

void tdsp_tasklet_init( PTDSP_TASKLET t, void (*func)(unsigned long), unsigned long data);
void tdsp_tasklet_kill(PTDSP_TASKLET t);
void tdsp_tasklet_schedule(PTDSP_TASKLET t);
void tdsp_tasklet_hi_schedule(PTDSP_TASKLET t);
void tdsp_tasklet_disable(PTDSP_TASKLET t);
void tdsp_tasklet_enable(PTDSP_TASKLET t);

#endif 
