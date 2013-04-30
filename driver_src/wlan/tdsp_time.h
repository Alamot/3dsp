/*************************************************************************
 *
 *	(c) 2004-05, 3DSP Corporation, all rights reserved.  Duplication or
 *	reproduction of any part of this software (source code, object code or
 *	comments) without the expressed written consent by 3DSP Corporation is
 *	forbidden.  For further information please contact:
 *
 *	3DSP Corporation
 *	16271 Laguna Canyon Rd
 *	Irvine, CA 92618
 *	www.3dsp.com 
 *
 *************************************************************************
 *
 * tdsp_time.h:
 *
 * description:
 * 	This header file, 
 *  
 * exports:
 *
 * make'ing:
 *
 * TODO:
 * 
 * see also:
 *
 * This source to be viewed with tabspace 4 (":se ts=4" for ex/vi)
 *
 ************************************************************************/

/*
	File name:	tdsp_time.h
	Version:	v1.0 2008/06/04
	Programmer:  Spear
	First Version: 
*/


#ifndef __TDSP_LINUX_TIME_H__
#define __TDSP_LINUX_TIME_H__


//#include <linux/version.h>
//#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
//#include <linux/types.h>
//#include <asm/div64.h>			// do_div defined in this file
//#include <linux/time.h>
//#include <linux/timex.h>		// CLOCK_TICK_RATE defined in this file
//#endif
//#include <linux/delay.h>



#include <linux/time.h>
#include <linux/delay.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#include <linux/jiffies.h>
#endif
#include "tdsp_memory.h"

static inline unsigned int tdsp_ticks_to_ms(unsigned int ticks)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
	struct timeval time;
	memset(&time, sizeof(time), 0);

	jiffies_to_timeval(ticks, &time);

	return ((time.tv_sec*1000) + (time.tv_usec/1000));
#else
	return ticks*1000/HZ;
#endif
}



static inline unsigned long long tdsp_ms_to_ticks(unsigned int ms)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
	struct timeval time;
	tdsp_memset(&time, sizeof(time), 0);

	time.tv_sec = 0;
	time.tv_usec = (suseconds_t)ms * 1000;

	return ( unsigned long long)timeval_to_jiffies(&time);
#else
	return ms*HZ/1000;
#endif
}

//delay about ms ºÁÃë, NOTE: call thsi fcn will cause thread schedule, drop the rest time slice
static inline void tdsp_sleep(unsigned int delay_ms)
{
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
		msleep(delay_ms);
	#else
		set_current_state("TASK_INTERRUPTIBLE");
		schedule_timeout(tdsp_ms_to_ticks(delay_ms));
	#endif
}	


#if 0
static inline unsigned int tdsp_get_current_time(void)
{

    struct timeval tv;
	do_gettimeofday(&tv);
	return tv;
}
#endif

#endif	// __TDSP_LINUX_TIME_H__

