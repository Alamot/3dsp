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
	File name:	tdsp_mutex.c
	Version:	2008/05/22
	Programmer: Spear
	First Version: 
*/


#include "tdsp_mutex.h"
#include "tdsp_memory.h"

void tdsp_spin_lock_init(TDSP_SPINLOCK *lc)
{
    spinlock_t * lock;
    //printk("enter [%s]\n",__FUNCTION__);
    lock =  (spinlock_t *)tdsp_memory_alloc(sizeof(spinlock_t));

    if(NULL == lock)
    {
        printk("[%s]:error, can't alloc memory for spinlock\n",__FUNCTION__);
        return;
    }
    spin_lock_init(lock);
    lc->lock = lock;
}

void tdsp_spin_lock(TDSP_SPINLOCK *lc)
{
    spinlock_t * lock = (spinlock_t *) lc->lock;
    if(NULL == lock)
    {
        printk("[%s]:error, lock is null\n",__FUNCTION__);
        return;
    }
	spin_lock(lock);
}

void tdsp_spin_lock_bh(TDSP_SPINLOCK *lc)
{
    spinlock_t * lock = (spinlock_t *) lc->lock;
    if(NULL == lock)
    {
        printk("[%s]:error, lock is null\n",__FUNCTION__);
        return;
    }
	spin_lock_bh(lock);
}

void tdsp_spin_lock_irqsave(TDSP_SPINLOCK *lc, unsigned long *flags)
{
	spinlock_t * lock = (spinlock_t *) lc->lock;
	unsigned long fg;
	
	if(NULL == lock)
	{
		printk("[%s]:error, lock is null\n",__FUNCTION__);
		return;
	}
	spin_lock_irqsave((spinlock_t *) lock, fg);
	*flags = fg;
}


void tdsp_spin_unlock(TDSP_SPINLOCK *lc)
{
    spinlock_t * lock = (spinlock_t *) lc->lock;
    if(NULL == lock)
    {
        printk("[%s]:error, lock is null\n",__FUNCTION__);
        return;
    }
	spin_unlock((spinlock_t *)lock);
}

void tdsp_spin_unlock_irqrestroe(TDSP_SPINLOCK *lc,unsigned long flags)
{
    spinlock_t * lock = (spinlock_t *) lc->lock;
    if(NULL == lock)
    {
        printk("[%s]:error, lock is null\n",__FUNCTION__);
        return;
    }
    spin_unlock_irqrestore(lock, flags);
}

void tdsp_spin_unlock_bh(TDSP_SPINLOCK *lc)
{
    spinlock_t * lock = (spinlock_t *) lc->lock;
    if(NULL == lock)
    {
        printk("[%s]:error, lock is null\n",__FUNCTION__);
        return;
    }
	spin_unlock_bh((spinlock_t *)lock);
}

void tdsp_spin_lock_kill(TDSP_SPINLOCK *lc)
{
    spinlock_t * lock = (spinlock_t *) lc->lock;
    if(NULL == lock)
    {
        printk("[%s]:error, lock is null\n",__FUNCTION__);
        return;
    }
    tdsp_memory_free( lock);
    lc->lock = NULL;
}


