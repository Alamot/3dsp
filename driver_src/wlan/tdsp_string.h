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
 * [tdsp_string.h] description:
 * 	This header file defindes some function related with string access.
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

#ifndef __TDSP_STRING_H__
#define __TDSP_STRING_H__

//#include "tdsp_basetypes.h"


/* NOTICE::
 *
 * os provide ARCH related funcion implemented by ASM have better efficiency
 * than we used now!
 */




/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 */
static inline unsigned int  _tdsp_strlen(const char * s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}




/**
 * strcpy - Copy a %NUL terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 */
static inline char * _tdsp_strcpy(char * dest,const char *src)
{
	char *tmp = dest;

	while ((*dest++ = *src++) != '\0')
		/* nothing */;
	return tmp;
}

/**
 * strncpy - Copy a %NUL terminated string with limited length
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @len: Max Length
 */
static inline char * _tdsp_strncpy(char * dest,const char *src, unsigned int len)
{
	char *tmp = dest;

	while (len) {
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
		len--;
	}
	return dest;
}


/**
 * strcmp - Compare two strings
 * @cs: One string
 * @ct: Another string
 */
static inline  unsigned int _tdsp_strcmp(const char * cs,const char * ct)
{
	register char __res;

	while (1) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}

	return __res;
}




/**
 * strcat - Append one %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 */
static inline char *_tdsp_strcat(char *dest, const char *src)
{
	char *tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0')
		;
	return tmp;
}




#define tdsp_strncpy(dest, src, len)		_tdsp_strncpy(dest, src, len)

#define tdsp_strcpy(dest, src)		_tdsp_strcpy(dest, src)
#define tdsp_strlen(string)			_tdsp_strlen(string)

#define tdsp_strcmp(cs, ct)			_tdsp_strcmp(cs, ct)

#define tdsp_strcat(dest, src)		_tdsp_strcat(dest, src)




#endif	//__TDSP_STRING_H__

