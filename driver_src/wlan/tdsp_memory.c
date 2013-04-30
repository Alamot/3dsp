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
 * [tdsp_buff.c] description:
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

 
/*
	File name:	tdsp_buff.c
	Version:	08/05/23
	Programmer: Spear
	First Version: 
*/

#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm.h>
//#include "tdsp_basetypes.h"
//#include "tdsp_debug.h"
#include "tdsp_memory.h"
//#include "tdsp_string.h"


//static char* TDSP_FILE_INDICATOR="MEMRY";


#ifdef DEBUG_OPEN__VKI

/*
 *	typedef struct {
 *		UINT8	used;
 *		UINT8	device_id;
 *		UINT32	start_addr;
 *		UINT32	length;
 *		UINT8	name[64];
 *	} tdsp_mem_log_t;
 */


tdsp_mem_log_t		tdsp_mem_log[TDSP_MEM_LOG_MAX_CNT];

#endif

void* _tdsp_memory_alloc(unsigned int required_size, void* func_name, unsigned int line)
{
	//NOTE: not use GFP_ATOMIC flag,cause we do NOT allco memory in ISR or DSR
	void* ptr;
	
	ptr = kmalloc(required_size, GFP_KERNEL);
	if(!ptr) {
		//DBG_VKI__MEMORY(LEVEL_ERR, "_tdsp_memory_alloc failed!\n");
		return NULL;
	}
	else {
#ifdef DEBUG_OPEN__VKI
		{
			UINT8 i;
			for(i = 0; i < TDSP_MEM_LOG_MAX_CNT ; i++) {
				if (tdsp_mem_log[i].used == 0) {
					tdsp_mem_log[i].used = 1;
					tdsp_mem_log[i].start_addr = ptr;
					tdsp_mem_log[i].length = required_size;
					
					tdsp_strcpy(tdsp_mem_log[i].func_name, func_name);
					tdsp_mem_log[i].func_length = line;
					break;
				}
			}
			if (i == TDSP_MEM_LOG_MAX_CNT) {
				DBG_VKI__MEMORY(LEVEL_ERR, "[%s]:     Log buffer our of service!!! \n",
								__FUNCTION__);
			}
		}
#endif
		return ptr;
	}
}



void _tdsp_memory_free(void* p)
{
	//kd_assert(p != NULL);

#ifdef DEBUG_OPEN__VKI
	{
		UINT8 i;
		for(i = 0; i < TDSP_MEM_LOG_MAX_CNT ; i++) {
			if ((tdsp_mem_log[i].used == 1)
				&& (tdsp_mem_log[i].start_addr == p)) {
				tdsp_mem_log[i].used = 0;
				break;
			}
		}
		if (i == TDSP_MEM_LOG_MAX_CNT) {
			DBG_VKI__MEMORY(LEVEL_ERR, "[%s]:    Log buffer no such record !!! \n",
							__FUNCTION__);
		}
	}
#endif
	
	kfree(p);
    p = NULL;
}



 
void _tdsp_memory_report(void)
{    

#ifdef DEBUG_OPEN__VKI
	UINT8 i;
	UINT32 total_alloced_size = 0;
	DBG_VKI__MEMORY(LEVEL_TRACE, "\n\n");
	DBG_VKI__MEMORY(LEVEL_ERR, "____________________________"
					"____________________________"
					"_____________________________________\n\n");
	for(i = 0; i < TDSP_MEM_LOG_MAX_CNT ; i++) {
		if (tdsp_mem_log[i].used == 1) {
			total_alloced_size += tdsp_mem_log[i].length;
			DBG_VKI__MEMORY(LEVEL_ERR, "[log_idx = %02d]	"
							"start_addr = %p;	length = %d;	"
							"func = %s;		line = %d;\n",
							i, 
							tdsp_mem_log[i].start_addr,
							tdsp_mem_log[i].length,
							tdsp_mem_log[i].func_name,
							tdsp_mem_log[i].func_length);
		}
	}
	DBG_VKI__MEMORY(LEVEL_ERR, "\n");

	DBG_VKI__MEMORY(LEVEL_ERR, "total_alloced_size = %d (Bytes)\n", total_alloced_size);
	DBG_VKI__MEMORY(LEVEL_ERR, "____________________________"
					"____________________________"
					"_____________________________________\n");
	
	DBG_VKI__MEMORY(LEVEL_ERR, "\n\n");

#endif


}

void * tdsp_memory_alloc(unsigned int required_size)
{
    //printk("enter [%s]\n",__FUNCTION__);
    return _tdsp_memory_alloc(required_size, (void *)__FUNCTION__, __LINE__);
}


void tdsp_memory_free(void * p)
{
    //printk("enter [%s]\n",__FUNCTION__);
    _tdsp_memory_free(p);
}


void tdsp_memory_report(void)
{
    _tdsp_memory_report();
}

