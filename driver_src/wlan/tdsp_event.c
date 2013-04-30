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
//#include "tdsp_basetypes.h"
//#include "tdsp_debug.h"
#include "tdsp_event.h"
#include "tdsp_memory.h"

void tdsp_event_init(PTDSP_EVENT env)
{ 
    PTDSP_EVENT_CNTXT env_ctx;
    env_ctx = (PTDSP_EVENT_CNTXT)tdsp_memory_alloc(sizeof(TDSP_EVENT_CNTXT));
    if(env_ctx == NULL)
    {
        printk("[%s]:error,can't alloc memory for event\n",__FUNCTION__);
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
        printk("[%s]: error,event is null\n",__FUNCTION__);
        return;
    }   
    atomic_set(&env_ctx->flag, EVENT_SIGNED);
    wake_up_interruptible(&env_ctx->wq);
}


int tdsp_event_wait(PTDSP_EVENT env,unsigned int timeout)
{
    int status;
    PTDSP_EVENT_CNTXT env_ctx = (PTDSP_EVENT_CNTXT)env->event;
    if(env_ctx == NULL)
    {
        printk("[%s]: error,event is null\n",__FUNCTION__);
        return 0;
    } 
    status = wait_event_interruptible_timeout(env_ctx->wq, tdsp_event_signed(&env_ctx->flag), timeout);
    atomic_set(&env_ctx->flag, EVENT_UNSIGNED);
    return status;
}


void tdsp_event_reset(PTDSP_EVENT env)		
{
    PTDSP_EVENT_CNTXT env_ctx = (PTDSP_EVENT_CNTXT)env->event;
    if(env_ctx == NULL)
    {
        printk("[%s]: error,event is null\n",__FUNCTION__);
        return;
    } 
    atomic_set(&env_ctx->flag, EVENT_UNSIGNED);
    wake_up_interruptible(&env_ctx->wq);
}

void tdsp_event_kill(PTDSP_EVENT env)
{
    PTDSP_EVENT_CNTXT env_ctx = (PTDSP_EVENT_CNTXT)env->event;
    if(env_ctx == NULL)
    {
        printk("[%s]: error,event is null\n",__FUNCTION__);
        return;
    } 
    tdsp_memory_free(env_ctx);
    env->event = NULL;
}

