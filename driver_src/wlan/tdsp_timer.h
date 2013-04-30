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
	File name:	tdsp_timer.h
	Version:	08/06/04
	Programmer:  Spear
	First Version: 
*/


#ifndef __TDSP_TIMER_H__
#define __TDSP_TIMER_H__

//#include "tdsp_basetypes.h"

#include <linux/timer.h>
#include "usbwlan_lib.h"
void tdsp_timer_init(PTDSP_TIMER t, void (*timeout_handler)(unsigned long), unsigned long data);


void tdsp_timer_stop(PTDSP_TIMER t);

void tdsp_timer_start(PTDSP_TIMER t,unsigned int timeout);

void tdsp_timer_reset(PTDSP_TIMER t, unsigned int sec);


void tdsp_timer_kill(PTDSP_TIMER t);

unsigned char	tdsp_get_random_byte(void);
unsigned int	tdsp_get_random_dword(void);
unsigned char  time_downout_duration(void);
#endif



 
