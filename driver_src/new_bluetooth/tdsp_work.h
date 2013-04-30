#ifndef __TDSP_WORK_H__
#define __TDSP_WORK_H__

#include <linux/version.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include "tdsp_memory.h"
#include "usbbt_lib.h"

typedef void (*TDSP_TASK_ROUTINE)(unsigned long);

typedef struct _WORK_DATA
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	struct delayed_work work;
#else	
	struct work_struct work;
#endif
	unsigned long para;
	TDSP_TASK_ROUTINE func;
}WORK_DATA, *PWORK_DATA;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
void _tdsp_work_routine(struct work_struct * param_data);
#else
void _tdsp_work_routine(void * param_data);
#endif

void* tdsp_workthread_create(char *threadname);
void tdsp_destory_workqueue(PTDSP_WORKQUEUE queue);
void tdsp_delaywork_init(PTDSP_WORK w, TDSP_TASK_ROUTINE func, unsigned long para);
int tdsp_queue_delayedwork(PTDSP_WORKQUEUE queue, PTDSP_WORK work, unsigned long delay);
void tdsp_cancel_delayedwork(PTDSP_WORKQUEUE queue, PTDSP_WORK work);
void tdsp_delaywork_release(PTDSP_WORKQUEUE queue, PTDSP_WORK work);
void tdsp_work_init(PTDSP_WORK w, TDSP_TASK_ROUTINE func, unsigned long para); 
int tdsp_queue_work(PTDSP_WORKQUEUE queue, PTDSP_WORK work);
void tdsp_work_release(PTDSP_WORK work);

#endif
