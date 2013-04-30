 /***********************************************************************
  * Copyright(C) 2006 3DSP Corporation. All Rights Reserved.
  
  * FILENAME		:busioctl.c         VERSION:1.0
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
#include "businit.h"


static FDO_DEVICE_DATA *ptdspbus_myself = NULL;

NTSTATUS Bus_UnPlugDevice( PBUSENUM_UNPLUG_HARDWARE UnPlug, PFDO_DEVICE_DATA FdoData );
NTSTATUS Bus_PlugInDevice( PBUSENUM_PLUGIN_HARDWARE PlugIn, ULONG PlugInSize, PFDO_DEVICE_DATA FdoData );
PTDSPBUS_DEVICE_DATA Bus_FindPdoBySerialNo(PFDO_DEVICE_DATA FdoData, ULONG SerialNo);

FDO_DEVICE_DATA *GetGlogalFdoData( void )
{
    return ( ptdspbus_myself );
}


void SetGlogalFdoData( PFDO_DEVICE_DATA pFdoData )
{
    ASSERT( pFdoData != NULL );

    ptdspbus_myself = pFdoData;
    return ;
}


int tdspbus_open( struct inode *inode, struct file *pfile )
{
    PFDO_DEVICE_DATA fdoData;

//    Bus_DebugPrint( "%s is called!\n", __FUNCTION__ );
    fdoData = GetGlogalFdoData();

    //
    // Check to see whether the bus is removed
    //

    if ( fdoData->DevicePnPState != Started )
    {
    }

    if ( fdoData->DevicePowerState != PowerDeviceD0 )
    {
    }

    pfile->private_data = fdoData;
    return STATUS_SUCCESS;
}

int tdspbus_release( struct inode *inode, struct file *pfile )
{
    NTSTATUS status;
    PFDO_DEVICE_DATA fdoData;

//	Bus_DebugPrint( "%s is called!\n", __FUNCTION__ );

    fdoData = GetGlogalFdoData();

    //
    // Check to see whether the bus is removed
    //

    if ( fdoData->DevicePnPState != Started )
    {
        status = STATUS_NO_SUCH_DEVICE;
        return status;
    }

    //
    // Check to see whether the bus enter power save (hibernation or standby)
    //
    if ( fdoData->DevicePowerState != PowerDeviceD0 )
    {
        Bus_DebugPrint( "tdspbus_release is called but system is not active!\n" );
        //
        // These commands are only allowed to go to the FDO.
        //
        status = STATUS_POWER_STATE_INVALID;
        return status;
    }

    pfile->private_data = NULL;
    status = STATUS_SUCCESS;

    return status;
}

int tdspbus_unlocked_ioctl( struct file *pfile, unsigned int cmd, unsigned long arg )
{
    NTSTATUS status;
    ULONG inlen, outlen;
    PFDO_DEVICE_DATA fdoData;
    PVOID buffer;

//	Bus_DebugPrint( "%s is called!\n", __FUNCTION__ );
	
    fdoData = GetGlogalFdoData();

    if ( pfile->private_data != fdoData )
    {
        Bus_DebugPrint( "tdspbus_unlocked_ioctl is called,param verify failed!file=0x%p.\n", pfile );
	return  - EFAULT;
    }

    if (( fdoData->DevicePnPState != Started )
		&& ((cmd != IOCTL_BUSENUM_SET_HOTKEY_FLAG) && (cmd != IOCTL_BUSENUM_SET_ANTENNA)))
    {
	Bus_DebugPrint("%d cmd = %x\n", fdoData->DevicePnPState, cmd - IOCTL_BUSENUM_PLUGIN_HARDWARE);
	return  - EFAULT;
    }

    if ( fdoData->DevicePowerState != PowerDeviceD0 )
    {
        Bus_DebugPrint( "tdspbus_unlocked_ioctl is called but the power state of fdo is not actived!fdoData=0x%p.\n", fdoData );
	return  - EFAULT;
    }

    if ((( cmd &IOCTL_BUSENUM_DISPLAY_IN_USE_DEVICE ) != IOCTL_BUSENUM_DISPLAY_IN_USE_DEVICE ) && (( cmd &IOCTL_BUSENUM_QUERY_BUS_POWER_STATE ) != IOCTL_BUSENUM_QUERY_BUS_POWER_STATE ))
    {
	Bus_DebugPrint( "%s cmd = %x\n", __FUNCTION__ , cmd - IOCTL_BUSENUM_PLUGIN_HARDWARE);
    }

    buffer = ( PVOID )arg;
    inlen = outlen = 0;

    status = STATUS_INVALID_PARAMETER;
	
    switch ( cmd )
    {
        case IOCTL_BUSENUM_PLUGIN_HARDWARE:
		tdspbus_trace();
            {
                BUSENUM_PLUGIN_HARDWARE plugin_hardware;
                ULONG len;

                Bus_DebugPrint( "3DSP Bus Driver PlugIn called!\n" );

                if ( !copy_from_user( &len, ( PVOID )arg, sizeof( ULONG )))
                {
                    if ( !copy_from_user( &plugin_hardware, ( PVOID )arg, len ))
                    {
                        status = Bus_PlugInDevice( &plugin_hardware, len, fdoData );
                    }
                }
                break;
            }

        case IOCTL_BUSENUM_UNPLUG_HARDWARE:
		tdspbus_trace();
            {
                BUSENUM_UNPLUG_HARDWARE unplugin_hardware;
                ULONG len;

                if ( !copy_from_user( &len, ( PVOID )arg, sizeof( ULONG )))
                {
                    if ( !copy_from_user( &unplugin_hardware, ( PVOID )arg, len ))
                    {
                        status = Bus_UnPlugDevice( &unplugin_hardware, fdoData );
                    }
                }
                break;
            }

	case IOCTL_BUSENUM_QUERY_BUS_POWER_STATE:
//		tdspbus_trace();
		{
                    *( PULONG )buffer = ( ULONG )fdoData->BusPowerState;
                    if ( copy_to_user(( PVOID )arg, buffer, sizeof( ULONG )))
                        return  - EFAULT;
                    status = STATUS_SUCCESS;
//			Bus_DebugPrint( "IOCTL_BUSENUM_QUERY_BUS_POWER_STATE, PowerState=%d!\n", fdoData->BusPowerState);
		}
		break;

        case IOCTL_BUSENUM_EJECT_HARDWARE:
		tdspbus_trace();
            {
#if 0		
                BUSENUM_EJECT_HARDWARE eject_hardware;
                ULONG len;

                if ( !copy_from_user( &len, ( PVOID )arg, sizeof( ULONG )))
                {
                    if ( !copy_from_user( &eject_hardware, ( PVOID )arg, len ))
                    {
                        status = Bus_UnPlugDevice( &eject_hardware, fdoData );
                    }
                }
#endif
                break;
            }

        case IOCTL_BUSENUM_DISPLAY_IN_USE_DEVICE:
//		tdspbus_trace();
		{
			ULONG val = 0;
			PTDSPBUS_DEVICE_DATA pdoData;
			UCHAR i;

			if ( copy_to_user(( PVOID )arg, &val, sizeof( val )))
				return  - EFAULT;

			for ( i = 0; i < BUS_DEVICE_TOTAL_COUNT; i++ )
			{
				pdoData = &fdoData->DeviceData[i];
				if ( pdoData->pDeviceInterface != NULL)
				{
					if (pdoData->pDeviceInterface->devstate == Started)
					{
						val |= pdoData->SerialNo;
					}
					else
					{
						val |= 0xff00;
					}
				}
			}

			if ( val != 0 )
			{
				if ((val & 0xff) == 0)
					val = 0xffff;
				else
					val &= 0x00ff;
			}
			else
			{
				val = 0x10000;
			}
			
			if ( copy_to_user(( PVOID )arg, &val, sizeof( val )))
				return  - EFAULT;

//			Bus_DebugPrint( "%s IOCTL_BUSENUM_DISPLAY_IN_USE_DEVICE val = %lx\n", __FUNCTION__ , val);
			status = STATUS_SUCCESS;
			break;
        	}
        case IOCTL_BUSENUM_RELOAD_BT_CODE:
		tdspbus_trace();
            #ifdef TDSP_BUS_DRIVER_OTHER_DSP_CODES
                {
                    PPDO_DEVICE_DATA pdoData;
                    PSP20_CODE pCodeBuffer1;
                    UCHAR LoadSp20FromFileFlagBak;

                    if ( fdoData->chipID )
                    {
                        pCodeBuffer1 = ( PSP20_CODE )SP20Code1_v3;
                    }
                    else
                    {
                        pCodeBuffer1 = ( PSP20_CODE )SP20Code1;
                    }

                    LoadSp20FromFileFlagBak = fdoData->LoadSp20FromFileFlag;
                    fdoData->LoadSp20FromFileFlag = 0;

                    pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_BT );
                    if ( pdoData != NULL )
                    {
                        pdoData->DeviceInitCompFlag = 0;
                    }
                    status = TDSP_ConfigHardwareWithCodeBuf( fdoData, pCodeBuffer1 );
                    fdoData->LoadSp20FromFileFlag = LoadSp20FromFileFlagBak;
                }
            #else
                status = STATUS_SUCCESS;
            #endif
            break;

        case IOCTL_BUSENUM_RELOAD_WLAN_CODE:
		tdspbus_trace();
            #ifdef TDSP_BUS_DRIVER_OTHER_DSP_CODES
                {
                    PPDO_DEVICE_DATA pdoData;
                    PSP20_CODE pCodeBuffer2;
                    UCHAR LoadSp20FromFileFlagBak;

                    if ( fdoData->chipID )
                    {
                        pCodeBuffer2 = ( PSP20_CODE )SP20Code2_v3;
                    }
                    else
                    {
                        pCodeBuffer2 = ( PSP20_CODE )SP20Code2;
                    }

                    LoadSp20FromFileFlagBak = fdoData->LoadSp20FromFileFlag;
                    fdoData->LoadSp20FromFileFlag = 0;

                    pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_WLAN );
                    if ( pdoData != NULL )
                    {
                        pdoData->DeviceInitCompFlag = 0;
                    }
                    status = TDSP_ConfigHardwareWithCodeBuf( fdoData, pCodeBuffer2 );
                    fdoData->LoadSp20FromFileFlag = LoadSp20FromFileFlagBak;
                }
            #else
                status = STATUS_SUCCESS;
            #endif
            break;

        case IOCTL_BUSENUM_RELOAD_COMBO_CODE:
		tdspbus_trace();
            #ifdef TDSP_BUS_DRIVER_CONTROL_HARDWARE
                {
                    PPDO_DEVICE_DATA pdoData;
                    PSP20_CODE pCodeBuffer;
                    UCHAR LoadSp20FromFileFlagBak;

                    if ( fdoData->chipID )
                    {
                        pCodeBuffer = ( PSP20_CODE )SP20Code_v3;
                    }
                    else
                    {
                        pCodeBuffer = ( PSP20_CODE )SP20Code;
                    }

                    LoadSp20FromFileFlagBak = fdoData->LoadSp20FromFileFlag;
                    fdoData->LoadSp20FromFileFlag = 0;

                    pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_BT );
                    if ( pdoData != NULL )
                    {
                        pdoData->DeviceInitCompFlag = 0;
                    }
                    pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_WLAN );
                    if ( pdoData != NULL )
                    {
                        pdoData->DeviceInitCompFlag = 0;
                    }
                    status = TDSP_ConfigHardwareWithCodeBuf( fdoData, pCodeBuffer );
                    fdoData->LoadSp20FromFileFlag = LoadSp20FromFileFlagBak;
                }
            #else
                status = STATUS_SUCCESS;
            #endif
            break;

        case IOCTL_BUSENUM_RELOAD_GPS_CODE:
		tdspbus_trace();
            #ifdef TDSP_BUS_DRIVER_OTHER_DSP_CODES
                {
                    PPDO_DEVICE_DATA pdoData;
                    PSP20_CODE pCodeBuffer1;
                    UCHAR LoadSp20FromFileFlagBak;


                    pCodeBuffer1 = ( PSP20_CODE )SP20CodeGPS;

                    LoadSp20FromFileFlagBak = fdoData->LoadSp20FromFileFlag;
                    fdoData->LoadSp20FromFileFlag = 0;

                    pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_GPS );
                    if ( pdoData != NULL )
                    {
                        pdoData->DeviceInitCompFlag = 0;
                    }
                    status = TDSP_ConfigHardwareWithCodeBuf( fdoData, pCodeBuffer1 );
                    fdoData->LoadSp20FromFileFlag = LoadSp20FromFileFlagBak;
                }
            #else
                status = STATUS_SUCCESS;
            #endif
            break;

        case IOCTL_BUSENUM_RELOAD_CODE_FROM_FILE:
		tdspbus_trace();
            #ifdef TDSP_BUS_DRIVER_CONTROL_HARDWARE
                {
                    PPDO_DEVICE_DATA pdoData;
                    UCHAR i;

                    fdoData->LoadSp20FromFileFlag = 1;

                    for ( i = 0; i < BUS_DEVICE_TOTAL_COUNT; i++ )
                    {
                        pdoData = Bus_FindPdoBySerialNo( fdoData, 0x1 << i );
                        if ( pdoData != NULL )
                        {
                            pdoData->DeviceInitCompFlag = 0;
                        }
                    }

                    status = TDSP_ConfigHardwareCodeFromFile( fdoData );
                }
            #else
                status = STATUS_SUCCESS;
            #endif
            break;

        case IOCTL_BUSENUM_SET_INIT_FLAG:
		tdspbus_trace();
            {
#if 0
			PPDO_DEVICE_DATA pdoData;
                    ULONG SerialNo;
                    //ULONG len;

                    if ( copy_from_user( &SerialNo, ( PVOID )arg, sizeof( ULONG )))
                        break;

                    pdoData = Bus_FindPdoBySerialNo( fdoData, SerialNo );
                    if ( pdoData != NULL )
                    {
                        pdoData->DeviceInitCompFlag = 1;
                        Bus_DebugPrint( "pdo[0x%p] Current initflag: %d\n", pdoData, pdoData->DeviceInitCompFlag );
                    }
#endif					
                    status = STATUS_SUCCESS;
            }
            break;

	case IOCTL_BUSENUM_QUERY_DEVICE_STATE:
		tdspbus_trace();
	{
		ULONG val = 0xff;
		PTDSPBUS_DEVICE_DATA pdoData;
		ULONG SerialNo;

		if ( copy_from_user( &SerialNo, ( PVOID )arg, sizeof( ULONG )))
			break;

		pdoData = Bus_FindPdoBySerialNo(fdoData, SerialNo);
		if ( pdoData != NULL )
		{
			if (pdoData->pDeviceInterface->devstate == Started)
				val = ( ULONG )Started;
		}
		Bus_DebugPrint( "query_device_state serial no = %lu, return value = %x\n", SerialNo, ( int )val );
		status = STATUS_SUCCESS;
		
		if ( copy_to_user(( PVOID )arg, &val, sizeof( ULONG )))
			return  - EFAULT;

		break;
	}
        case IOCTL_BUSENUM_QUERY_DEVICE_VERSION:
		tdspbus_trace();
            {
                    PTDSPBUS_DEVICE_DATA pdoData;
                    ULONG SerialNo;

                    if ( copy_from_user( &SerialNo, ( PVOID )arg, sizeof( ULONG )))
                        break;

                    pdoData = Bus_FindPdoBySerialNo( fdoData, SerialNo );
                    if ( pdoData != NULL )
                    {
                        if ( copy_to_user(( PVOID )arg, pdoData->pDeviceInterface->version, MAX_VERSIONID_LEN))
                            return  - EFAULT;
                        status = STATUS_SUCCESS;
                        //Irp->IoStatus.Information = MAXVERSIONLENTH;
                    }
                    else
                    {
                        SerialNo = 0;
                        if ( copy_to_user(( PVOID )arg, &SerialNo, sizeof( SerialNo )))
                            return  - EFAULT;
                        status = STATUS_SUCCESS;
                        //Irp->IoStatus.Information = 0;
                    }
            }
            break;

        case IOCTL_BUSENUM_QUERY_BUS_VERSION:
		tdspbus_trace();
            {
                if ( copy_to_user(( PVOID )arg, fdoData->MyVersion, MAX_VERSIONID_LEN ))
                    return  - EFAULT;
                status = STATUS_SUCCESS;
            }
            break;

        case IOCTL_BUSENUM_QUERY_FIRMWARE_VERSION:
		tdspbus_trace();
		{
			PTDSPBUS_DEVICE_DATA pdoData;

			pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_BT);
			if ( pdoData == NULL )
				pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_WLAN);
			
			if ( pdoData != NULL )
			{
				Bus_DebugPrint( "8051_v = %lx, dsp_v = %lx\n", pdoData->pDeviceInterface->c8051_version, pdoData->pDeviceInterface->dsp_version);
				if ( copy_to_user(( PVOID )arg, &pdoData->pDeviceInterface->dsp_version, sizeof( unsigned long )))
					return  - EFAULT;
                    	}
			status = STATUS_SUCCESS;
		}
		break;
		
        case IOCTL_BUSENUM_QUERY_8051_VERSION:
		tdspbus_trace();
		{
			PTDSPBUS_DEVICE_DATA pdoData;

			pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_BT);
			if ( pdoData == NULL )
				pdoData = Bus_FindPdoBySerialNo( fdoData, BUS_DEVICE_SERIAL_NO_WLAN);
			
			if ( pdoData != NULL )
			{
				Bus_DebugPrint( "8051_v = %lx, dsp_v = %lx\n", pdoData->pDeviceInterface->c8051_version, pdoData->pDeviceInterface->dsp_version);
				if ( copy_to_user(( PVOID )arg, &pdoData->pDeviceInterface->c8051_version, sizeof( unsigned long )))
					return  - EFAULT;
                    	}
			status = STATUS_SUCCESS;
		}
		break;
        case IOCTL_BUSENUM_SET_HOTKEY_FLAG:
		tdspbus_trace();
		{
			ULONG hotkey_flag;
			UCHAR i;
			
			if ( copy_from_user( &hotkey_flag, ( PVOID )arg, sizeof( ULONG )))
				break;

			Bus_DebugPrint( "hotkey_flag is %lu\n", hotkey_flag );
			for (i = 0; i < BUS_DEVICE_TOTAL_COUNT; i++)
			{
				fdoData->DeviceData[i].uHotKeyPressed = (UCHAR)(hotkey_flag & 0x1);
				fdoData->DeviceData[i].state_old_from_wb = StartWithOS;
				if (fdoData->DeviceData[i].pDeviceInterface != NULL)
				{
					fdoData->DeviceData[i].pDeviceInterface->ParaFlag &= (~0x1);
					fdoData->DeviceData[i].pDeviceInterface->ParaFlag |= fdoData->DeviceData[i].uHotKeyPressed;
					Bus_DebugPrint( "SerialNo=%d ParaFlag=%d\n", 
						fdoData->DeviceData[i].SerialNo,
						fdoData->DeviceData[i].pDeviceInterface->ParaFlag);
				}
			}			
			status = STATUS_SUCCESS;
		}
		break;		
	case IOCTL_BUSENUM_SET_ANTENNA:
		tdspbus_trace();
		{
			ULONG antenna_flag;
			UCHAR i;
			
			if ( copy_from_user( &antenna_flag, ( PVOID )arg, sizeof( ULONG )))
				break;

			Bus_DebugPrint( "antenna_flag is %lu\n", antenna_flag );
			for (i = 0; i < BUS_DEVICE_TOTAL_COUNT; i++)
			{
				fdoData->DeviceData[i].uAntennaFlag = (UCHAR)(antenna_flag & 0x3);
				
				if ((fdoData->DeviceData[i].SerialNo == BUS_DEVICE_SERIAL_NO_WLAN)
					&& (fdoData->DeviceData[i].pDeviceInterface != NULL))
				{
					fdoData->DeviceData[i].pDeviceInterface->ParaFlag &= (~0x6);
					fdoData->DeviceData[i].pDeviceInterface->ParaFlag |= (fdoData->DeviceData[i].uAntennaFlag << 1);
					Bus_DebugPrint( "SerialNo=%d ParaFlag=%d\n", 
						fdoData->DeviceData[i].SerialNo,
						fdoData->DeviceData[i].pDeviceInterface->ParaFlag);
				}
			}			
			status = STATUS_SUCCESS;
		}		
		break;		
        case IOCTL_BUSENUM_QUERY_DSP_START_FLAG:
		tdspbus_trace();
            {
                *( PULONG )buffer = 0;	//( ULONG )fdoData->DSPStartFlag;
                if ( copy_to_user(( PVOID )arg, buffer, sizeof( ULONG )))
                    return  - EFAULT;
                status = STATUS_SUCCESS;
                //Irp->IoStatus.Information = sizeof( ULONG );
            }
            break;

        case IOCTL_BUSENUM_QUERY_DEVICE_COMNUM:
		tdspbus_trace();
            {
                    PTDSPBUS_DEVICE_DATA pdoData;
                    ULONG SerialNo;

                    if ( copy_from_user( &SerialNo, ( PVOID )arg, sizeof( ULONG )))
                        break;

                    pdoData = Bus_FindPdoBySerialNo( fdoData, SerialNo );
                    if ( pdoData != NULL )
                    {
                        if ( copy_to_user(( PVOID )arg, &pdoData->ComNum, sizeof( ULONG )))
                            return  - EFAULT;
                        status = STATUS_SUCCESS;
                        //Irp->IoStatus.Information = sizeof( ULONG );
                    }
                    else
                    {
                        SerialNo = 0;
                        if ( copy_to_user(( PVOID )arg, &SerialNo, sizeof( ULONG )))
                            return  - EFAULT;
                        status = STATUS_SUCCESS;
                        //Irp->IoStatus.Information = 0;
                    }
            }
            break;

        case IOCTL_BUSENUM_QUERY_DEVICE_COMPANYID:
		tdspbus_trace();
            {

                    PTDSPBUS_DEVICE_DATA pdoData;
                    ULONG SerialNo = *( PULONG )buffer;


                    if ( copy_from_user( &SerialNo, ( PVOID )arg, sizeof( ULONG )))
                        break;

                    pdoData = Bus_FindPdoBySerialNo( fdoData, SerialNo );
                    if ( pdoData != NULL )
                    {
                        if ( copy_to_user(( PVOID )arg, &pdoData->CompanyID, sizeof( ULONG )))
                            return  - EFAULT;
                        status = STATUS_SUCCESS;
                        //Irp->IoStatus.Information = sizeof( USHORT );
                    }
                    else
                    {
                        SerialNo = 0;
                        if ( copy_to_user(( PVOID )arg, &SerialNo, sizeof( ULONG )))
                            return  - EFAULT;
                        status = STATUS_SUCCESS;
                        // Irp->IoStatus.Information = 0;
                    }
            }
            break;

        case IOCTL_BUSENUM_GET_CHIP_ID:
		tdspbus_trace();
            {
                if ( copy_to_user(( PVOID )arg, &fdoData->chipID, sizeof( ULONG )))
                    return  - EFAULT;
                status = STATUS_SUCCESS;
                //Irp->IoStatus.Information = sizeof( ULONG );
            }
            break;

        case IOCTL_BUSENUM_GET_SUB_SYSTEM_ID_INFO:
		tdspbus_trace();
            if ( sizeof( ULONG ) == outlen )
            {
                if ( copy_to_user(( PVOID )arg, &fdoData->SubSystemIDInfo, sizeof( ULONG )))
                    return  - EFAULT;
                status = STATUS_SUCCESS;
                //Irp->IoStatus.Information = sizeof( ULONG );
            }
            break;

        case IOCTL_BUSENUM_READ_ENUM_MODE:
		tdspbus_trace();
            {
                    PTDSPBUS_DEVICE_DATA pdoData;

			fdoData->EnumMode = 0;
			pdoData = Bus_FindPdoBySerialNo(fdoData, BUS_DEVICE_SERIAL_NO_BT);
			if (pdoData != NULL)
			{
				if (pdoData->pDeviceInterface->devstate == Started)
				{
					fdoData->EnumMode = BUS_DEVICE_SERIAL_NO_BT;
				}
				else if (pdoData->state_old_from_wb == NotStarted)
				{
					fdoData->EnumMode = BUS_DEVICE_SERIAL_NO_BT << 8;
				}
			}
			pdoData = Bus_FindPdoBySerialNo(fdoData, BUS_DEVICE_SERIAL_NO_WLAN);
			if (pdoData != NULL)
			{
				if (pdoData->pDeviceInterface->devstate == Started)
				{
					fdoData->EnumMode |= BUS_DEVICE_SERIAL_NO_WLAN;
				}
				else if (pdoData->state_old_from_wb == NotStarted)
				{
					fdoData->EnumMode |= (BUS_DEVICE_SERIAL_NO_WLAN << 8);
				}
			}

			if (fdoData->EnumMode & 0xff)
				fdoData->EnumMode &= 0xff;
			else if (fdoData->EnumMode >> 8)
				fdoData->EnumMode = (ULONG)(-1);
				
			if ( copy_to_user(( PVOID )arg, &fdoData->EnumMode, sizeof( ULONG )))
				return  - EFAULT;
			Bus_DebugPrint( "enum mode is %lu\n", fdoData->EnumMode );
			status = STATUS_SUCCESS;
            }
            break;

        case IOCTL_BUSENUM_WRITE_ENUM_MODE:
		tdspbus_trace();
		{
			ULONG enummode;

			if ( copy_from_user( &enummode, ( PVOID )arg, sizeof( ULONG )))
				break;

			Bus_DebugPrint( "enum mode is %lu\n", enummode );
			fdoData->EnumMode = enummode;

			status = STATUS_SUCCESS;
		}
		break;

        case IOCTL_BUSENUM_QUERY_RESET_FLAG:
		tdspbus_trace();
            {
                *( PULONG )buffer = 0;//( ULONG )fdoData->ResetFlag;
                if ( copy_to_user(( PVOID )arg, buffer, sizeof( ULONG )))
                    return  - EFAULT;
                status = STATUS_SUCCESS;
                //Irp->IoStatus.Information = sizeof( ULONG );
            }
            break;

#if	0//def TDSP_ONE_KEY_CONTROL_RF_SUPPORT

            case IOCTL_BUSENUM_SET_TEST_POWER_ON:
		tdspbus_trace();

                //TDSP_WriteToReg( fdoData, TDSP_WIRELESS_POWRE_ON_OFF, 1 );
                //TDSP_GetPowerLevel( fdoData );
                fdoData->PowerLevel = 1;
                Bus_DebugPrint( "tdspbus_unlocked_ioctl is called!cmd= IOCTL_BUSENUM_SET_TEST_POWER_ON,level=%d,PowerState=%d!\n", fdoData->PowerLevel, TDSP_GetCurrentBusPowerState( fdoData ));
                status = STATUS_SUCCESS;
                //Irp->IoStatus.Information = 0;
                break;

            case IOCTL_BUSENUM_SET_TEST_POWER_OFF:
		tdspbus_trace();

                //TDSP_WriteToReg( fdoData, TDSP_WIRELESS_POWRE_ON_OFF, 0 );
                //TDSP_GetPowerLevel( fdoData );
                fdoData->PowerLevel = 0;
                Bus_DebugPrint( "tdspbus_unlocked_ioctl is called!cmd= IOCTL_BUSENUM_SET_TEST_POWER_OFF,level=%d,PowerState=%d!\n", fdoData->PowerLevel, TDSP_GetCurrentBusPowerState( fdoData ));
                status = STATUS_SUCCESS;
                //Irp->IoStatus.Information = 0;
                break;

            case IOCTL_BUSENUM_SET_DEVICE_STATE:
		tdspbus_trace();
                if ( sizeof( ULONG ) == inlen )
                {
                    ULONG state;

                    if ( copy_from_user( &state, ( PVOID )arg, sizeof( ULONG )))
                        break;

                    TDSP_WriteToReg( fdoData, TDSP_DEVICE_STATE, state );
                    status = STATUS_SUCCESS;
                    //Irp->IoStatus.Information = 0;
                }
                break;

#endif

        case IOCTL_BUSENUM_QUERY_RF_TYPE:
		tdspbus_trace();
            {
                *( PULONG )buffer = ( ULONG )fdoData->RfType;
                if ( copy_to_user(( PVOID )arg, buffer, sizeof( ULONG )))
                    return  - EFAULT;
                status = STATUS_SUCCESS;
                //Irp->IoStatus.Information = sizeof( ULONG );
            }
            break;

        case IOCTL_BUSENUM_QUERY_AUTO_ENUM_FLAG:
		tdspbus_trace();
            {
                ULONG autoenum;

                #ifdef TDSP_ONE_KEY_CONTROL_RF_SUPPORT
                    autoenum = fdoData->AutoEnum;
                #else
                    autoenum = 0;
                #endif

                if ( copy_to_user(( PVOID )arg, &autoenum, sizeof( ULONG )))
                    return  - EFAULT;
                status = STATUS_SUCCESS;
                //Irp->IoStatus.Information = sizeof( ULONG );
            }
            break;

        case IOCTL_BUSENUM_READ_REGISTER:
		tdspbus_trace();
            {
#if 0		
                RW_REGISTER_STRUC rd_register;
                PUCHAR tmpbuf;

                Bus_DebugPrint( "3DSP Read Register called!\n" );
                status = STATUS_INVALID_PARAMETER;

                if ( !arg )
                {
                    Bus_DebugPrint( "3DSP Bus Driver Read Register failed,NULL pointer!\n" );
                    break;
                }

                if ( !copy_from_user( &rd_register, ( PVOID )arg, sizeof( RW_REGISTER_STRUC )))
                {
                    if (( rd_register.length == 0 ) || ( rd_register.length >= 1024 ))
                    {
                        Bus_DebugPrint( "3DSP Bus Driver Read Register failed,length error!\n" );
                        break;
                    }

                    if ( rd_register.offset >= 0x6000 )
                    {
                        Bus_DebugPrint( "3DSP Bus Driver Read Register failed,offset error!\n" );
                        break;
                    }

                    tmpbuf = tdsp_alloc_mem( rd_register.length, GFP_ATOMIC );
                    if ( tmpbuf )
                    {
                        PFDO_DEVICE_DATA FdoData;

                        FdoData = GetGlogalFdoData();
//                        TDSP_HAL_READ_BYTES(( PUCHAR )FdoData->BaseRegisterAddress + rd_register.offset, tmpbuf, rd_register.length );
                        if ( !copy_to_user(( PUCHAR )arg + 8, tmpbuf, rd_register.length ))
                        {
                            status = STATUS_SUCCESS;
                        }
                        tdsp_free_mem( tmpbuf );
                    }
                    else
                    {
                        Bus_DebugPrint( "3DSP Bus Driver Read Register failed,no memory!\n" );
                        break;
                    }

                }
#endif				
                break;
            }


        case IOCTL_BUSENUM_WRITE_REGISTER:
		tdspbus_trace();
            {
#if 0				
                RW_REGISTER_STRUC rd_register;
                PUCHAR tmpbuf;

                Bus_DebugPrint( "3DSP Write Register called!\n" );
                status = STATUS_INVALID_PARAMETER;

                if ( !arg )
                {
                    Bus_DebugPrint( "3DSP Bus Driver Write Register failed,NULL pointer!\n" );
                    break;
                }

                if ( !copy_from_user( &rd_register, ( PVOID )arg, sizeof( RW_REGISTER_STRUC )))
                {
                    if (( rd_register.length == 0 ) || ( rd_register.length >= 1024 ))
                    {
                        Bus_DebugPrint( "3DSP Bus Driver Write Register failed,length error!\n" );
                        break;
                    }

                    if ( rd_register.offset >= 0x6000 )
                    {
                        Bus_DebugPrint( "3DSP Bus Driver Write Register failed,offset error!\n" );
                        break;
                    }

                    tmpbuf = tdsp_alloc_mem( rd_register.length, GFP_ATOMIC );
                    if ( tmpbuf )
                    {
                        PFDO_DEVICE_DATA FdoData;

                        FdoData = GetGlogalFdoData();
                        if ( !copy_from_user( tmpbuf, ( PUCHAR )arg + 8, rd_register.length ))
                        {
//                            TDSP_HAL_WRITE_BYTES(( PUCHAR )FdoData->BaseRegisterAddress + rd_register.offset, tmpbuf, rd_register.length );
                            status = STATUS_SUCCESS;
                        }
                        tdsp_free_mem( tmpbuf );
                    }
                    else
                    {
                        Bus_DebugPrint( "3DSP Bus Driver Write Register failed,no memory!\n" );
                        break;
                    }

                }
#endif				
                break;
            }

#ifdef TDSP_LOAD_EEPROM_SUPPORT
            case IOCTL_BUSENUM_WRITE_EEPROM:
		tdspbus_trace();
                {
                    LOAD_EEPROM_STRUC tmp_load,  *pload;
                    char szRom[136]; //128+sizeof(ULONG)+sizeof(ULONG)

                    Bus_DebugPrint( "3DSP Bus Driver Write eeprom called!\n" );
                    pload = ( PLOAD_EEPROM_STRUC ) &tmp_load;
                    status = STATUS_INVALID_PARAMETER;

                    if ( !arg )
                    {
                        Bus_DebugPrint( "3DSP Bus Driver Write eeprom call failed,NULL pointer!\n" );
                        break;
                    }

                    if ( !copy_from_user( pload, ( PVOID )arg, sizeof( LOAD_EEPROM_STRUC )))
                    {
                        if (( pload->length <= 128 ) && ( pload->offset < 1024 ) && (( pload->offset + pload->length ) <= 1024 ))
                        {
                            PFDO_DEVICE_DATA fdoData;

                            if ( !copy_from_user( szRom, ( PVOID )arg, pload->length + 8 ))
                            {
                                fdoData = GetGlogalFdoData();
                                if ( fdoData )
                                {
                                    UCHAR i = 0;

                                    fdoData->EepromOffset = pload->offset;
                                    fdoData->EepromLen = pload->length;
                                    RtlCopyMemory( fdoData->EepromBuf, szRom + 8, pload->length );
                                    Bus_DebugPrint( "TDSP_LOAD_EEPROM_SUPPORT: offset = %lu, length = %lu\n", fdoData->EepromOffset, fdoData->EepromLen );
                                    Bus_DebugPrint( "TDSP_LOAD_EEPROM_SUPPORT: eeprom content to modify...\n" );
                                    TDSP_PrintBuffer( fdoData->EepromBuf, fdoData->EepromLen );

                                    while ( i <= 20 )
                                    {
                                        if ( !fdoData->ResetFlag )
                                        {
                                            break;
                                        }
                                        TDSP_Delay( 100 );
                                        i++;
                                    }
                                    if ( !fdoData->ResetFlag )
                                    {
                                        Bus_DebugPrint( "Now submit a real reset... for loading eeprom\n" );
                                        fdoData->LoadEepromFlag = 1;
                                        fdoData->ResetFlag = 1;
                                        #ifdef TDSP_ONE_KEY_CONTROL_RF_SUPPORT
                                            IoStopTimer( fdoData );
                                        #endif

                                        {
                                            PPDO_DEVICE_DATA pdoData;
                                            KIRQL oldIrql;
                                            PLIST_ENTRY entry, listHead, nextEntry;

                                            KeAcquireSpinLock( &fdoData->PdoListLock, &oldIrql );

                                            listHead = &fdoData->ListOfPDOs;

                                            for ( entry = listHead->Flink, nextEntry = entry->Flink; entry != listHead; entry = nextEntry, nextEntry = entry->Flink )
                                            {
                                                pdoData = CONTAINING_RECORD( entry, PDO_DEVICE_DATA, Link );

                                                if ( pdoData->DevicePnPState == Started )
                                                {
                                                    pdoData->PengingFlag |= CURRENTRESET_BIT;
                                                }

                                                if ( pdoData->DevicePnPState == Starting )
                                                {
                                                    pdoData->PengingFlag |= RESETINIT_BIT;
                                                }
                                            }

                                            KeReleaseSpinLock( &fdoData->PdoListLock, oldIrql );
                                        }
                                        // Disable interrupt
                                        Bus_DisableInterrupt( fdoData );
                                        Bus_InitPdoResetFlag( fdoData );
                                        Bus_NotifyAllReset( fdoData, RESET_EVENT_NOTIFY_PREPARATION );
                                    }
                                    status = STATUS_SUCCESS;
                                }
                            }
                            else
                            {
                                Bus_DebugPrint( "copy_from_user failed when writing eeprom!!!\n" );
                            }
                        }

                    }
                    break;
                }

            case IOCTL_BUSENUM_LOAD_EEPROM:
		tdspbus_trace();
                {
                    UCHAR i = 0;

                    while ( i <= 20 )
                    {
                        if ( !fdoData->ResetFlag )
                        {
                            break;
                        }
                        TDSP_Delay( 100 );
                        i++;
                    }

                    if ( !fdoData->ResetFlag )
                    {
                        Bus_DebugPrint( "Now submit a real reset... for writing eeprom!\n" );
                        fdoData->LoadEepromFlag = 1;
                        fdoData->ResetFlag = 1;
                        #ifdef TDSP_ONE_KEY_CONTROL_RF_SUPPORT
                            IoStopTimer( fdoData );
                        #endif

                        {
                            PPDO_DEVICE_DATA pdoData;
                            KIRQL oldIrql;
                            PLIST_ENTRY entry, listHead, nextEntry;

                            KeAcquireSpinLock( &fdoData->PdoListLock, &oldIrql );

                            listHead = &fdoData->ListOfPDOs;

                            for ( entry = listHead->Flink, nextEntry = entry->Flink; entry != listHead; entry = nextEntry, nextEntry = entry->Flink )
                            {
                                pdoData = CONTAINING_RECORD( entry, PDO_DEVICE_DATA, Link );

                                if ( pdoData->DevicePnPState == Started )
                                {
                                    pdoData->PengingFlag |= CURRENTRESET_BIT;
                                }

                                if ( pdoData->DevicePnPState == Starting )
                                {
                                    pdoData->PengingFlag |= RESETINIT_BIT;
                                }
                            }

                            KeReleaseSpinLock( &fdoData->PdoListLock, oldIrql );
                        }

                        // Disable interrupt
                        Bus_DisableInterrupt( fdoData );
                        Bus_InitPdoResetFlag( fdoData );
                        Bus_NotifyAllReset( fdoData, RESET_EVENT_NOTIFY_PREPARATION );
                    }
                    status = STATUS_SUCCESS;
                    break;
                }
#endif

        default:
        	Bus_DebugPrint( "unknow cmd = %x\n", cmd - IOCTL_BUSENUM_PLUGIN_HARDWARE);
            break; // default status is STATUS_INVALID_PARAMETER
    }

    //Irp->IoStatus.Status = status;
    //IoCompleteRequest( Irp, IO_NO_INCREMENT );
//    Bus_DecIoCount( fdoData );
    return status;
}
