 /***********************************************************************
  * Copyright(C) 2006 3DSP Corporation. All Rights Reserved.
  
  * FILENAME		:busenum.h         VERSION:1.0
  * CREATE DATE	:2009/01/16
  * PURPOSE:	This file includes functions related to 802.11, 802.3 protocol 
  *			header transforming when sending data out or receiving data in.
  *
  * AUTHORS:     
  *
  * DECLARATION:  This document contains confidential proprietary information that
  *               is solely for authorized personnel. It is not to be disclosed to
  *               any unauthorized person without prior written consent of 3DSP
  *               Corporation.		
  ***********************************************************************/

#ifndef BUSENUM_H
    #define BUSENUM_H

    #include <linux/types.h>
    #include <linux/version.h>
    //#include <assert.h>
    #include <linux/pci.h>
    #include <linux/module.h>
    #include <linux/workqueue.h>
    #include <linux/init.h>
    #include <linux/list.h>
    #include <linux/skbuff.h>
    #include <linux/netdevice.h>
    #include <linux/wait.h>
    #include <linux/interrupt.h>
    #include <linux/timer.h>
    #include <linux/delay.h>
    #include <linux/sched.h>
    #include <linux/time.h>
    #include <linux/wait.h>
    #include <linux/kernel.h>
    #include <linux/ioctl.h>
    #include <linux/fs.h>

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
        #define LINUX26
    #endif

    #ifdef LINUX26
        #include <linux/jiffies.h>
        #include <linux/firmware.h>
    #endif

    #include "basetype.h"

//    #define DBG // Uncomment to enable debugging

    #define BUS_CONFIG_FILE  "/etc/tdspbus.cfg"
    #define BUS_CONFIG_SECTION  "busparam"
    #define BUS_CFG_AUTOENUM  "AutoEnum"
    #define BUS_CFG_ENUMMODE "EnumMode"

    #define MAX_HARDWARE_IDS_LEN     60

    #define DRV_NAME	"3dspusbbus"


    #define BUS_DEVICE_SERIAL_NO_NULL		0x0
    #define BUS_DEVICE_SERIAL_NO_BT			0x1
    #define BUS_DEVICE_SERIAL_NO_WLAN		0x2
    #define BUS_DEVICE_SERIAL_NO_GPS		0x4
    // Define total device
    #define BUS_DEVICE_TOTAL_COUNT			3
    // Define the enum mode of device
    #define BUS_ENUM_MODE_NONE				0
    #define BUS_ENUM_MODE_BT				BUS_DEVICE_SERIAL_NO_BT
    #define BUS_ENUM_MODE_WLAN				BUS_DEVICE_SERIAL_NO_WLAN
    #define BUS_ENUM_MODE_COMBO			(BUS_ENUM_MODE_BT + BUS_ENUM_MODE_WLAN)
    #define BUS_ENUM_MODE_GPS				BUS_DEVICE_SERIAL_NO_GPS
	
    #define TDSP_RF_WLAN_SUPPORT
    //Define RF Type
    #define BUS_RF_TYPE_DEFAULT				0
    #define BUS_RF_TYPE_WLAN				0x1
    #define BUS_RF_TYPE_GPS					0x2
    #define BUS_RF_TYPE_WLAN_GPS			0x3

    #define HOTKEY_FLAG_OR		0xFF000000
    #define HOTKEY_FLAG_AND		0x00FFFFFF
    #define HOTKEY_FLAG_SHIFT	24
	
    #define BUS_IOCTL_MAGIC        0x6b  // 'k'
    #define BUSENUM_IOCTL(_index_)     _IO(BUS_IOCTL_MAGIC, _index_)

    #define IOCTL_BUSENUM_PLUGIN_HARDWARE				BUSENUM_IOCTL (0x0)
    #define IOCTL_BUSENUM_UNPLUG_HARDWARE			BUSENUM_IOCTL (0x1)
    #define IOCTL_BUSENUM_EJECT_HARDWARE				BUSENUM_IOCTL (0x2)
    #define IOCTL_TOASTER_DONT_DISPLAY_IN_UI_DEVICE    BUSENUM_IOCTL (0x3)
    #define IOCTL_BUSENUM_DISPLAY_IN_USE_DEVICE		BUSENUM_IOCTL (0x4)
    #define IOCTL_BUSENUM_RELOAD_BT_CODE				BUSENUM_IOCTL (0x5)
    #define IOCTL_BUSENUM_RELOAD_WLAN_CODE			BUSENUM_IOCTL (0x6)
    #define IOCTL_BUSENUM_RELOAD_GPS_CODE				BUSENUM_IOCTL (0x7)
    #define IOCTL_BUSENUM_RELOAD_CODE_FROM_FILE		BUSENUM_IOCTL (0xe)
    #define IOCTL_BUSENUM_RELOAD_COMBO_CODE			BUSENUM_IOCTL (0xf)
    #define IOCTL_BUSENUM_SET_INIT_FLAG				BUSENUM_IOCTL (0x10)
    #define IOCTL_BUSENUM_READ_REGISTER				BUSENUM_IOCTL (0x11)
    #define IOCTL_BUSENUM_WRITE_REGISTER				BUSENUM_IOCTL (0x12)
    #define IOCTL_BUSENUM_LOAD_EEPROM					BUSENUM_IOCTL (0x1c)
    #define IOCTL_BUSENUM_WRITE_EEPROM				BUSENUM_IOCTL (0x1d)
    #define IOCTL_BUSENUM_QUERY_AUTO_ENUM_FLAG		BUSENUM_IOCTL (0x1e)
    #define IOCTL_BUSENUM_QUERY_RF_TYPE				BUSENUM_IOCTL (0x1f)
    #define IOCTL_BUSENUM_QUERY_DEVICE_STATE			BUSENUM_IOCTL (0x20)
    #define IOCTL_BUSENUM_QUERY_DEVICE_VERSION		BUSENUM_IOCTL (0x21)
    #define IOCTL_BUSENUM_QUERY_BUS_VERSION			BUSENUM_IOCTL (0x22)
    #define IOCTL_BUSENUM_QUERY_FIRMWARE_VERSION		BUSENUM_IOCTL (0x23)
    #define IOCTL_BUSENUM_QUERY_DSP_START_FLAG		BUSENUM_IOCTL (0x24)
    #define IOCTL_BUSENUM_QUERY_DEVICE_COMNUM		BUSENUM_IOCTL (0x25)
    #define IOCTL_BUSENUM_GET_CHIP_ID					BUSENUM_IOCTL (0x26)
    #define IOCTL_BUSENUM_GET_SUB_SYSTEM_ID_INFO		BUSENUM_IOCTL (0x27)
    #define IOCTL_BUSENUM_QUERY_DEVICE_COMPANYID		BUSENUM_IOCTL (0x28)
    #define IOCTL_BUSENUM_READ_ENUM_MODE				BUSENUM_IOCTL (0x29)
    #define IOCTL_BUSENUM_WRITE_ENUM_MODE			BUSENUM_IOCTL (0x2a)
    #define IOCTL_BUSENUM_QUERY_RESET_FLAG			BUSENUM_IOCTL (0x2b)
    #define IOCTL_BUSENUM_SET_TEST_POWER_ON			BUSENUM_IOCTL (0x2c)
    #define IOCTL_BUSENUM_SET_TEST_POWER_OFF			BUSENUM_IOCTL (0x2d)
    #define IOCTL_BUSENUM_QUERY_BUS_POWER_STATE		BUSENUM_IOCTL (0x2e)
    #define IOCTL_BUSENUM_SET_DEVICE_STATE				BUSENUM_IOCTL (0x2f)
    #define IOCTL_BUSENUM_SET_HOTKEY_FLAG				BUSENUM_IOCTL (0x30)
    #define IOCTL_BUSENUM_QUERY_8051_VERSION			BUSENUM_IOCTL (0x31)
    #define IOCTL_BUSENUM_SET_ANTENNA					BUSENUM_IOCTL (0x32)


    #define IN
    #define OUT

    #define MAX_VERSIONID_LEN     64

	
typedef enum _BUS_POWER_STATE
{
	PowerWirlessOff = 0,  // Power Off
	PowerWirlessOn,  // Power On
	PowerOff2On,  // Power state is changing from off to on
	PowerOn2Off,  // Power state is changing from on to off
	PowerUnKnown  // Power Unknown state
} BUS_POWER_STATE;
	
typedef enum _DEVICE_POWER_STATE
{
 	PowerDeviceUnspecified = 0,
	PowerDeviceD0, 
	PowerDeviceD1, 
	PowerDeviceD2, 
	PowerDeviceD3, 
	PowerDeviceMaximum
} DEVICE_POWER_STATE,  *PDEVICE_POWER_STATE;

typedef enum _SYSTEM_POWER_STATE
{
	PowerSystemUnspecified = 0, 
	PowerSystemWorking, 
	PowerSystemSleeping1, 
	PowerSystemSleeping2, 
	PowerSystemSleeping3, 
	PowerSystemHibernate, 
	PowerSystemShutdown, 
	PowerSystemMaximum
} SYSTEM_POWER_STATE,  *PSYSTEM_POWER_STATE;

typedef enum _BUS_DEVICE_START_DEVICE_STATE
{
	DeviceStartNone = 0,  // No device would be started
	DeviceStartBT,  // Start bluetooth device
	DeviceStartWLAN,  // Start WLAN device
	DeviceStartGPS,  // Start GPS device
	DeviceStartUnKnown  // Start Unknown state
} BUS_DEVICE_START_DEVICE_STATE;
    //
    // These are the states a PDO or FDO transition upon
    // receiving a specific PnP Irp. Refer to the PnP Device States
    // diagram in DDK documentation for better understanding.
    //

typedef enum _DEVICE_PNP_STATE
{
	NotStarted = 0,  // Not started yet
	Started,  // Device has received the START_DEVICE IRP
	StopPending,  // Device has received the QUERY_STOP IRP
	Stopped,  // Device has received the STOP_DEVICE IRP
	RemovePending,  // Device has received the QUERY_REMOVE IRP
	SurpriseRemovePending,  // Device has received the SURPRISE_REMOVE IRP
	Deleted,  // Device has received the REMOVE_DEVICE IRP
	Starting,  // Device is starting but not started yet
	ErrorInitPending,  // Occur error when device is intializing.
	DeviceNone,  // device not exist
	ErrorOccur,  // error occur
	UnKnown,  // Unknown state
	StartWithOS	//init mode, make driver can work with system , only once. after OS startup, state is decided by WB.
} DEVICE_PNP_STATE;

typedef struct _TDSPBUS_INTERFACE_DATA
{
        // A back pointer to the bus
	void *ParentFdo;

	void *pPciDev;
	void *pBtDev;

	// An array of (zero terminated wide character strings).
	// The array itself also null terminated
	unsigned char version[MAX_VERSIONID_LEN]; // bt or wlan driver's version
	
	unsigned long c8051_version;

	unsigned long dsp_version;

	// Unique serail number of the device on the bus
	unsigned char SerialNo; // send to bus driver
	
	unsigned char ResetFlag;	// Reset Flag
	unsigned char ParaFlag;	//from wb config.

	// Device State
	unsigned long devstate; //set by bus driver.

	// Callback operation
	int( *open )( struct _TDSPBUS_INTERFACE_DATA *dev );
	int( *close )( struct _TDSPBUS_INTERFACE_DATA *dev );
	int( *suspend )( struct _TDSPBUS_INTERFACE_DATA *dev, unsigned long state );
	int( *resume )( struct _TDSPBUS_INTERFACE_DATA *dev );
	int( *notify_reset )( struct _TDSPBUS_INTERFACE_DATA *dev, int resetevent );
	int( *notify_initreset )( struct _TDSPBUS_INTERFACE_DATA *dev, int resetevent );
	int( *notify_deatch )( struct _TDSPBUS_INTERFACE_DATA *dev, int reason );
	void *pUsbDev; // usb_device
} TDSPBUS_INTERFACE,  *PTDSPBUS_INTERFACE;

typedef struct _TDSPBUS_DEVICE_DATA
{
//	UCHAR MyVersion[MAX_VERSIONID_LEN];	
	UCHAR SerialNo;
	USHORT CompanyID;	
	ULONG ComNum;
	UCHAR uHotKeyPressed;	
	UCHAR uAntennaFlag;	//1: 0      2: 1        other:auto
	UCHAR state_old_from_wb;	//backup devstate by IO interface.
	
	struct _TDSPBUS_INTERFACE_DATA	*pDeviceInterface;
}TDSPBUS_DEVICE_DATA, *PTDSPBUS_DEVICE_DATA;

typedef struct _FDO_DEVICE_DATA
{
	ULONG chipID;
	ULONG SubSystemIDInfo;	
	ULONG EnumMode;	
	BUS_POWER_STATE BusPowerState;
	
	DEVICE_POWER_STATE DevicePowerState;
	DEVICE_PNP_STATE DevicePnPState;
	UCHAR MyVersion[MAX_VERSIONID_LEN];	
	UCHAR RfType;
	
	struct _TDSPBUS_DEVICE_DATA DeviceData[BUS_DEVICE_TOTAL_COUNT];
} FDO_DEVICE_DATA,  *PFDO_DEVICE_DATA;

typedef struct _BUSINTERFACE_STRUCT
{
        ULONG Size;
        ULONG SerialNo;
        UCHAR HardwareIDs[];
} BUSINTERFACE_STRUCT,  *PBUSINTERFACE_STRUCT;

typedef BUSINTERFACE_STRUCT BUSENUM_PLUGIN_HARDWARE,  *PBUSENUM_PLUGIN_HARDWARE;

typedef struct _BUSENUM_UNPLUG_HARDWARE
{
        IN ULONG Size;
        ULONG SerialNo;
        ULONG Reserved[2];
} BUSENUM_UNPLUG_HARDWARE,  *PBUSENUM_UNPLUG_HARDWARE;

typedef struct _BUSENUM_EJECT_HARDWARE
{
        IN ULONG Size;
        ULONG SerialNo;
        ULONG Reserved[2];
} BUSENUM_EJECT_HARDWARE,  *PBUSENUM_EJECT_HARDWARE;

#define RtlEqualMemory(Destination,Source,Length)		(!memcmp((Destination),(Source),(Length)))
#define RtlMoveMemory(Destination,Source,Length)		memmove((Destination),(Source),(Length))
#define RtlCopyMemory(Destination,Source,Length)		memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill)			memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length)				memset((Destination),0,(Length))

#define tdsp_alloc_mem(size, flag)		kmalloc(size, flag)
#define tdsp_free_mem(pbuffer)			kfree(pbuffer)


#ifdef DBG	
#define Bus_DebugPrint	printk
#define tdspbus_trace()		Bus_DebugPrint( "---%s():%d\n    ", __FUNCTION__, __LINE__)
#else
#define Bus_DebugPrint(a, arg...)
#define tdspbus_trace()
#endif


    #define tdspbus_assert(_x)	do											\
    {														\
    if (!(_x))											\
    {													\
    printk("\n!!!!!!!!!!!!!  Assertion failed : File: %s :%d:",	\
    __FILE__, __LINE__);				\
    printk(# _x);									\
    printk("!!!!!!!!!!!!!!!\n");									\
    printk("%d",*((PSHORT)0));					\
    }													\
    }while(0)

    #define ASSERT tdspbus_assert
    #define ASSERT_BT	tdspbus_assert

    #define EVENT_UNSIGNED  0
    #define EVENT_SIGNED       1
	
#endif
