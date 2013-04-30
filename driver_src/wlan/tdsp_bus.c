#include "tdsp_bus.h"
#ifdef WB_PROXY_FLAG
#include <linux/version.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "tdsp_memory.h"
#include "usbwlan_lib.h"
#include "lwlan_netdev.h"
#include "usbwlan_version.h"

//static char* TDSP_FILE_INDICATOR="TDBUS";

#define BUS_DEVICE_SERIAL_NO_WLAN		0x2



#define BUS_APP_NAME	"WLAN"
#define BUS_PRINTK(fmt, arg...)		{	\
	printk("%s: ", BUS_APP_NAME);			\
	printk("%s: " fmt "" , __FUNCTION__ , ## arg);	\
}

extern int register_tdspbus_usbdev(PTDSPBUS_INTERFACE pInterface);
extern int unregister_tdspbus_usbdev(PTDSPBUS_INTERFACE pInterface);

int  Bus_closeInterface(void *pIntf)
{
	PTDSPBUS_INTERFACE pBusInterfaceData = (PTDSPBUS_INTERFACE)pIntf;
		
	BUS_PRINTK(" enter;\n");

	if (pBusInterfaceData == NULL)
		return -EIO;
	
	unregister_tdspbus_usbdev(pBusInterfaceData);	
	tdsp_memory_free(pBusInterfaceData);
	return 0;
}

int Bus_Open(PTDSPBUS_INTERFACE pIntf)
{	
	BUS_PRINTK(" enter;\n");
	
	//init dsp
	if( 0 != Adap_Initialize(pIntf->pBtDev))
	{
		BUS_PRINTK(" Adap_Initialize  failed!!\n");
		return -EIO;
	}
	
	return wlan_netdev_setup(pIntf->pBtDev);
}

int Bus_Close(PTDSPBUS_INTERFACE pIntf)
{
	BUS_PRINTK(" enter;\n");

	Adap_Halt(pIntf->pBtDev);
	
	wlan_netdev_destroy(pIntf->pBtDev);
	
	//release all resources
	Adap_UnInitialize(pIntf->pBtDev);
	return 0;
}

void *Bus_new_interface(void *dev)
{
	PTDSPBUS_INTERFACE pBusInterfaceData;

	pBusInterfaceData = tdsp_memory_alloc(sizeof(TDSPBUS_INTERFACE));
	if (pBusInterfaceData == NULL)
	{
		BUS_PRINTK(" enter, but tdsp_memory_alloc fail;\n");
		return NULL;
	}

	tdsp_memset(pBusInterfaceData, 0, sizeof(TDSPBUS_INTERFACE));
	
	pBusInterfaceData->pBtDev = dev;
	
	BUS_PRINTK(" dev = %p, intf = %p;\n" , dev, pBusInterfaceData);	

	return pBusInterfaceData;
}

int Bus_openInterface(void *pIntf)
{
	PTDSPBUS_INTERFACE pBusInterfaceData = (PTDSPBUS_INTERFACE)pIntf;
	int ret;

	BUS_PRINTK(" enter;\n");

	pBusInterfaceData->SerialNo = BUS_DEVICE_SERIAL_NO_WLAN;
	strcat(pBusInterfaceData->version, WLAN_DRV_VERSION);
	pBusInterfaceData->open = Bus_Open;
	pBusInterfaceData->close = Bus_Close;
	//	pBusInterfaceData.suspend = Bus_Suspend;
	//	pBusInterfaceData.resume = Bus_Resume;
	//	pBusInterfaceData.notify_interrupt = Bus_Notify_Interrupt;
	//	pBusInterfaceData.notify_reset = Bus_Notify_Reset;
	//	pBusInterfaceData.notify_initreset = Bus_Notify_Initreset;
	//	pBusInterfaceData.notify_completeinit = Bus_Notify_Completeinit;
	//	pBusInterfaceData.notify_deatch = Bus_Notify_Detach;

	ret = register_tdspbus_usbdev(pBusInterfaceData);
	if (ret != 0)
		ret = register_tdspbus_usbdev(pBusInterfaceData);
	
	if (0 != ret)
	{
		kfree(pBusInterfaceData);
	}
	
	return ret;
}

void Bus_set_firmware_version(void *pIntf, unsigned long c8051_version, unsigned long dsp_version)
{

	PTDSPBUS_INTERFACE pBusInterfaceData = (PTDSPBUS_INTERFACE)pIntf;

	pBusInterfaceData->dsp_version = dsp_version;

	pBusInterfaceData->c8051_version = c8051_version;
}

unsigned char Bus_get_hotkey_flag(void *pIntf)
{

	PTDSPBUS_INTERFACE pBusInterfaceData = (PTDSPBUS_INTERFACE)pIntf;
	return (pBusInterfaceData->ParaFlag & 0x1);
}

unsigned char Bus_get_antenna_flag(void *pIntf)
{

	PTDSPBUS_INTERFACE pBusInterfaceData = (PTDSPBUS_INTERFACE)pIntf;

	return ((pBusInterfaceData->ParaFlag >> 1) & 0x3);
}

#endif
