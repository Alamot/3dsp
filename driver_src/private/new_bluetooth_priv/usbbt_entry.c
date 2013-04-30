#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include "usbbt_lib.h"

static int __init _bluetooth_lib_init(void)
{
	int err = 0;
	return err;	
}

static void __exit _bluetooth_lib_exit(void)
{
	;
}

MODULE_LICENSE("Proprietary");
MODULE_DESCRIPTION("Tdsp usb bluetooth lib");

EXPORT_SYMBOL(Bluetooth_Adap_Create);
EXPORT_SYMBOL(Bluetooth_Adap_Destory);
EXPORT_SYMBOL(Bluetooth_Adap_Set_BusInf);
EXPORT_SYMBOL(Bluetooth_Adap_Get_BusInf);
EXPORT_SYMBOL(Bluetooth_Adap_Set_hdev);
EXPORT_SYMBOL(Bluetooth_Adap_Get_hdev);
EXPORT_SYMBOL(Bluetooth_Adap_Get_uintf);
EXPORT_SYMBOL(Bluetooth_Adap_Resume);
EXPORT_SYMBOL(Bluetooth_Adap_Suspend);
EXPORT_SYMBOL(Bluetooth_Adap_IsOpen);
EXPORT_SYMBOL(Bluetooth_Adap_SetOpen);
EXPORT_SYMBOL(Bluetooth_Adap_ClearOpen);
EXPORT_SYMBOL(Bluetooth_Adap_Open_Dev);
EXPORT_SYMBOL(Bluetooth_Adap_Close_Dev);
EXPORT_SYMBOL(Bluetooth_Adap_Init);
EXPORT_SYMBOL(Bluetooth_Adap_Set_FirmVer);
EXPORT_SYMBOL(Bluetooth_Adap_Exit_Lmp_Dump);
EXPORT_SYMBOL(Bluetooth_Adap_IsInD3);
EXPORT_SYMBOL(Bluetooth_Adap_SendCmd);
EXPORT_SYMBOL(Bluetooth_Adap_SendData);
EXPORT_SYMBOL(Bluetooth_Adap_DiagSys);
EXPORT_SYMBOL(Bluetooth_Adap_WriteUsbReg);
EXPORT_SYMBOL(Bluetooth_Adap_ReadUsbReg);
EXPORT_SYMBOL(Bluetooth_Adap_ReadDspReg);
EXPORT_SYMBOL(Bluetooth_Adap_WriteDspReg);
EXPORT_SYMBOL(Bluetooth_Adap_Read_Compelet);
EXPORT_SYMBOL(Bluetooth_Adap_Inq_Compelet);
EXPORT_SYMBOL(Bluetooth_Adap_Write_Compelet);
EXPORT_SYMBOL(Bluetooth_Adap_Int_Compelet);
EXPORT_SYMBOL(Bluetooth_Adap_ctrl_Compelet);
EXPORT_SYMBOL(b_syscall_set);
EXPORT_SYMBOL(Bluetooth_Adap_Get_Dumpdata);
EXPORT_SYMBOL(Bluetooth_Adap_GetWorkMod);
EXPORT_SYMBOL(Bluetooth_Adap_GetDev);
//^^BLUETOOTH_DEBUG^^
EXPORT_SYMBOL(dbg_zone);
EXPORT_SYMBOL(dbg_level);
//vvBLUETOOTH_DEBUGvv

module_init(_bluetooth_lib_init);
module_exit(_bluetooth_lib_exit);
