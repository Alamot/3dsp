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
 * [tdsp_event.h] description:
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


#ifndef __TDSP_EVENT_H__
#define __TDSP_EVENT_H__

#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>


//#include "tdsp_basetypes.h"
#include "usbwlan_lib.h"
#define EVENT_UNSIGNED    0
#define EVENT_SIGNED      1

#if 0
typedef struct _TDSP_EVENT
{
    void* env;
}DSP_EVENT,*PTDSP_EVENT;
#endif

//typedef PEVENT PDSP_EVENT;

typedef struct _TDSP_EVENT_CNTXT
{
    wait_queue_head_t wq;

    atomic_t flag;
    
}TDSP_EVENT_CNTXT,*PTDSP_EVENT_CNTXT;

#define tdsp_event_signed(_x)		( atomic_read(_x) == EVENT_SIGNED)


void tdsp_event_init(PTDSP_EVENT event);
void tdsp_event_set(PTDSP_EVENT event);
int tdsp_event_wait(PTDSP_EVENT event,unsigned int timeout);
void tdsp_event_reset(PTDSP_EVENT event);

void tdsp_event_kill(PTDSP_EVENT event);
/*
 * function declare
 */
	





/*
 * MACRO defines
 */




#endif		//__TDSP_EVENT_H__

