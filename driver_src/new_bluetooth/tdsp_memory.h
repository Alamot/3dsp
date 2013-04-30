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
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/mm.h>

#include <linux/string.h>

static inline void* tdsp_memory_alloc(unsigned int required_size)
{
	return(kmalloc(required_size, GFP_KERNEL));
}
static inline void* tdsp_memory_alloc_atomic(unsigned int required_size)
{
	return(kmalloc(required_size, GFP_ATOMIC));
}
static inline void* tdsp_memory_vmalloc(unsigned long required_size)
{
	return(vmalloc(required_size));
}
static inline void tdsp_memory_vfree(void *p)
{
	vfree(p);
}
static inline void tdsp_memory_free(void *p)
{
	kfree(p);
}
static inline void tdsp_memset(void* p,  unsigned char value, unsigned int len)
{
	memset(p,value,len);
}
static inline int tdsp_memcmp(const void * p1, const void * p2, unsigned long len)
{
	return memcmp(p1,p2,len);
}
static inline void* tdsp_memcpy(void* dest, const void * src, unsigned int len)
{
	return (memcpy(dest,src,len));
}
void* tdsp_memory_move(void *dest, void *src, unsigned long size);
void* tdsp_zero_memory(void *dest,unsigned long size);
#endif

