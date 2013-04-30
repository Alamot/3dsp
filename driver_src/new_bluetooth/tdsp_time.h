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

#include <linux/time.h>
#include <linux/delay.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#include <linux/jiffies.h>
#endif
#include "tdsp_memory.h"

static inline unsigned long tdsp_ms_to_ticks(unsigned long ms)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
	struct timeval time;
	memset(&time, 0, sizeof(time));
	time.tv_sec = 0;
	time.tv_usec = (suseconds_t)ms * 1000;
	return (unsigned long)timeval_to_jiffies(&time);
#else
	return ms*HZ/1000;
#endif
}
static inline unsigned long tdsp_get_ticks(void)
{
	return (unsigned long)jiffies;
}

#endif

