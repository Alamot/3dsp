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
 * [lwlan_entry.c] description:
 * 	This source file defines the entry point of wlan driver 4 linux OS
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/netdevice.h>
#include <linux/kernel.h>
#include "tdsp_memory.h"
#include "tdsp_tasklet.h"
#include "tdsp_mutex.h"
#include "tdsp_event.h"
#include "tdsp_usb.h"
#include "tdsp_timer.h"
#include "tdsp_time.h"
#include "tdsp_bus.h"
#include "tdsp_file.h"
#include "usbwlan_version.h"
#include "usbwlan_lib.h"
#include "lwlan_netdev.h"
#include "lwlan_ioctl.h"
//#include "tdsp_bus.h"

#define USBVENDOR_DSP						0x05e1
#define USBDEVICE_DSP						0x0100

#define USB_EP_CTRL			(0)
#define USB_EP_BULKIN		  (1)   //rx pipe
#define USB_EP_BULKOUT_BODY	(2)   //send pipe
#define USB_EP_BULKIN_2		(3)   //rx pipe
#define USB_EP_BULKOUT_HEAD	(4)  //send pipe
#define USB_EP_BULKINT		 (9)   //int pipe

#ifndef PM_EVENT_FREEZE
#define PM_EVENT_FREEZE 	0x0001
#endif

#ifndef PM_EVENT_SUSPEND
#define PM_EVENT_SUSPEND	0x0002
#endif

#ifndef PM_EVENT_HIBERNATE
#define PM_EVENT_HIBERNATE	0x0004
#endif

// #define DEBUG // uncomment to enable verbose logging of all driver functions

int tdsp_print(const char * fmt,...) 
{
#ifdef DEBUG
    va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);
	return r;
#else
	return 0;
#endif
    //printk(fmt,...);
}

//static char* TDSP_FILE_INDICATOR="ENTRY";

static struct usb_device_id _3dsp_id_tbl[] = {
	/* 3dsp usb wlan */
	{ USB_DEVICE(USBVENDOR_DSP, USBDEVICE_DSP) },
	{ }	/* Terminating entry */
};

static SYS_CALL syscall;

static void _wlan_syscall_set(void)
{
    syscall.memory_free     = tdsp_memory_free;
    syscall.memory_set      = tdsp_memset;
    syscall.memory_alloc    = tdsp_memory_alloc;
    syscall.memory_copy     = tdsp_memcpy;
    syscall.memory_cmp      = tdsp_memcmp;
    syscall.memory_vmalloc		= tdsp_memory_vmalloc;
	syscall.memory_vfree	= tdsp_memory_vfree;
    syscall.event_init      = tdsp_event_init;
    syscall.event_set       = tdsp_event_set;
    syscall.event_reset     = tdsp_event_reset;
    syscall.event_wait      = tdsp_event_wait;
    syscall.event_kill      = tdsp_event_kill;
  
    syscall.spin_lock_init    = tdsp_spin_lock_init;
    syscall.spin_lock         = tdsp_spin_lock;
    syscall.spin_lock_bh      = tdsp_spin_lock_bh;
    syscall.spin_lock_irqsave = tdsp_spin_lock_irqsave;
    syscall.spin_unlock       = tdsp_spin_unlock;
    syscall.spin_unlock_bh       = tdsp_spin_unlock_bh;
    syscall.spin_unlock_irqrestore = tdsp_spin_unlock_irqrestroe;
    syscall.spin_lock_kill         = tdsp_spin_lock_kill;

    syscall.timer_init           = tdsp_timer_init;
    syscall.timer_start          = tdsp_timer_start;
    syscall.timer_stop           = tdsp_timer_stop;
    syscall.timer_reset          = tdsp_timer_reset;
    syscall.timer_kill           = tdsp_timer_kill;
    syscall.sleep                = tdsp_sleep;
    syscall.ms_to_ticks          = tdsp_ms_to_ticks;
	syscall.get_random_byte      = tdsp_get_random_byte; 
	syscall.get_random_dword     = tdsp_get_random_dword;
    syscall.time_downout_duration = time_downout_duration;

    //set taskelet related func pointer 
    syscall.tasklet_init     = tdsp_tasklet_init;
    syscall.tasklet_schedule = tdsp_tasklet_schedule;
    syscall.tasklet_kill     = tdsp_tasklet_kill;
    syscall.tasklet_enable   = tdsp_tasklet_enable;
    syscall.tasklet_disable  = tdsp_tasklet_disable;
    syscall.worklet_init     = tdsp_worklet_init;
    syscall.worklet_running  = tdsp_worklet_running;
    syscall.worklet_schedule = tdsp_worklet_schedule;
    syscall.worklet_kill     = tdsp_worklet_kill;
    
    syscall.urb_alloc       = tdsp_usb_alloc_urb;
    syscall.urb_free        = tdsp_usb_free_urb;
    syscall.urb_unlink      = tdsp_usb_unlink_urb;
    syscall.urb_kill        = tdsp_usb_kill_urb;
    syscall.urb_getlen      = tdsp_usb_get_len;
    syscall.urb_getstatus   = tdsp_usb_get_status;
    syscall.urb_getcntxt    = tdsp_usb_get_context;
    syscall.usb_issrmv      = tdsp_usb_is_srmv;
    syscall.usb_alloc_ctrlreq = tdsp_usb_alloc_ctrlreq;
    syscall.usb_set_ctrlreq   = tdsp_usb_set_ctrlreq;
    syscall.usb_free_ctrlreq  = tdsp_usb_free_ctrlreq;
    syscall.usb_ctrlmsg     = tdsp_usb_ctrl_msg;
    syscall.usb_ctrlreq     = tdsp_usb_ctrl_req;
    syscall.usb_rcvint      = tdsp_usb_rcvint;
    syscall.usb_bulkin      = tdsp_usb_bulkin;
    syscall.usb_bulkout     = tdsp_usb_bulkout;

    syscall.skb_alloc         = tdsp_netdev_skb_alloc;
    syscall.skb_sbmt          = tdsp_netdev_skb_sbmt;
    syscall.skb_free          = tdsp_netdev_skb_free;
    syscall.netq_start        = tdsp_netdev_netq_start;
    syscall.netq_ifstop       = tdsp_netdev_netq_ifstop;
    syscall.netq_stop         = tdsp_netdev_netq_stop;

    syscall.iw_send_bssid_to_up	= iw_send_bssid_to_up;
    syscall.wlan_netdev_setup = wlan_netdev_setup;
    syscall.wlan_netdev_destroy = wlan_netdev_destroy;
	
    syscall.bus_set_fw_ver    = Bus_set_firmware_version;
    syscall.bus_get_hkey_flag = Bus_get_hotkey_flag;
    syscall.bus_get_antenna_flag = Bus_get_antenna_flag;
    syscall.print                = tdsp_print;
    
	syscall.openfile		= tdsp_openfile;
	syscall.closefile		= tdsp_closefile;
	syscall.open_success		= tdsp_open_success;
	syscall.readfile		= tdsp_readfile;
}

#ifdef DEBUG_OPEN__WLAN
void prt_time()
{
	struct timeval tv1;
	int mt,st;
	
	memset(&tv1, sizeof(tv1), 0);
	do_gettimeofday(&tv1);

	st = tv1.tv_sec % 60;
	mt = (tv1.tv_sec % 3600) / 60;
	
	printk("-%10i.%06i(%d:%d) \n   ", (int)tv1.tv_sec, (int)tv1.tv_usec,  mt, st);
}

#else
#define prt_time()
#endif
static int _wlan_usb_probe(struct usb_interface *intf, const struct usb_device_id *id)
{

	struct usb_device *udev = interface_to_usbdev(intf);
	struct usb_host_endpoint  *ep;
	struct usb_host_interface *uif;
	PTDSPBUS_INTERFACE pBusInterfaceData;
	//PBT_DEVICE_EXT	devExt = NULL;
	//struct hci_dev *hdev = NULL;
	int i;
	//TDSP_STATUS        status;
	void *    pAdap; 
	volatile USB_INFO usb_info;
	
	printk("Enter [%s]\n",__FUNCTION__);    
	prt_time();
        
	if (!id->driver_info) 
	{
		const struct usb_device_id *match;
		match = usb_match_id(intf, _3dsp_id_tbl);
		if (match)
			id = match;
	}

	if (intf->cur_altsetting->desc.bInterfaceNumber != 0)
	{
		printk( "intf %d is not 3dsp wlan interface\n", 
		                 intf->cur_altsetting->desc.bInterfaceNumber);
		return -ENODEV;
	}

	/* Find endpoints that we need */
	uif = intf->cur_altsetting;
	printk( "3dsp usb wlan device found:) intf %d, alt configs %d, endpoints %d\n", 
		             uif->desc.bInterfaceNumber, 
		             intf->num_altsetting,
		             uif->desc.bNumEndpoints);

	if (uif->desc.bNumEndpoints < 5) 
	{
		printk( "%s: endpoints not enough!!\n",__FUNCTION__);
		return -EIO;
	}

	/* Only for debug, print out the endpoint informations */
	for (i = 0; i < uif->desc.bNumEndpoints; i++) 
	{
		ep = &uif->endpoint[i];
		printk( "ep addr %d, type %d, in/out %s\n", 
				ep->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK,
				ep->desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK,
				ep->desc.bEndpointAddress & USB_DIR_IN ? " IN" : "OUT");
	}

   
	//syscall.print           =  printk;
	printk("tdsp_spin_lock_init is %x,syscall's is %x\n",
            (unsigned int)tdsp_spin_lock_init,
            (unsigned int)syscall.spin_lock_init);  
	printk("sc is %x\n",(unsigned int)&syscall);
	syscall_set((PSYS_CALL)&syscall);
    
    //fill usb device info
	usb_info.usb_dev = udev;
	usb_info.usb_intf = intf;
	usb_info.EP_control      = USB_EP_CTRL;
	usb_info.EP_bulkout_HEAD = USB_EP_BULKOUT_HEAD;
	usb_info.EP_bulkout_BODY = USB_EP_BULKOUT_BODY;      //pipe pointer
	usb_info.EP_bulkin       = USB_EP_BULKIN;
	usb_info.EP_interrupt    = USB_EP_BULKINT;

	printk("usb_info is %x\n",(unsigned int)&usb_info);
	if(NULL == (pAdap = Adap_Create((PUSB_INFO)&usb_info)))
	{
		printk("[%s] Usb device Init failed \n",__FUNCTION__);
		return -EIO;
	}

    
//2 check what this routine doing!!!
	usb_set_intfdata(intf, pAdap);
	Adap_Set_Removed(pAdap,0);

#ifdef WB_PROXY_FLAG	

	pBusInterfaceData = (PTDSPBUS_INTERFACE)Bus_new_interface((void *) pAdap);
	pBusInterfaceData->pUsbDev = udev; //aku-aku
	if (pBusInterfaceData == NULL)
	{
		printk("%s: register wlan USB driver to WB_PROXY failed!!\n", __FUNCTION__ );	
		Adap_UnInitialize(pAdap);
		return -EIO;		
	}
    Adap_Set_BusInf(pAdap,pBusInterfaceData);
	if (0 != Bus_openInterface(pBusInterfaceData))
	{
		Adap_Set_BusInf(pAdap,NULL);
		printk("%s: register wlan USB driver to WB_PROXY failed!!\n", __FUNCTION__ );	
		Adap_UnInitialize(pAdap);
		return -EIO;		
	}

#else
	if(0 != Adap_Initialize(pAdap))
	{
		printk("%s: Dsp_Initialize  failed!!\n", __FUNCTION__ );
		Adap_Destroy(pAdap);
		return -EIO;
	}
	if (0 != wlan_netdev_setup(pAdap))
	{
		printk("%s: setup wlan dev failed!!\n", __FUNCTION__ );
       	Adap_UnInitialize(pAdap);
		Adap_Destroy(pAdap);
		return -EIO;
	}
#endif

	printk( "Leave [%s]\n",__FUNCTION__);   
	prt_time();
	return 0;
}


static void _wlan_usb_disconnect(struct usb_interface *intf)
{
	void*	pAdap = NULL; 

	printk("Enter [%s] \n",__FUNCTION__);    
	prt_time();
	
	pAdap = usb_get_intfdata(intf);

	if (pAdap == NULL)
		return;
	Adap_Set_Removed(pAdap,1);
#ifdef WB_PROXY_FLAG
	Bus_closeInterface(Adap_Get_BusInf(pAdap));
#else
	//destroy netdev
	wlan_netdev_destroy(pAdap);
	Adap_Halt(pAdap);
	Adap_UnInitialize(pAdap);
#endif

	/* If usb device exists, release its resource */
	//if (pAdap->usb_context)
		//UsbDev_Release(pAdap);
	Adap_Destroy(pAdap);


	usb_set_intfdata(intf, NULL);
	
	printk("Leave [%s]",__FUNCTION__);
	prt_time();
}


/*
type = 0: get suspend_type, and clear it
type != 0: set suspend_type
*/
int _wlan_suspend_type(int type)
{
	static int suspend_type = -1;
	
	if (type != 0)
	{
		switch (type)
		{
		case PM_EVENT_SUSPEND:
			suspend_type = 2;
			break;
		case PM_EVENT_HIBERNATE:
			suspend_type = 4;
			break;
		case PM_EVENT_FREEZE:
			suspend_type = 1;
			break;
		default:
			suspend_type = -1;
			break;
		}
		return 0;
	}
	
	if ((type == 0) && (suspend_type != -1))
	{	
		type = suspend_type;
		suspend_type = -1;
		return type;
	}

	return -1;
}

static int _wlan_usb_suspend(struct usb_interface *intf, pm_message_t message)
{
	// TODO: cancel timers
	//3 Jakio20080822: this is like " pnp query stop"

	//  1. disable  intsending timers
	//  2. cancel watchdog timers
	//  3. cancel watch dog workqueue
	//  4. cancel tx sending timers
	void * pAdap = NULL; 
	
	pAdap = usb_get_intfdata(intf);

	if (pAdap == NULL)
	{
		printk("enter [%s]: but intfdata=%p, exit (%d)\n",__FUNCTION__, pAdap, message.event);   
		return 0;
	}
		
	printk("enter [%s]: %d  ",__FUNCTION__, message.event); 
	prt_time();

	_wlan_suspend_type(message.event);	

	Adap_Suspend(pAdap, message.event);
	prt_time();
	return 0;
}

static int _wlan_usb_resume(struct usb_interface *intf)
{
	void *	pAdap = NULL; 
	int suspend_type;

	suspend_type = _wlan_suspend_type(0);
	
	pAdap = usb_get_intfdata(intf);
	if (pAdap == NULL)
	{
		printk("enter [%s]: but intfdata = %p, exit (%d)\n",__FUNCTION__, pAdap, suspend_type);   
		return 0;
	}
	prt_time();
	Adap_Resume(pAdap, suspend_type);
	prt_time();
	return 0;
}

#if 1
static int _wlan_usb_reset_resume(struct usb_interface *intf)
{
	void *	pAdap = NULL; 
	int suspend_type;
	pAdap = usb_get_intfdata(intf);
	if (pAdap == NULL)
		return 0;
	suspend_type = _wlan_suspend_type(0);
	
	printk( "entry [%s] %d ",__FUNCTION__, suspend_type);    
	prt_time();

	Adap_Resume(pAdap,suspend_type);		//same to _wlan_usb_resume
	return 0;
}

static int _wlan_usb_pre_reset(struct usb_interface *intf)
{
//	PDSP_ADAPTER_T	pAdap = NULL; 

	printk("entry [%s]\n",__FUNCTION__);   
	return 0;
}

static int _wlan_usb_post_reset(struct usb_interface *intf)
{
//	PDSP_ADAPTER_T	pAdap = NULL; 

	printk("entry [%s]\n",__FUNCTION__);   
	return 0;
}
#endif

/*Driver Information*/
static struct usb_driver _wlan_usb_driver = {
	.name		= "3DSP_WLAN_USB",
	.id_table   = _3dsp_id_tbl,
	.probe      = _wlan_usb_probe,
	.disconnect	= _wlan_usb_disconnect,
	.suspend	= _wlan_usb_suspend,
	.resume		= _wlan_usb_resume,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)	
	.reset_resume	= _wlan_usb_reset_resume,
	.pre_reset	= _wlan_usb_pre_reset,
	.post_reset	= _wlan_usb_post_reset,	
#endif	
};

#ifdef DEBUG_OPEN
static char compiled_time[]={"Compiled at "__TIME__"  [ "__DATE__" ]"};


static void wlan_show_version(void)
{
	printk( "\n");
	printk( "\n");
	printk(  "/*****************************************************************************\\\n" );
	printk(  "|                                                                             |\n" );
	printk( "|                          "WLAN_DRV_NAME"                               |\n");
	printk(  "|                                                                             |\n" );
	printk(  "|   Version : "WLAN_DRV_VERSION"      RX - FIFO                                        |\n");
	printk(  "|                            TX - FIFO                                        |\n");
	printk(  "|                                                                             |\n" );
	printk(  "|                          %s              |\n" , compiled_time);
	printk(  "|                                                                             |\n" );	
	printk(  "|                                                                             |\n" );	
	printk(  "\\*****************************************************************************/\n" );
	printk( "\n");
	printk( "\n");

	return;
	
}

static void wlan_show_msg_exit(void)
{
	printk( "\n");
	printk( "\n");
	printk(  "/******************************************"
				 "***********************************\\\n" );	
	printk( "\n");
	printk(  "    "WLAN_DRV_NAME"\n" );
	printk(  "    Exiting . . . . . .                              (done)\n" );
	printk( "\n");
	printk(  "\\******************************************"
				 "***********************************/\n" );	
	printk( "\n");
	printk( "\n");
	
	return;
}
#endif

static int __init _wlan_usb_init(void)
{
	int err;

#ifdef DEBUG_OPEN
	wlan_show_version();
    tdsp_set_debug();
#endif
    //set syscall function poiter first
    _wlan_syscall_set();
	if ((err = usb_register(&_wlan_usb_driver)) < 0)
    {   
		printk("%s: register wlan USB driver failed!!\n", 
						__FUNCTION__ );
    }
	return err;

	
}

static void __exit _wlan_usb_exit(void)
{

    usb_deregister(&_wlan_usb_driver);
    
#ifdef DEBUG_OPEN
	wlan_show_msg_exit();
#endif

}






MODULE_DEVICE_TABLE(usb, _3dsp_id_tbl);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Tdsp Wireless NIC driver");


module_init(_wlan_usb_init);
module_exit(_wlan_usb_exit);



