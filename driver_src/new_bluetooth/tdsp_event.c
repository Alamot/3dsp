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
 * [tdsp_lqueue.c] description:
 * 	This source file defindes some process about loop queue.
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

#include "tdsp_event.h"
#include "tdsp_memory.h"

void tdsp_event_init(PTDSP_EVENT env)
{ 
	PTDSP_EVENT_CNTXT env_ctx;
	env_ctx = (PTDSP_EVENT_CNTXT)kmalloc(sizeof(TDSP_EVENT_CNTXT), GFP_KERNEL);
	if(env_ctx == NULL)
	{
		printk("%s:error,can't alloc memory for event\n",__FUNCTION__);
		return;
	}
	init_waitqueue_head(&env_ctx->wq);
	atomic_set(&env_ctx->flag, EVENT_UNSIGNED);
	env->event = env_ctx;
}
void tdsp_event_set(PTDSP_EVENT env)
{
	PTDSP_EVENT_CNTXT env_ctx = (PTDSP_EVENT_CNTXT)env->event;
	if(env_ctx == NULL)
	{
		printk("%s: error,event is null\n",__FUNCTION__);
		return;
	}
	atomic_set(&env_ctx->flag, EVENT_SIGNED);
	wake_up_interruptible(&env_ctx->wq);
}
long tdsp_event_wait(PTDSP_EVENT env,long timeout)
{
	long status;
	PTDSP_EVENT_CNTXT env_ctx = (PTDSP_EVENT_CNTXT)env->event;
	if(env_ctx == NULL)
	{
		printk("%s: error,event is null\n",__FUNCTION__);
		return 0;
	} 
	status = wait_event_interruptible_timeout(env_ctx->wq, atomic_read(&env_ctx->flag) == EVENT_SIGNED, timeout);
	atomic_set(&env_ctx->flag, EVENT_UNSIGNED);
	return status;
}
void tdsp_event_kill(PTDSP_EVENT env)
{
	PTDSP_EVENT_CNTXT env_ctx = (PTDSP_EVENT_CNTXT)env->event;
	if(env_ctx == NULL)
	{
		printk("%s: error,event is null\n",__FUNCTION__);
		return;
	} 
	kfree(env_ctx);
	env->event = NULL;
}
void tdsp_event_flag_set(PTDSP_EVENT event, unsigned long flag)
{
	PTDSP_EVENT_CNTXT env_ctx = (PTDSP_EVENT_CNTXT)event->event;
	if(env_ctx == NULL)
	{
		printk("%s: error,event is null\n",__FUNCTION__);
		return;
	} 
	atomic_set(&env_ctx->flag, flag);
}

