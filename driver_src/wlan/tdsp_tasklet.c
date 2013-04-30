 /***********************************************************************
  * Copyright(C) 2006 3DSP Corporation. All Rights Reserved.
  
  * FILENAME		:tdsp_tasklet.c         VERSION:1.0
  * CREATE DATE	:2009/01/20
  * PURPOSE:	This file includes functions related to workqueue, we defined it, worklet.
  *
  * AUTHORS:
  *
  * DECLARATION:  This document contains confidential proprietary information that
  *               is solely for authorized personnel. It is not to be disclosed to
  *               any unauthorized person without prior written consent of 3DSP
  *               Corporation.		
  ***********************************************************************/
  
#include "tdsp_tasklet.h"


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)
void _tdsp_worklet_routine(struct work_struct * param_data)
#else
void _tdsp_worklet_routine(void * param_data)
#endif
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)
	PWORKLET_DATA p_worklet = container_of(param_data, WORKLET_DATA, work);
#else
	PWORKLET_DATA p_worklet = (PWORKLET_DATA)param_data;
#endif

	if (1 == p_worklet->running)
	{
		p_worklet->func(p_worklet->para);
		p_worklet->running = 0;
	}
}

void tdsp_worklet_init(PTDSP_WORKLET w, TDSP_TASKLET_ROUTINE func, unsigned long para)
{
    PWORKLET_DATA work_data =  (PWORKLET_DATA)tdsp_memory_alloc(sizeof(WORKLET_DATA));
	//work_data->queue = create_singlethread_workqueue("tdsp_worklet");
	work_data->func = func;
	work_data->para = para;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)
	INIT_WORK(&(work_data->work), _tdsp_worklet_routine);
#else
	INIT_WORK(&(work_data->work), _tdsp_worklet_routine, work_data);
#endif
	work_data->running = 0;
    w->worklet = work_data;
}

void tdsp_worklet_kill(PTDSP_WORKLET w)
{
//	cancel_work_sync(&p_tasklet->work);
    PWORKLET_DATA work_data =  (PWORKLET_DATA)w->worklet;
	//destroy_workqueue(work_data->queue);
    tdsp_memory_free(work_data);
    w->worklet = NULL;
}

void tdsp_worklet_schedule(PTDSP_WORKLET w)
{
    PWORKLET_DATA work_data =  (PWORKLET_DATA)w->worklet;
	work_data->running = 1;
	//queue_work(work_data->queue, &work_data->work);
    schedule_work(&work_data->work);
}

unsigned char tdsp_worklet_running(PTDSP_WORKLET w)
{
    PWORKLET_DATA work_data =  (PWORKLET_DATA)w->worklet;
    return work_data->running;
}


void tdsp_tasklet_init( PTDSP_TASKLET t,
            void (*func)(unsigned long), unsigned long data)
{   
    struct tasklet_struct * tasklet;
   // printk("enter [%s]\n",__FUNCTION__);
    tasklet = (struct tasklet_struct *)tdsp_memory_alloc(sizeof(struct tasklet_struct));
    if(NULL == tasklet)
    {
        printk("[%s]:error, can't alloc memory for tasklet\n",__FUNCTION__);
        return;
    }
    tasklet_init(tasklet, func, data);
    t->tasklet = tasklet;
}

void tdsp_tasklet_kill(PTDSP_TASKLET t)
{
    struct tasklet_struct * tasklet = ( struct tasklet_struct *)t->tasklet;
       if(NULL == tasklet)
    {
        printk("[%s]:error, tasklet is null\n",__FUNCTION__);
        return;
    }
    tasklet_kill(tasklet);
    tdsp_memory_free(tasklet);
    t->tasklet = NULL;
}

void tdsp_tasklet_schedule(PTDSP_TASKLET t)
{
    //ASSERT(t);
    //ASSERT(t->taskelet);
    struct tasklet_struct * tasklet = ( struct tasklet_struct *)t->tasklet;
    if(NULL == tasklet)
    {
        printk("[%s]:error, tasklet is null\n",__FUNCTION__);
        return;
    }
    tasklet_schedule(t->tasklet);
}

void tdsp_tasklet_hi_schedule(PTDSP_TASKLET t)
{
    struct tasklet_struct * tasklet = ( struct tasklet_struct *)t->tasklet;
    if(NULL == tasklet)
    {
        printk("[%s]:error, tasklet is null\n",__FUNCTION__);
        return;
    }
    tasklet_hi_schedule(tasklet);
}

void tdsp_tasklet_disable(PTDSP_TASKLET t)
{
    struct tasklet_struct * tasklet = ( struct tasklet_struct *)t->tasklet;
    if(NULL == tasklet)
    {
        printk("[%s]:error, tasklet is null\n",__FUNCTION__);
        return;
    }
    tasklet_disable(tasklet);
}

void tdsp_tasklet_enable(PTDSP_TASKLET t)
{
     struct tasklet_struct * tasklet = ( struct tasklet_struct *)t->tasklet;
    if(NULL == tasklet)
    {
        printk("[%s]:error, tasklet is null\n",__FUNCTION__);
        return;
    }
    tasklet_enable(tasklet);
}



