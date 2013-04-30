/************************************************************************
* Copyright (c) 2006, 3DSP Corporation, all rights reserved.  Duplication or
* reproduction of any part of this software (source code, object code or
* comments) without the expressed written consent by 3DSP Corporation is
* forbidden.  For further information please contact:
*
* 3DSP Corporation
* 16271 Laguna Canyon Rd
* Irvine, CA 92618
* www.3dsp.com
*************************************************************************/
/*
	File name:	tdsp_mutex.h
	Version:	v1.0 Jan.26/2008
	Programmer: Wu Min
	First Version: 
*/

#ifndef __TDSP_MUTEX_H__
#define __TDSP_MUTEX_H__

#include <linux/spinlock.h>
#include <linux/hardirq.h>
#include "usbbt_lib.h"

void tdsp_spin_lock_init(TDSP_SPINLOCK *lock);
void tdsp_spin_lock(TDSP_SPINLOCK *lock);
void tdsp_spin_lock_bh(TDSP_SPINLOCK *lc);
void tdsp_spin_lock_irqsave(TDSP_SPINLOCK *lc, unsigned long *flags);
void tdsp_spin_unlock(TDSP_SPINLOCK *lock);
void tdsp_spin_unlock_bh(TDSP_SPINLOCK *lc);
void tdsp_spin_unlock_irqrestroe(TDSP_SPINLOCK *lc,unsigned long *flags);
void tdsp_spin_lock_kill(TDSP_SPINLOCK *lock);
unsigned int tdsp_isin_atomic(void);

#endif 

