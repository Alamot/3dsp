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

//#include "tdsp_basetypes.h"
//#include <asm/semaphore.h>
#include <linux/spinlock.h>
#include "usbwlan_lib.h"



//typedef spinlock_t 	tdsp_spinlock;

void tdsp_spin_lock_init(TDSP_SPINLOCK *lock);

void tdsp_spin_lock(TDSP_SPINLOCK *lock);

void tdsp_spin_unlock(TDSP_SPINLOCK *lock);

void tdsp_spin_lock_kill(TDSP_SPINLOCK *lock);

void tdsp_spin_lock_irqsave(TDSP_SPINLOCK *lc, unsigned long *flags);
void tdsp_spin_unlock_irqrestroe(TDSP_SPINLOCK *lc,unsigned long flags);

void tdsp_spin_lock_bh(TDSP_SPINLOCK *lc);
void tdsp_spin_unlock_bh(TDSP_SPINLOCK *lc);
#if 0
typedef struct semaphore		TDSP_MUTEX;
typedef TDSP_MUTEX *			PTDSP_MUTEX;

VOID tdsp_mutex_init(PTDSP_MUTEX mutex);

VOID tdsp_mutex_kill(PTDSP_MUTEX mutex);

VOID tdsp_mutex_lock(PTDSP_MUTEX mutex);

VOID tdsp_mutex_unlock(PTDSP_MUTEX mutex);




typedef struct semaphore		TDSP_SEM;
typedef TDSP_SEM 				*PTDSP_SEM;

VOID tdsp_sem_init(PTDSP_SEM mutex, INT32 val);

VOID tdsp_sem_destroy(PTDSP_SEM mutex);

//semaphore down
VOID tdsp_sem_wait(PTDSP_SEM mutex);

//return 0 indicates get the semaphore
INT32 tdsp_sem_trywait(PTDSP_SEM mutex);

VOID tdsp_sem_post(PTDSP_SEM mutex);
#endif 

#endif 

