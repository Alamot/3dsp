#include "tdsp_work.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
void _tdsp_work_routine(struct work_struct *param_data)
#else
void _tdsp_work_routine(void *param_data)
#endif
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	struct delayed_work *dw = container_of(param_data, struct delayed_work, work);
	PWORK_DATA p_worklet = (PWORK_DATA)container_of(dw, WORK_DATA, work);
#else
	PWORK_DATA p_worklet = (PWORK_DATA)param_data;
#endif
	p_worklet->func(p_worklet->para);
}

void* tdsp_workthread_create(char *threadname)
{
	return (create_singlethread_workqueue(threadname));
}
void tdsp_destory_workqueue(PTDSP_WORKQUEUE queue)
{
	if(NULL == queue->workqueue)
	{
		printk("%s:error, queue is null\n",__FUNCTION__);
		return;
	}
	flush_workqueue((struct workqueue_struct *)queue->workqueue);
	destroy_workqueue((struct workqueue_struct *)queue->workqueue);
}

void tdsp_delaywork_init(PTDSP_WORK w, TDSP_TASK_ROUTINE func, unsigned long para)
{
	PWORK_DATA work_data =  (PWORK_DATA)tdsp_memory_alloc_atomic(sizeof(WORK_DATA));
	if(work_data == NULL)
		return;
	work_data->func = func;
	work_data->para = para;
	init_timer(&(work_data->work.timer));
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	INIT_WORK(&(work_data->work.work), _tdsp_work_routine);
#else
	INIT_WORK(&(work_data->work), _tdsp_work_routine, work_data);
#endif
	w->work = work_data;
}

int tdsp_queue_delayedwork(PTDSP_WORKQUEUE queue, PTDSP_WORK work, unsigned long delay)
{
	PWORK_DATA pwd = (PWORK_DATA)(work->work);
	if(NULL == queue->workqueue|| NULL == pwd)
	{
		printk("%s:error, queue or work is null\n",__FUNCTION__);
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	return (queue_delayed_work((struct workqueue_struct *)queue->workqueue, (struct delayed_work *)(&pwd->work), delay));
#else
	return (queue_delayed_work((struct workqueue_struct *)queue->workqueue, (struct work_struct *)(&pwd->work), delay));
#endif
}

void tdsp_cancel_delayedwork(PTDSP_WORKQUEUE queue, PTDSP_WORK work)
{
	PWORK_DATA pwd = (PWORK_DATA)(work->work);
	if(NULL == queue->workqueue|| NULL == pwd)
	{
		printk("%s:error, queue or work is null\n",__FUNCTION__);
		return;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	cancel_delayed_work((struct delayed_work *)(&pwd->work));
#else
	cancel_delayed_work((struct work_struct *)(&pwd->work));
#endif
/*
	flush_workqueue((struct workqueue_struct *)queue->workqueue);
*/
}

void tdsp_delaywork_release(PTDSP_WORKQUEUE queue, PTDSP_WORK work)
{
	PWORK_DATA pwd = (PWORK_DATA)(work->work);
	if(NULL == queue->workqueue|| NULL == pwd)
	{
		printk("%s:error, queue or work is null\n",__FUNCTION__);
		return;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	cancel_delayed_work((struct delayed_work *)(&pwd->work));
#else
	cancel_delayed_work((struct work_struct *)(&pwd->work));
#endif
/*
	flush_workqueue((struct workqueue_struct *)queue->workqueue);
*/
	kfree((PWORK_DATA)work->work);
}

void tdsp_work_init(PTDSP_WORK w, TDSP_TASK_ROUTINE func, unsigned long para)
{
	PWORK_DATA work_data =  (PWORK_DATA)tdsp_memory_alloc(sizeof(WORK_DATA));
	work_data->func = func;
	work_data->para = para;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	INIT_WORK(&(work_data->work.work), _tdsp_work_routine);
#else
	INIT_WORK(&(work_data->work), _tdsp_work_routine, work_data);
#endif
	w->work = work_data;
}

int tdsp_queue_work(PTDSP_WORKQUEUE queue, PTDSP_WORK work)
{
	PWORK_DATA pwd = (PWORK_DATA)(work->work);
	if(NULL == queue->workqueue || NULL == pwd)
	{
		printk("%s:error, queue or work is null\n",__FUNCTION__);
	}

	return (queue_work((struct workqueue_struct *)queue->workqueue, (struct work_struct *)(&pwd->work)));
}

void tdsp_work_release(PTDSP_WORK work)
{
	PWORK_DATA pwd = (PWORK_DATA)(work->work);
	if(NULL == pwd)
	{
		printk("%s:error, queue or work is null\n",__FUNCTION__);
		return;
	}
	kfree((PWORK_DATA)work->work);
	work->work = NULL;
}


