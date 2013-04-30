
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include "../../wlan/usbwlan_lib.h"

#ifdef DEBUG_OPEN
static char compiled_time[] ={"Compiled at "__TIME__"  [ "__DATE__" ]"};
#endif

static int __init _wlan_lib_init(void)
{
	int err = 0;

#ifdef DEBUG_OPEN
	printk( "\n");
	printk( "\n");
	printk(  "/*****************************************************************************\\\n" );
	printk(  "|                                                                             |\n" );
	printk( "|                          "USB WLAN LIB"                               |\n");
	printk(  "|                                                                             |\n" );
	printk(  "|                          %s              |\n" , compiled_time);
	printk(  "|                                                                             |\n" );	
	printk(  "|                                                                             |\n" );	
	printk(  "\\*****************************************************************************/\n" );
	printk( "\n");
	printk( "\n");
#endif
    //set syscall function poiter first
	return err;

	
}

static void __exit _wlan_lib_exit(void)
{

#ifdef DEBUG_OPEN
	printk( "\n");
	printk( "\n");
	printk(  "/******************************************"
				 "***********************************\\\n" );	
	printk( "\n");
	printk(  "    USB WLAN LIB\n" );
	printk(  "    Exiting . . . . . .                              (done)\n" );
	printk( "\n");
	printk(  "\\******************************************"
				 "***********************************/\n" );	
	printk( "\n");
	printk( "\n");
	
	return;
#endif

}






MODULE_LICENSE("Proprietary");
MODULE_DESCRIPTION("Tdsp usb wlan lib");

EXPORT_SYMBOL(Adap_Create);
EXPORT_SYMBOL(Adap_Initialize);
EXPORT_SYMBOL(Adap_Halt);
EXPORT_SYMBOL(Adap_UnInitialize);
EXPORT_SYMBOL(Adap_Destroy);
EXPORT_SYMBOL(Adap_Tx_Available);
EXPORT_SYMBOL(Adap_Send);
EXPORT_SYMBOL(Adap_Set_BusInf);
EXPORT_SYMBOL(Adap_Get_BusInf);
EXPORT_SYMBOL(syscall_set);
EXPORT_SYMBOL(tdsp_set_debug);
EXPORT_SYMBOL(Usb_Completion_Imp);
EXPORT_SYMBOL(Adap_Get_Mac_Addr);
EXPORT_SYMBOL(Adap_Set_Netdev);
EXPORT_SYMBOL(Adap_Get_Netdev);
EXPORT_SYMBOL(Adap_Get_Netstat);
EXPORT_SYMBOL(Adap_Get_StatData);
EXPORT_SYMBOL(Adap_Set_Netstat);
EXPORT_SYMBOL(Adap_Set_Removed);
EXPORT_SYMBOL(Adap_Get_DotType);
EXPORT_SYMBOL(Adap_Get_Mode);
EXPORT_SYMBOL(Adap_Set_Defult_Channel);
EXPORT_SYMBOL(Adap_Get_Channel);
EXPORT_SYMBOL(Adap_Set_Mode);
EXPORT_SYMBOL(Adap_Get_Channel_Num);
EXPORT_SYMBOL(Adap_Start_Scan);
EXPORT_SYMBOL(Adap_Get_Scan_Result);
EXPORT_SYMBOL(Adap_Get_BSSID);
EXPORT_SYMBOL(Adap_Disassociate);
EXPORT_SYMBOL(Adap_Set_SSID);
EXPORT_SYMBOL(Adap_Get_SSID);
EXPORT_SYMBOL(Adap_Set_Rts);
EXPORT_SYMBOL(Adap_Get_Rts);
EXPORT_SYMBOL(Adap_Set_Frag);
EXPORT_SYMBOL(Adap_Get_Frag);
EXPORT_SYMBOL(Adap_Set_Rate);
EXPORT_SYMBOL(Adap_Get_Rate);
EXPORT_SYMBOL(Adap_Get_Rssi);
EXPORT_SYMBOL(Adap_Set_Auth);
EXPORT_SYMBOL(Adap_Get_Auth_Alg);
EXPORT_SYMBOL(Adap_Get_Key_Mng);
EXPORT_SYMBOL(Adap_Disalbe_Encode);
EXPORT_SYMBOL(Adap_Set_Encode);
EXPORT_SYMBOL(Adap_Get_Encode);

EXPORT_SYMBOL(Adap_Remove_Key_Ext);
EXPORT_SYMBOL(Adap_Add_Key_Ext);
EXPORT_SYMBOL(Adap_Is_Encode_Enabled);
EXPORT_SYMBOL(Adap_Get_Encode_Mode);
EXPORT_SYMBOL(Adap_Get_Encode_Key);
EXPORT_SYMBOL(Adap_Set_Domain);
EXPORT_SYMBOL(Adap_Get_Domain);
EXPORT_SYMBOL(Adap_Get_Dsp_Version);
EXPORT_SYMBOL(Adap_Has_Joined);

EXPORT_SYMBOL(Adap_Driver_isWork);

EXPORT_SYMBOL(Adap_Has_Connected);

EXPORT_SYMBOL(Adap_Is_Privacy_On);

EXPORT_SYMBOL(Adap_Suspend);
EXPORT_SYMBOL(Adap_Resume);
EXPORT_SYMBOL(Adap_Rest_Resume);
EXPORT_SYMBOL(Adap_Use_WPA);
EXPORT_SYMBOL(Adap_Print_USBReg);
EXPORT_SYMBOL(Tx_Print_Status);
module_init(_wlan_lib_init);
module_exit(_wlan_lib_exit);
