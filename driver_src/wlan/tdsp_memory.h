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
 * [tdsp_memory.h] description:
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
	File name:	tdsp_buff.h
	Version:	2008/05/23
	Programmer:  Spear
	First Version: 
*/


#ifndef __TDSP_MEMORY_H__
#define __TDSP_MEMORY_H__


#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>

#include <linux/string.h>
#include <linux/vmalloc.h>
//#include "tdsp_basetypes.h"


/*
 * device driver ID define:
 * 
 * --------------------------------------------------------------
 * | status Name			| value	| Bit						|
 * --------------------------------------------------------------
 * | bus					| 0		| 00000000					|
 * | wlan					| 1		| 00000001					|
 * | bluetooth				| 2		| 00000010					|
 * |  						| 4		| 00000100					|
 * --------------------------------------------------------------
 */

#ifndef DEVICE_ID__BUS
#define DEVICE_ID__BUS			0
#endif

#ifndef DEVICE_ID__WLAN
#define DEVICE_ID__WLAN			1
#endif

#ifndef DEVICE_ID__BT
#define DEVICE_ID__BT			2
#endif







#ifdef DEBUG_OPEN__VKI

#define TDSP_MEM_LOG_MAX_CNT	40


typedef struct {
	unsigned char	used;
	void*	start_addr;
	unsigned int	length;
	unsigned char	func_name[64];
	unsigned int	func_length;
} tdsp_mem_log_t;

#endif


/*
 * inline function define
 */


static inline void tdsp_memset(void* p,  unsigned char value, unsigned int len)
{
    //printk("enter [%s]\n",__FUNCTION__);
    memset(p,value,len);
}

static inline int tdsp_memcmp(const void * p1, const void * p2, unsigned int len)
{
	return memcmp((p1),(p2),(len));
}

static inline void tdsp_memcpy(void* dest, const void * src, unsigned int len)
{
	memcpy(dest,src,len);
}

static inline void tdsp_zero_mem(void * p,unsigned int  len)
{
    memset(p,0,len);
}

static inline void tdsp_memory_vfree(void *p)
{
	vfree(p);
}

static inline void* tdsp_memory_vmalloc(unsigned long required_size)
{
	return(vmalloc(required_size));
}


// This may more efficiency than memcmp on eCos system !		fdeng
#if 0
static inline bool tdsp_mac_addr_cmp(const char * addr1, const char * addr2 )
{
	if((addr1[0] == addr2[0]) && (addr1[1] == addr2[1]) 
		&& (addr1[2] == addr2[2]) && (addr1[3] == addr2[3]) 
		&& (addr1[4] == addr2[4]) && (addr1[5] == addr2[5])) {
		return true;
	}
	else {
		return false;
	}
}
#endif
//PVOID _tdsp_memory_alloc(UINT32 required_size, VOID *func_name, UINT32 line);


//VOID _tdsp_memory_free(PVOID p);


//VOID _tdsp_memory_report(VOID);


void* tdsp_memory_alloc(unsigned int required_size);

void tdsp_memory_free(void* p);

void tdsp_memory_report(void);

#endif

