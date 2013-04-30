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
 * tdsp_timer.c:
 *
 * created:
 * 	??/??/????	3DSP
 *
 * description:
 * 	This source file, 
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

#include <linux/sched.h>
#include "tdsp_memory.h"
#include "tdsp_timer.h"
#include "tdsp_time.h"

void tdsp_timer_init(PTDSP_TIMER t, void (*timeout_handler)(unsigned long), unsigned long data)
{
	struct timer_list * tl;
	tl = (struct timer_list *)kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	tl->data = (unsigned long)data;
	tl->function = timeout_handler;
	t->timer = tl;
	init_timer(tl);
}
void tdsp_timer_start(PTDSP_TIMER t, unsigned int timeout)
{
	struct timer_list * tl = ( struct timer_list *)t->timer;
	tl->expires = (unsigned long)(jiffies + tdsp_ms_to_ticks(timeout));
	add_timer(tl);
}
void tdsp_timer_stop(PTDSP_TIMER t)
{
	struct timer_list * tl = ( struct timer_list *)t->timer;
	del_timer(tl);
}

void tdsp_timer_stop_sync(PTDSP_TIMER t)
{
	struct timer_list * tl = ( struct timer_list *)t->timer;
	del_timer_sync(tl);
}
void tdsp_timer_reset(PTDSP_TIMER t, unsigned int sec)
{
	struct timer_list * tl = ( struct timer_list *)t->timer;
	mod_timer(tl, jiffies + tdsp_ms_to_ticks(sec));
}
void tdsp_timer_kill(PTDSP_TIMER t)
{
	struct timer_list * tl = ( struct timer_list *)t->timer;
	if (tl == NULL)
		return;
	del_timer(tl);
	kfree(tl);
	t->timer = NULL;
}
inline void tdsp_usdelay(unsigned long usecs)
{
        udelay(usecs);
}
inline void tdsp_msdelay(unsigned int msecs)
{
        msleep(msecs);
}

