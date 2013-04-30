/*
 * base_types.h:
 * 	Copyright (c) 2004 3DSP
 *		All rights Reserved.
 *
 * This source to be viewed with tabspace 2 (":se ts=2" for ex/vi)
 *
 */

#ifndef _BASETYPES_H_

#define _BASETYPES_H_
#include <linux/types.h>

//
// Define some basic macros or type
//

//-------------------------------------------------------------------------
// Bit Mask definitions
//-------------------------------------------------------------------------
#define BIT_0       0x0001
#define BIT_1       0x0002
#define BIT_2       0x0004
#define BIT_3       0x0008
#define BIT_4       0x0010
#define BIT_5       0x0020
#define BIT_6       0x0040
#define BIT_7       0x0080
#define BIT_8       0x0100
#define BIT_9       0x0200
#define BIT_10      0x0400
#define BIT_11      0x0800
#define BIT_12      0x1000
#define BIT_13      0x2000
#define BIT_14      0x4000
#define BIT_15      0x8000
#define BIT_16      0x10000
#define BIT_17      0x20000
#define BIT_18      0x40000
#define BIT_19      0x80000
#define BIT_20      0x100000
#define BIT_21      0x200000
#define BIT_22      0x400000
#define BIT_23      0x800000
#define BIT_24      0x01000000
#define BIT_25      0x02000000
#define BIT_26      0x04000000
#define BIT_27      0x08000000
#define BIT_28      0x10000000
#define BIT_29      0x20000000
#define BIT_30      0x40000000
#define BIT_31      0x80000000

#define BIT_0_1     0x0003
#define BIT_0_2     0x0007
#define BIT_0_3     0x000F
#define BIT_0_4     0x001F
#define BIT_0_5     0x003F
#define BIT_0_6     0x007F
#define BIT_0_7     0x00FF
#define BIT_0_8     0x01FF
#define BIT_0_11    0x0FFF
#define BIT_0_13    0x3FFF
#define BIT_0_15    0xFFFF
#define BIT_1_2     0x0006
#define BIT_1_3     0x000E
#define BIT_2_5     0x003C
#define BIT_3_4     0x0018
#define BIT_4_5     0x0030
#define BIT_4_6     0x0070
#define BIT_4_7     0x00F0
#define BIT_5_7     0x00E0
#define BIT_5_9     0x03E0
#define BIT_5_11    0x0FE0
#define BIT_5_12    0x1FE0
#define BIT_5_15    0xFFE0
#define BIT_5_16    0x1FFE0
#define BIT_6_7     0x00c0
#define BIT_7_11    0x0F80
#define BIT_8_11    0x0F00
#define BIT_8_15    0xFF00
#define BIT_9_13    0x3E00
#define BIT_10_17   0x3FC00
#define BIT_12_15   0xF000
#define BIT_15_18	0x78000
#define BIT_16_18   0x00070000
#define BIT_16_20   0x001F0000
#define BIT_17_18   0x00060000
#define BIT_19_20   0x00180000
#define BIT_21_25   0x03E00000
#define BIT_22_27   0x0FC00000
#define BIT_22_31   0xFFC00000
#define BIT_24_28   0x1F000000
#define BIT_26_27   0x0C000000


typedef enum
{
	TDSPBUS_FALSE,
	TDSPBUS_TRUE
} boolean;

typedef	signed char		int8;
typedef	signed short		int16;
typedef signed int		      int32;
typedef __s64			int64;
typedef	unsigned char		uint8;
typedef	unsigned short	      uint16;
typedef	unsigned int		uint32;
typedef	unsigned long		unsignedlong;
typedef	long		             signedlong;
typedef __u64			uint64;


//----Define for windows code.-------------
typedef uint8 			UCHAR, *PUCHAR,* PCSZ;
typedef uint8 			UNICODE_STRING,*PUNICODE_STRING;
typedef int8 				CHAR, *PCHAR;
typedef uint16 			USHORT, *PUSHORT;
typedef int16 			SHORT, *PSHORT;
typedef	unsignedlong		ULONG, *PULONG;
typedef	signedlong	      LONG, *PLONG;
typedef	uint32			UINT, UINT32,*PUINT,*PUINT32;
typedef	int32			      INT, *PINT;
typedef uint64			ULONGLONG, *PULONGLONG;
typedef int64				LONGLONG, *PLONGLONG;
typedef unsigned long		ULONG_PTR,* PULONG_PTR; // LP64, in LLP64, should be unsigned long long
typedef boolean                BOOLEAN;
typedef	int32                   KIRQL;	// for dummy para in KeAcquireSpinLock and KeReleaseSpinLock
typedef void                    VOID, *PVOID;
typedef uint8                   WCHAR,* PWCHAR;
typedef	dma_addr_t	      PHYSICAL_ADDRESS;	
typedef	spinlock_t             KSPIN_LOCK;

#define	Bit(x)		(1 << (x))

#ifndef NULL
	#define	NULL			((void *)0x0)
#endif

#define FALSE 0
#define TRUE 1

#define kTenMilliSec	1

#define consistent_sync(a,b,c)		

//for windows style timer and DPC function
typedef struct 
{
	unsigned long QuadPart;
} LARGE_INTEGER;

#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]


////////////////////////////////////  types to reload from windows  /////////////////////////////


typedef signed long NTSTATUS;
typedef NTSTATUS *PNTSTATUS;

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

//
// Generic test for information on any status value.
//

#define NT_INFORMATION(Status) ((ULONG)(Status) >> 30 == 1)

//
// Generic test for warning on any status value.
//

#define NT_WARNING(Status) ((ULONG)(Status) >> 30 == 2)

//
// Generic test for error on any status value.
//

#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)

//************************************************************************************//

//
// MessageId: STATUS_NO_SUCH_DEVICE
//
// MessageText:
//
//  A device which does not exist was specified.
//
#define STATUS_NO_SUCH_DEVICE            ((NTSTATUS)0xC000000EL)

//
// The success status codes 0 - 63 are reserved for wait completion status.
// FacilityCodes 0x5 - 0xF have been allocated by various drivers.
//
#define STATUS_SUCCESS                          ((NTSTATUS)0x00000000L) // ntsubauth

//
// MessageId: STATUS_INVALID_PARAMETER
//
// MessageText:
//
//  An invalid parameter was passed to a service or function.
//
#define STATUS_INVALID_PARAMETER         ((NTSTATUS)0xC000000DL)

//
// MessageId: STATUS_INSUFFICIENT_RESOURCES
//
// MessageText:
//
//  Insufficient system resources exist to complete the API.
//
#define STATUS_INSUFFICIENT_RESOURCES    ((NTSTATUS)0xC000009AL)     // ntsubauth

//
// MessageId: STATUS_POWER_STATE_INVALID
//
// MessageText:
//
//  Current device power state cannot support this request.
//
#define STATUS_POWER_STATE_INVALID       ((NTSTATUS)0xC00002D3L)

//
// MessageId: STATUS_ALREADY_COMMITTED
//
// MessageText:
//
//  {Already Committed}
//  The specified address range is already committed.
//
#define STATUS_ALREADY_COMMITTED         ((NTSTATUS)0xC0000021L)

//
// MessageId: STATUS_PENDING
//
// MessageText:
//
//  The operation that was requested is pending completion.
//
#define STATUS_PENDING                   ((NTSTATUS)0x00000103L)    // winnt

//
// MessageId: STATUS_UNSUCCESSFUL
//
// MessageText:
//
//  {Operation Failed}
//  The requested operation was unsuccessful.
//
#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)

//
// MessageId: STATUS_INVALID_DEVICE_REQUEST
//
// MessageText:
//
//  The specified request is not a valid operation for the target device.
//
#define STATUS_INVALID_DEVICE_REQUEST    ((NTSTATUS)0xC0000010L)

#endif /*_BASETYPES_H_*/
