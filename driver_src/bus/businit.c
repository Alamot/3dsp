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
 ************************************************************************/
#include <linux/pci.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#include "basetype.h"
#include "businit.h"

// This string is displayed in the about box of the WB Utility
#define BUS_STRING_VERSION_ID "BusU090716"

static int tdspbus_major = 0;
static char g_driver_name[] = "3DSP USB Bus Driver";
static char g_complie_datetime[] = "Build " __TIME__ ", " __DATE__;
UCHAR g_dbg_level = 5;

long tdspbus_unlocked_ioctl( struct file *pfile, unsigned int cmd, unsigned long arg );
int tdspbus_open( struct inode *inode, struct file *pfile );
int tdspbus_release( struct inode *inode, struct file *pfile );
void SetGlogalFdoData( PFDO_DEVICE_DATA pFdoData );
FDO_DEVICE_DATA *GetGlogalFdoData( void );
	
static struct file_operations tdspbus_fops =
{
    .owner = THIS_MODULE, .unlocked_ioctl = tdspbus_unlocked_ioctl, .open = tdspbus_open, .release = tdspbus_release,
};

VOID TDSP_GetRfType( PFDO_DEVICE_DATA FdoData )
{
    FdoData->RfType = 0;
    #ifdef TDSP_RF_WLAN_SUPPORT
        FdoData->RfType += BUS_RF_TYPE_WLAN;
    #endif

    #ifdef TDSP_RF_GPS_SUPPORT
        FdoData->RfType += BUS_RF_TYPE_GPS;
    #endif
}


PTDSPBUS_DEVICE_DATA Bus_FindPdoBySerialNo(PFDO_DEVICE_DATA FdoData, ULONG SerialNo)
{
	UINT32 i;
	
	for (i = 0; i < BUS_DEVICE_TOTAL_COUNT; i++)
	{
		if (FdoData->DeviceData[i].SerialNo == SerialNo)
		{
			if (FdoData->DeviceData[i].pDeviceInterface != NULL)
				return &FdoData->DeviceData[i];
			else
				return NULL;
		}
	}

	return NULL;
}

NTSTATUS Bus_PlugInDevice( PBUSENUM_PLUGIN_HARDWARE PlugIn, ULONG PlugInSize, PFDO_DEVICE_DATA FdoData )
{
	PTDSPBUS_DEVICE_DATA pdoData = NULL;
	ULONG serialNo;
	int ret;

	serialNo = PlugIn->SerialNo;
	
	Bus_DebugPrint( "%s():%d trace! %lx\n", __FUNCTION__, __LINE__ , serialNo);
	PlugIn->SerialNo = PlugIn->SerialNo & HOTKEY_FLAG_AND;		
	
	if (( PlugIn->SerialNo == BUS_DEVICE_SERIAL_NO_BT ) || ( PlugIn->SerialNo == BUS_DEVICE_SERIAL_NO_WLAN ))
	{
		if ( FdoData->RfType == BUS_RF_TYPE_GPS )
		{
			Bus_DebugPrint( "%s():%d RfType = %d, but SerialNo = %ld\n", __FUNCTION__, __LINE__ , FdoData->RfType, PlugIn->SerialNo);
			return STATUS_INVALID_PARAMETER;
		}
	}
	else if ( PlugIn->SerialNo == BUS_DEVICE_SERIAL_NO_GPS )
	{
		if ( FdoData->RfType == BUS_RF_TYPE_WLAN )
		{
			Bus_DebugPrint( "%s():%d RfType = %d, but SerialNo = %ld\n", __FUNCTION__, __LINE__ , FdoData->RfType, PlugIn->SerialNo);
			return STATUS_INVALID_PARAMETER;
		}
	}
	
	if (FdoData->DevicePnPState == NotStarted)
	{
		Bus_DebugPrint( "%s():%d fail! DevicePnPState = %d, SerialNo = %ld\n", __FUNCTION__, __LINE__ , FdoData->DevicePnPState, PlugIn->SerialNo);
		return STATUS_NO_SUCH_DEVICE;
	}

	pdoData = Bus_FindPdoBySerialNo(FdoData, PlugIn->SerialNo);
	
	if (pdoData == NULL)
	{
		Bus_DebugPrint("%s():%d fail! SerialNo = %ld not found\n", __FUNCTION__, __LINE__ , PlugIn->SerialNo);
		return STATUS_NO_SUCH_DEVICE;
	}
	
	if (pdoData->pDeviceInterface->devstate == Started)
	{
		Bus_DebugPrint("%s():%d fail! SerialNo = %ld devstate(%ld) already is Started \n", __FUNCTION__, __LINE__ , PlugIn->SerialNo, pdoData->pDeviceInterface->devstate);
		return STATUS_INVALID_PARAMETER;
	}
	
	pdoData->pDeviceInterface->pPciDev = NULL;//( void* )FdoData->p_pcidev;

	ret = pdoData->pDeviceInterface->open(pdoData->pDeviceInterface);	
	if (ret == 0)
	{
		pdoData->pDeviceInterface->devstate = Started;
		pdoData->state_old_from_wb = pdoData->pDeviceInterface->devstate;
	}
	return 0;
}


NTSTATUS Bus_UnPlugDevice( PBUSENUM_UNPLUG_HARDWARE UnPlug, PFDO_DEVICE_DATA FdoData )
{
	BOOLEAN plugOutAll;
	PTDSPBUS_DEVICE_DATA pdoData = NULL;
	int ret;

	plugOutAll = (0 == UnPlug->SerialNo);

	if (plugOutAll)
	{
		Bus_DebugPrint( "%s():%d trace! Plugging out all the devices, SerialNo = %lx\n", __FUNCTION__, __LINE__ , UnPlug->SerialNo);
	}
	else
	{
		Bus_DebugPrint( "%s():%d trace! Plugging out %lu\n", __FUNCTION__, __LINE__ , UnPlug->SerialNo);
	}

	if (plugOutAll)
	{	
		UINT32 i;
		for (i = 0; i < BUS_DEVICE_TOTAL_COUNT; i++)
		{
			if (FdoData->DeviceData[i].pDeviceInterface != NULL)
			{
				UnPlug->SerialNo = FdoData->DeviceData[i].SerialNo;
				Bus_UnPlugDevice(UnPlug, FdoData);
			}
		}
	}
	else
	{
		pdoData = Bus_FindPdoBySerialNo(FdoData, UnPlug->SerialNo & HOTKEY_FLAG_AND);
		
		if (pdoData == NULL)
		{
			Bus_DebugPrint("%s():%d fail! SerialNo = %ld not found\n", __FUNCTION__, __LINE__ , UnPlug->SerialNo);
			return STATUS_NO_SUCH_DEVICE;
		}

		if (pdoData->pDeviceInterface->devstate == NotStarted)
		{
			Bus_DebugPrint("%s():%d fail! SerialNo = %ld devstate is NotStarted\n", __FUNCTION__, __LINE__ , UnPlug->SerialNo);
			return STATUS_NO_SUCH_DEVICE;
		}
		
		pdoData->state_old_from_wb = pdoData->pDeviceInterface->devstate;		
		ret = pdoData->pDeviceInterface->close(pdoData->pDeviceInterface);	//whk		
		if (ret == 0)
		{
			pdoData->pDeviceInterface->devstate = Stopped;
			pdoData->state_old_from_wb = pdoData->pDeviceInterface->devstate;
		}
	}

	return 0;
}


VOID TDSP_SetBusPowerState( PFDO_DEVICE_DATA FdoData, BUS_POWER_STATE PowerState )
{
	FdoData->BusPowerState = PowerState;
}

BUS_POWER_STATE TDSP_GetCurrentBusPowerState( PFDO_DEVICE_DATA FdoData )
{
	return ( FdoData->BusPowerState );
}

/*
 * Name			: tdspbus_init_module
 *				:
 * Parameters	: none
 *				:
 * Return value	: type: unsigned long
 *				: description: Module init SUCCESS/FAILURE
 *				:
 * Description	: This Procedure network driver module initalization
 *				:
 */
static int __init tdspbus_init_module( void )
{
    int ret = 0;
    FDO_DEVICE_DATA *FdoData;

    Bus_DebugPrint( "\n" );
    Bus_DebugPrint( "**************                                *******************\n" );
    Bus_DebugPrint( "**************        3DSP BUS Driver         *******************\n" );
    Bus_DebugPrint( "**************                                *******************\n" );
    Bus_DebugPrint( "\n" );
    Bus_DebugPrint( "%s() enter!\n", __FUNCTION__ );
    Bus_DebugPrint( "%s   %s\n", g_driver_name, g_complie_datetime );
    Bus_DebugPrint( "   Version : %s\n", BUS_STRING_VERSION_ID );
    Bus_DebugPrint( "\n" );

    if ( !( FdoData = kmalloc( sizeof( FDO_DEVICE_DATA ), GFP_KERNEL )))
    {
        Bus_DebugPrint("%s: kmalloc() failed!\n", DRV_NAME );
        return  - 1;
    }

    RtlFillMemory( FdoData, sizeof(FDO_DEVICE_DATA), 0);
    SetGlogalFdoData( FdoData );

    FdoData->DevicePowerState = PowerDeviceD0;
    FdoData->DevicePnPState = NotStarted;
    TDSP_GetRfType(FdoData);
    RtlCopyMemory( FdoData->MyVersion, BUS_STRING_VERSION_ID, sizeof( BUS_STRING_VERSION_ID ));
	
    FdoData->DeviceData[0].SerialNo = BUS_DEVICE_SERIAL_NO_BT;
    FdoData->DeviceData[1].SerialNo = BUS_DEVICE_SERIAL_NO_WLAN;
    FdoData->DeviceData[2].SerialNo = BUS_DEVICE_SERIAL_NO_GPS;
	
    tdspbus_major = register_chrdev( 0, DRV_NAME, &tdspbus_fops );
    if ( tdspbus_major < 0 )
    {
        kfree(FdoData);
        Bus_DebugPrint( "Fatal Error: register_chrdev() failed, aborting!\n\n" );
        return  tdspbus_major;
    }

    Bus_DebugPrint( "%s() exit,ret=%d!\n", __FUNCTION__, ret );
    return ret;
}


/*
 * Name			: m3dsp_module_exit
 *				:
 * Parameters	: none
 *				:
 * Return value	: type: void
 *				: description:
 *				:
 * Description	: This Procedure unregister network driver module from
 *				: PCT region
 *				:
 * Notice 		:
 * See also		:
 * TODO			:
 */
static void __exit tdspbus_cleanup_module( void )
{
    FDO_DEVICE_DATA *FdoData;

    Bus_DebugPrint( "%s enter!\n", __FUNCTION__ );
	
    FdoData = GetGlogalFdoData();
	
    unregister_chrdev( tdspbus_major, DRV_NAME );	
//    RemoveAllDevice( FdoData );
    kfree( FdoData );

    Bus_DebugPrint( "%s Exit!\n", __FUNCTION__ );
}



int register_tdspbus_usbdev( PTDSPBUS_INTERFACE pInterface )
{
    PFDO_DEVICE_DATA FdoData;
    int ret, i;

    if ( pInterface == NULL )
    {
        Bus_DebugPrint( "%s() failed because of NULL pointer!\n", __FUNCTION__ );
        return  STATUS_INVALID_PARAMETER;
    }

    Bus_DebugPrint( "%s be called,SerialNo = %d,version=%s!\n\n", __FUNCTION__,  ( int )pInterface->SerialNo, pInterface->version);

    FdoData = GetGlogalFdoData();

    if (pInterface->SerialNo == BUS_DEVICE_SERIAL_NO_NULL)
    {
            Bus_DebugPrint( "%s: pInterface->SerialNo = %d, error!\n",__FUNCTION__,  BUS_DEVICE_SERIAL_NO_NULL);
            return ( STATUS_INVALID_PARAMETER );
    }
    else if (( pInterface->SerialNo == BUS_DEVICE_SERIAL_NO_BT ) || ( pInterface->SerialNo == BUS_DEVICE_SERIAL_NO_WLAN ))
    {
        if ( FdoData->RfType == BUS_RF_TYPE_GPS )
        {
            Bus_DebugPrint( "%s: RfType error!\n", __FUNCTION__ );
            return ( STATUS_INVALID_PARAMETER );
        }
    }
    else if ( pInterface->SerialNo == BUS_DEVICE_SERIAL_NO_GPS )
    {
        if ( FdoData->RfType == BUS_RF_TYPE_WLAN )
        {
            Bus_DebugPrint( "%s: RfType error!\n", __FUNCTION__ );
            return ( STATUS_INVALID_PARAMETER );
        }
    }

    if (FdoData->DevicePowerState != PowerDeviceD0)
    {
        Bus_DebugPrint( "%s failed because of inactived Power State!\n", __FUNCTION__ );
        return ( STATUS_INVALID_PARAMETER );
    }

    if (( pInterface->open == NULL ) || ( pInterface->close == NULL ))// || ( pInterface->suspend == NULL ) || ( pInterface->resume == NULL ) || ( pInterface->notify_interrupt == NULL ) || ( pInterface->notify_reset == NULL ) || ( pInterface->notify_initreset == NULL ) || ( pInterface->notify_completeinit == NULL ) || ( pInterface->notify_deatch == NULL ))
    {
        Bus_DebugPrint( "%s failed because of NULL function callback pointer!\n", __FUNCTION__ );
        return ( STATUS_INVALID_PARAMETER );
    }

    ret = 0;

    for (i = 0; i < BUS_DEVICE_TOTAL_COUNT; i++)
    {
        if (FdoData->DeviceData[i].SerialNo == pInterface->SerialNo)
        {
        	if (FdoData->DeviceData[i].pDeviceInterface == pInterface)
      		{
			ret = EEXIST;
			Bus_DebugPrint( "%s: %s exist %p\n", g_driver_name, __FUNCTION__, pInterface);
			return -1;
      		}
		else if (FdoData->DeviceData[i].pDeviceInterface != NULL)
		{
			ret = EEXIST;
			Bus_DebugPrint( "%s: %s not NULL %p\n", g_driver_name, __FUNCTION__, FdoData->DeviceData[i].pDeviceInterface);
			return -1;
		}
			
		break;
        }
    }
	
    if (i >= BUS_DEVICE_TOTAL_COUNT)
    {
        Bus_DebugPrint( "%s: %s() error, SerialNo = %d is not supported!\n", g_driver_name, __FUNCTION__, ( int )pInterface->SerialNo );
        return -1;
    }


    pInterface->devstate = NotStarted;
    pInterface->ResetFlag = 0;
    pInterface->ParentFdo = (void *)FdoData;
//    pInterface->BaseRegisterAddress = NULL;//FdoData->BaseRegisterAddress;
    pInterface->pPciDev = NULL;//( void* )FdoData->p_pcidev;
    FdoData->DeviceData[i].pDeviceInterface = pInterface;
    FdoData->DeviceData[i].ComNum = 0;
    FdoData->DeviceData[i].CompanyID = 0;
    FdoData->DevicePnPState = Started;
    pInterface->ParaFlag = FdoData->DeviceData[i].uHotKeyPressed | (FdoData->DeviceData[i].uAntennaFlag << 1);
	
    TDSP_SetBusPowerState(FdoData, PowerWirlessOn);

	if ((FdoData->DeviceData[i].state_old_from_wb == Started)
		|| (FdoData->DeviceData[i].state_old_from_wb == StartWithOS))
	{
		ret = FdoData->DeviceData[i].pDeviceInterface->open(pInterface);	//whk
		if (ret == 0)
			FdoData->DeviceData[i].pDeviceInterface->devstate = Started;
	}
	
    Bus_DebugPrint( "Exit %s with success, SerialNo = %d!state_old_from_wb = %d, devstate = %ld ParaFlag = %x\n\n",__FUNCTION__,  
		pInterface->SerialNo, FdoData->DeviceData[i].state_old_from_wb, 
		FdoData->DeviceData[i].pDeviceInterface->devstate, pInterface->ParaFlag);

    return 0;
}

int unregister_tdspbus_usbdev( PTDSPBUS_INTERFACE pInterface )
{
	FDO_DEVICE_DATA *FdoData;
	PTDSPBUS_DEVICE_DATA pdoData = NULL;
	int i;
	int ret;
	int close_flag = 0;

    if ( pInterface == NULL )
    {
        Bus_DebugPrint( "%s failed because of NULL pointer!\n", __FUNCTION__ );
        return  STATUS_INVALID_PARAMETER;
    }
	
    if (pInterface->SerialNo == BUS_DEVICE_SERIAL_NO_NULL)
    {
            Bus_DebugPrint( "%s: pInterface->SerialNo = %d, error!\n", __FUNCTION__, BUS_DEVICE_SERIAL_NO_NULL);
            return STATUS_INVALID_PARAMETER;
    }

    FdoData = GetGlogalFdoData();

    if (FdoData->DevicePowerState != PowerDeviceD0)
    {
        Bus_DebugPrint( "%s() failed because of inactived Power State!\n", __FUNCTION__ );
    }

    Bus_DebugPrint( "%s be called,SerialNo = %d,version=%s!\n\n", __FUNCTION__, ( int )pInterface->SerialNo, pInterface->version );

	pdoData = Bus_FindPdoBySerialNo(FdoData, pInterface->SerialNo);

	if (pdoData == NULL)
	{
		Bus_DebugPrint("%s():%d fail! SerialNo = %d not found\n", __FUNCTION__, __LINE__ , pInterface->SerialNo);
		return STATUS_NO_SUCH_DEVICE;
	}

	if (pdoData->pDeviceInterface == pInterface)
	{
		if (pdoData->pDeviceInterface->devstate == Started)
			close_flag = 1;
		pdoData->pDeviceInterface = NULL;
	}


	ret = 0;
	for (i = 0; i < BUS_DEVICE_TOTAL_COUNT; i++)
	{
		if (FdoData->DeviceData[i].pDeviceInterface != NULL)
			break;
	}
	
	if (i >= BUS_DEVICE_TOTAL_COUNT)
	{
		FdoData->DevicePnPState = NotStarted;
		TDSP_SetBusPowerState(FdoData, PowerWirlessOff);
	}

	if (close_flag == 1)	//if bluetooth, close func may be a long time.
		pInterface->close(pInterface);

	Bus_DebugPrint( "%s EXIT\n", __FUNCTION__);
	return 0;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,10)
    MODULE_LICENSE( "GPL" );
#endif
MODULE_AUTHOR( "3DSP" );
MODULE_DESCRIPTION( "3DSP usb bus driver" );

EXPORT_SYMBOL( register_tdspbus_usbdev );
EXPORT_SYMBOL( unregister_tdspbus_usbdev );

module_init( tdspbus_init_module );
module_exit( tdspbus_cleanup_module );
