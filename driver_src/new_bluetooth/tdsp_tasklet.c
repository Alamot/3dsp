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

void tdsp_tasklet_init( PTDSP_TASKLET t, void (*func)(unsigned long), unsigned long data)
{   
	struct tasklet_struct *tasklet;
	tasklet = (struct tasklet_struct *)kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	if(NULL == tasklet)
	{
		printk("%s:error, can't alloc memory for tasklet\n",__FUNCTION__);
		return;
	}
	tasklet_init(tasklet, func, data);
	t->tasklet = tasklet;
}
void tdsp_tasklet_kill(PTDSP_TASKLET t)
{
	struct tasklet_struct *tasklet = ( struct tasklet_struct *)t->tasklet;
	if(NULL == tasklet)
	{
		printk("%s:error, tasklet is null\n",__FUNCTION__);
		return;
	}
	tasklet_kill(tasklet);
	kfree(tasklet);
	t->tasklet = NULL;
}
void tdsp_tasklet_schedule(PTDSP_TASKLET t)
{
	struct tasklet_struct *tasklet = ( struct tasklet_struct *)t->tasklet;
	if(NULL == tasklet)
	{
		printk("%s:error, tasklet is null\n",__FUNCTION__);
		return;
	}
	tasklet_schedule(t->tasklet);
}

void tdsp_tasklet_hi_schedule(PTDSP_TASKLET t)
{
	struct tasklet_struct *tasklet = ( struct tasklet_struct *)t->tasklet;
	if(NULL == tasklet)
	{
		printk("%s:error, tasklet is null\n",__FUNCTION__);
		return;
	}
	tasklet_hi_schedule(tasklet);
}

void tdsp_tasklet_disable(PTDSP_TASKLET t)
{
	struct tasklet_struct *tasklet = ( struct tasklet_struct *)t->tasklet;
	if(NULL == tasklet)
	{
		printk("%s:error, tasklet is null\n",__FUNCTION__);
		return;
	}
	tasklet_disable(tasklet);
}

void tdsp_tasklet_enable(PTDSP_TASKLET t)
{
	struct tasklet_struct *tasklet = ( struct tasklet_struct *)t->tasklet;
	if(NULL == tasklet)
	{
		printk("%s:error, tasklet is null\n",__FUNCTION__);
		return;
	}
	tasklet_enable(tasklet);
}



