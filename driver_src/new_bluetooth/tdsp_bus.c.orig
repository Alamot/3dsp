#include <linux/usb.h>
#include <linux/version.h>
#include <linux/slab.h>

//#include "tdsp_bus.h"

#define MAX_VERSIONID_LEN    64
#define BT_STRING_VERSION_ID "BTU100723"

#define BUS_DEVICE_SERIAL_NO_NULL		0x0
#define BUS_DEVICE_SERIAL_NO_BT		0x1
#define BUS_DEVICE_SERIAL_NO_WLAN	0x2
#define BUS_DEVICE_SERIAL_NO_GPS		0x4

extern unsigned int combodrv;

typedef struct _TDSPBUS_INTERFACE_DATA 
{
	// A back pointer to the bus
	void *ParentFdo;

	void *pPciDev; //pci_dev
	void *pBtDev; //PBT_DEVICE_EXT
	
	// An array of (zero terminated wide character strings).
	// The array itself also null terminated
	unsigned char version[MAX_VERSIONID_LEN]; // send to bus driver
	unsigned long c8051_version;
	unsigned long dsp_version;
	
	// Unique serail number of the device on the bus
	unsigned char SerialNo; // send to bus driver
	
	unsigned char ResetFlag;	// Reset Flag
	unsigned char ParaFlag;	/*from wb config.
			bit0~bit7
			bit0:		hotkey
			bit12:	antenna
			bit3:
			bit4:
			*/
	
	// Device State
	unsigned long devstate; //set by bus driver.

	// Callback operation
	int (*open)(struct _TDSPBUS_INTERFACE_DATA *dev);
	int (*close)(struct _TDSPBUS_INTERFACE_DATA *dev);
	int (*suspend)(struct _TDSPBUS_INTERFACE_DATA *dev, unsigned long state);
	int (*resume)(struct _TDSPBUS_INTERFACE_DATA *dev);
	int (*notify_reset)(struct _TDSPBUS_INTERFACE_DATA *dev, int resetevent);
	int (*notify_initreset)(struct _TDSPBUS_INTERFACE_DATA *dev, int resetevent);
	int (*notify_deatch)(struct _TDSPBUS_INTERFACE_DATA *dev, int reason);
} TDSPBUS_INTERFACE, *PTDSPBUS_INTERFACE;

void *register_HCI_device(void *dev);
void unregister_HCI_device(void *dev);

#define BUS_APP_NAME	"BLUETOOTH"
#define BUS_PRINTK(fmt, arg...)		{	\
	printk("%s: ", BUS_APP_NAME);			\
	printk("%s: " fmt "" , __FUNCTION__ , ## arg);	\
}

extern int register_tdspbus_usbdev(PTDSPBUS_INTERFACE pInterface);
extern int unregister_tdspbus_usbdev(PTDSPBUS_INTERFACE pInterface);

int  Bus_closeInterface(void *pIntf)
{

	PTDSPBUS_INTERFACE pBusInterface = (PTDSPBUS_INTERFACE)pIntf;
		
	printk("%s enter;\n" , __FUNCTION__);

	if (pBusInterface == NULL)
		return -EIO;
	
	unregister_tdspbus_usbdev(pBusInterface);	
	kfree(pBusInterface);
	return 0;
}

int Bus_Open(PTDSPBUS_INTERFACE pIntf)
{	
	BUS_PRINTK(" enter;\n");
	if (((void *)0x0) != register_HCI_device(pIntf->pBtDev))
		return 0;
	return 1;
}

int Bus_Close(PTDSPBUS_INTERFACE pIntf)
{
	BUS_PRINTK( " enter;\n");
	unregister_HCI_device(pIntf->pBtDev);
	return 0;
}

void *Bus_new_interface(void *dev)
{
	PTDSPBUS_INTERFACE pBusInterfaceData;

	pBusInterfaceData = kzalloc(sizeof(TDSPBUS_INTERFACE),GFP_KERNEL);

	if (pBusInterfaceData == NULL)
	{
		BUS_PRINTK( " kzalloc fail!\n");
		return NULL;
	}
	memset(pBusInterfaceData, 0, sizeof(TDSPBUS_INTERFACE));	
	pBusInterfaceData->pBtDev = dev;
	BUS_PRINTK(" enter, dev = %p, intf = %p;\n" , dev, pBusInterfaceData);	
	return pBusInterfaceData;
}

int Bus_openInterface(void *pIntf)
{
	PTDSPBUS_INTERFACE pBusInterfaceData = (PTDSPBUS_INTERFACE)pIntf;
	int ret;

	pBusInterfaceData->SerialNo = BUS_DEVICE_SERIAL_NO_BT;
	strcat(pBusInterfaceData->version, BT_STRING_VERSION_ID);
	pBusInterfaceData->open = Bus_Open;
	pBusInterfaceData->close = Bus_Close;
	//	pBusInterfaceData->suspend = Bus_Suspend;
	//	pBusInterfaceData->resume = Bus_Resume;
	//	pBusInterfaceData->notify_interrupt = Bus_Notify_Interrupt;
	//	pBusInterfaceData->notify_reset = Bus_Notify_Reset;
	//	pBusInterfaceData->notify_initreset = Bus_Notify_Initreset;
	//	pBusInterfaceData->notify_completeinit = Bus_Notify_Completeinit;
	//	pBusInterfaceData->notify_deatch = Bus_Notify_Detach;

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

unsigned int tdsp_get_combodrv(void)
{
	return (combodrv);
}
