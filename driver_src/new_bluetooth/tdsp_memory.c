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
 * [tdsp_memory.c] description:
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
 
#include "tdsp_memory.h"

void* tdsp_memory_move(void *dest, void *src, unsigned long size)
{
	if(dest == NULL || src == NULL)
	{
		printk("%s: error,dest or src is null\n",__FUNCTION__);
		return NULL;
	}
	return (memmove(dest, src, size));
}
void* tdsp_zero_memory(void *dest,unsigned long size)
{
	if(dest == NULL)
	{
		printk("%s: error,dest is null\n",__FUNCTION__);
		return NULL;
	}
	return (memset(dest, 0, size));
}
