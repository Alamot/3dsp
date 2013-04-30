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
 * [lbluetooth_entry.c] description:
 * 	This source file defines the entry point of bluetooth driver 4 linux OS
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
#include <linux/kernel.h>
#include "tdsp_memory.h"
#include "tdsp_tasklet.h"
#include "tdsp_mutex.h"
#include "tdsp_event.h"
#include "tdsp_usb.h"
#include "tdsp_timer.h"
#include "tdsp_time.h"
#include "tdsp_bus.h"
#include "tdsp_work.h"
#include "tdsp_skb.h"
#include "tdsp_file.h"

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)
#include <linux/usb/quirks.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26) 
#include <linux/semaphore.h> 
#else 
#include <asm/semaphore.h> 
#endif

#define USBVENDOR_DSP						0x05e1
#define USBDEVICE_DSP						0x0100

#define HCISETDBGZONE       _IOW('H', 101, int)
#define HCICODECOVERAGE         _IOW('H', 102, int)
#define IOCTL_READ_SCRATCH  _IOW('H', 103, int)
#define IOCTL_READ_DSP_REG      _IOW('H', 104, long)
#define IOCTL_READ_USB_REG      _IOW('H', 105, long)
#define IOCTL_WRITE_USB_REG      _IOW('H', 106, long)
#define IOCTL_DUMP_LMP_PKT      _IOW('H', 107, long)
#define IOCTL_SYS_DIAG       _IOW('H', 108, int)
#define IOCTL_DBG_CRASH      _IOW('H', 109, int)
#define IOCTL_WRITE_DSP_REG      _IOW('H', 110, long)
#define FW_WORK_MODE_SINGLE 0x00

struct dbg_dsp_reg{
    unsigned long offset;
    unsigned long len;
    unsigned char out[256 * 4];
};

struct dbg_usb_reg{
    unsigned long addr;
    unsigned long len;
    unsigned char out[1024];
};

#define MAX_PDU_LENGTH               17

typedef struct _LMP_PUD_PACKAGE 
{
	unsigned char TransID : 1;
	unsigned char OpCode  : 7;
	unsigned char contents[MAX_PDU_LENGTH - 1];
} LMP_PUD_PACKAGE_T, *PLMP_PUD_PACKAGE_T;

struct lmp_dump{
    unsigned short hConn;
    unsigned char tx;
    unsigned int tsec;
    unsigned int tms;
    LMP_PUD_PACKAGE_T lmpPdu;
};

#define BT_CRASH()  do{*(char *)0 = 1;}while(0)

unsigned int combodrv = 1;
module_param(combodrv, uint, S_IRUGO);

int hci_notify_frame(void *databuf);

int tdsp_print(const char * fmt,...) 
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return r;
}

static struct usb_device_id _3dsp_id_tbl[] = {
	/* 3dsp usb bluetooth */
	{ USB_DEVICE(USBVENDOR_DSP, USBDEVICE_DSP) },
	{ }	/* Terminating entry */
};

// Mutex to sync between open and close
static struct semaphore openLock;

static SYS_CALL syscall;

static void _bluetooth_syscall_set(void)
{
	syscall.spin_lock_init	= tdsp_spin_lock_init;
	syscall.spin_lock		= tdsp_spin_lock;
	syscall.spin_lock_bh	= tdsp_spin_lock_bh;
	syscall.spin_lock_irqsave	= tdsp_spin_lock_irqsave;
	syscall.spin_unlock		= tdsp_spin_unlock;
	syscall.spin_unlock_bh	= tdsp_spin_unlock_bh;
	syscall.spin_unlock_irqrestore	= tdsp_spin_unlock_irqrestroe;
	syscall.spin_lock_kill	= tdsp_spin_lock_kill;

	syscall.memory_alloc	= tdsp_memory_alloc;
	syscall.memory_alloc_atomic	= tdsp_memory_alloc_atomic;
	syscall.memory_free	= tdsp_memory_free;
	syscall.memory_set	= tdsp_memset;
	syscall.memory_copy 	= tdsp_memcpy;
	syscall.memory_cmp	= tdsp_memcmp;
	syscall.memory_vmalloc		= tdsp_memory_vmalloc;
	syscall.memory_vfree	= tdsp_memory_vfree;
	syscall.memory_move	= tdsp_memory_move;
	syscall.zero_memory	= tdsp_zero_memory;
	
	syscall.event_init		= tdsp_event_init;
	syscall.event_set		= tdsp_event_set;
	syscall.event_wait		= tdsp_event_wait;
	syscall.event_kill		= tdsp_event_kill;
	syscall.event_flag_set		= tdsp_event_flag_set;
	
	syscall.timer_init		= tdsp_timer_init;
	syscall.timer_start		= tdsp_timer_start;
	syscall.timer_stop		= tdsp_timer_stop;
	syscall.timer_stop_sync	= tdsp_timer_stop_sync;
	syscall.timer_reset		= tdsp_timer_reset;
	syscall.timer_kill		= tdsp_timer_kill;
	syscall.ms_to_ticks	= tdsp_ms_to_ticks;
	syscall.get_ticks		= tdsp_get_ticks;
	syscall.usdelay		= tdsp_usdelay;
	syscall.msdelay		= tdsp_msdelay;

	syscall.tasklet_init		= tdsp_tasklet_init;
	syscall.tasklet_schedule	= tdsp_tasklet_schedule;
	syscall.tasklet_enable	= tdsp_tasklet_enable;
	syscall.tasklet_disable	= tdsp_tasklet_disable;
	syscall.tasklet_kill		= tdsp_tasklet_kill;
    
	syscall.usb_urb_alloc	= tdsp_usb_urb_alloc;
	syscall.usb_urb_free	= tdsp_usb_urb_free;
	syscall.usb_urb_unlink 	= tdsp_usb_urb_unlink;
	syscall.usb_get_status	= tdsp_usb_get_status;
	syscall.usb_get_context		= tdsp_usb_get_context;
	syscall.usb_get_len	= tdsp_usb_get_len;
	syscall.usb_urb_kill	= tdsp_usb_urb_kill;
	
	syscall.usb_alloc_ctrlreq	= tdsp_usb_alloc_ctrlreq;
	syscall.usb_set_ctrlreq	= tdsp_usb_set_ctrlreq;
	syscall.usb_free_ctrlreq	= tdsp_usb_free_ctrlreq;
	syscall.usb_ctrlreq		= tdsp_usb_ctrl_req;
	syscall.usb_rcvint		= tdsp_usb_rcvint;
	syscall.usb_bulkin		= tdsp_usb_bulkin;
	syscall.usb_bulkout	= tdsp_usb_bulkout;
	
	syscall.bus_set_fw_ver    = Bus_set_firmware_version;
	syscall.bus_get_hkey_flag = Bus_get_hotkey_flag;

	syscall.print                = tdsp_print;

/*  ------------------  Addtional Functions  ----------------  */
	syscall.hci_notify_frame		= hci_notify_frame;

	syscall.workthread_create	= tdsp_workthread_create;
	syscall.delaywork_init	= tdsp_delaywork_init;
	syscall.work_init		= tdsp_work_init;
	syscall.queue_delayedwork	= tdsp_queue_delayedwork;
	syscall.cancel_delayedwork	= tdsp_cancel_delayedwork;
	syscall.queue_work	= tdsp_queue_work;
	syscall.destory_workqueue	= tdsp_destory_workqueue;
       syscall.delaywork_release       = tdsp_delaywork_release;
	syscall.work_release		= tdsp_work_release;

	syscall.skb_queue_init	= tdsp_skb_queue_init;
	syscall.skb_queue_release	= tdsp_skb_queue_release;
	syscall.skb_get_queue_len	= tdsp_skb_get_queue_len;
    	syscall.skb_put_queue_tail	= tdsp_skb_put_queue_tail;
	syscall.skb_is_queue_empty	= tdsp_skb_is_queue_empty;
	syscall.skb_dequeue_skb	= tdsp_skb_dequeue_skb;
	syscall.skb_alloc_atomic	= tdsp_skb_alloc_atomic;
	syscall.skb_kfree			= tdsp_skb_kfree;
	syscall.put_to_skb			= tdsp_put_to_skb;
	syscall.skb_get_len		= tdsp_skb_get_len;
	syscall.skb_get_dataptr		= tdsp_skb_get_dataptr;
	syscall.skb_set_dev		= tdsp_skb_set_dev;
	syscall.skb_set_btpkt_type	= tdsp_skb_set_btpkt_type;
	syscall.bt_skb_alloc_atomic	= tdsp_bt_skb_alloc_atomic;

	syscall.usb_get_sndbulkpipe	= tdsp_usb_get_sndbulkpipe;
	syscall.usb_submit_bulkouturb	= tdsp_usb_submit_bulkouturb;
	syscall.usb_ctrl_msg_b		= tdsp_usb_ctrl_msg_b;
	syscall.usb_get_reqtype		= tdsp_usb_get_reqtype;
	syscall.usb_status_nodev	= tdsp_usb_status_nodev;
	syscall.usb_status_inval	= tdsp_usb_status_inval;
	syscall.usb_status_shutdown	= tdsp_usb_status_shutdown;
	syscall.usb_status_proto	= tdsp_usb_status_proto;
	syscall.usb_status_connreset	= tdsp_usb_status_connreset;
	syscall.usb_status_noent	= tdsp_usb_status_noent;

	syscall.openfile		= tdsp_openfile;
	syscall.closefile		= tdsp_closefile;
	syscall.open_success		= tdsp_open_success;
	syscall.readfile		= tdsp_readfile;
	
	syscall.isin_atomic             = tdsp_isin_atomic;

	syscall.get_combodrv		= tdsp_get_combodrv;
}

#ifdef DEBUG_OPEN__BLUETOOTH
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

static int hci_usb_open(struct hci_dev *hdev)
{
	void *devExt = hci_get_drvdata(hdev);

	if (test_and_set_bit(HCI_RUNNING, &hdev->flags))
	{   
		return 0;
	}

	if(Bluetooth_Adap_IsOpen(devExt))
	{
		printk("%s: Device %s has already OPENED!\n", __FUNCTION__, hdev->name);    
		return -EIO;
	}

	Bluetooth_Adap_SetOpen(devExt);

	/* Try to be MASTER aggressively */
	hdev->link_mode |= HCI_LM_MASTER;

	Bluetooth_Adap_Init(hci_get_drvdata(hdev));
    
	if(combodrv){
		Bluetooth_Adap_Set_FirmVer(devExt);
	}

	return 0;
}

static int hci_usb_close(struct hci_dev *hdev)
{
	void *devExt = NULL;

	devExt = hci_get_drvdata(hdev);

	if (!test_and_clear_bit(HCI_RUNNING, &hdev->flags)){
		printk("%s: %s NOT Running\n", __FUNCTION__, hdev->name);    
		return 0;
	}

	if(!Bluetooth_Adap_IsOpen(devExt))
	{
		printk("%s: %s NOT Opened\n", __FUNCTION__, hdev->name);    
		goto end;
	}

	Bluetooth_Adap_Exit_Lmp_Dump(devExt);

end:
	Bluetooth_Adap_ClearOpen(devExt);
	return 0;
}

static int hci_usb_flush(struct hci_dev *hdev)
{
	return 0;
}

static int hci_usb_send_frame(struct sk_buff *skb)
{
	int retval = 0;
	struct hci_dev *hdev = (struct hci_dev *) skb->dev;
	void	*devExt = hci_get_drvdata(hdev);
    
	if (!hdev) {
		printk("%s: Frame for uknown device (hdev=NULL)\n", __FUNCTION__);
		return -ENODEV;
	}

	if (!test_bit(HCI_RUNNING, &hdev->flags)){
		printk("%s: HCI Not Running\n", __FUNCTION__);
		return -EBUSY;
	}

	if(!Bluetooth_Adap_IsOpen(devExt))
	{
		printk("%s: Device %s NOT OPENED\n", __FUNCTION__, hdev->name);    
		return -EIO;
	}

	if(Bluetooth_Adap_IsInD3(devExt))
	{
		printk("%s: Device %s in D3\n", __FUNCTION__, hdev->name);    
		return -EBUSY;
	}
    
	switch (bt_cb(skb)->pkt_type) {
	case HCI_COMMAND_PKT:
		hdev->stat.cmd_tx++;
		Bluetooth_Adap_SendCmd(devExt, skb);     
		break;
	case HCI_ACLDATA_PKT:
		hdev->stat.acl_tx++;
		
		Bluetooth_Adap_SendData(devExt, skb);
		kfree_skb(skb);
		break;
	case HCI_SCODATA_PKT:
		hdev->stat.sco_tx++;
		Bluetooth_Adap_SendData(devExt, skb);
		kfree_skb(skb);
		break;
	default:
		kfree_skb(skb);
		return 0;
	}
	
	return retval;
}

static void hci_usb_destruct(struct hci_dev *hdev)
{
}

static void hci_usb_notify(struct hci_dev *hdev, unsigned int evt)
{
}

static int hci_usb_ioctl(struct hci_dev *hdev, unsigned int cmd, unsigned long arg)
{
	struct dbg_dsp_reg *d_reg = NULL;
	struct dbg_usb_reg *u_reg = NULL;
	int errcode = 0;

	void *devExt = hci_get_drvdata(hdev);
	
	if (!test_bit(HCI_RUNNING, &hdev->flags))
	{
		printk("%s: %s NOT running\n", __FUNCTION__, hdev->name);
		return -1;
	}

	switch(cmd){
//^^BLUETOOTH_DEBUG^^
	case HCISETDBGZONE:
		printk(KERN_INFO "Debug zone 0x%04x-->0x%04x, debug level 0x%04x-->0x%04x\n", 
			dbg_zone, (arg & 0xFFFF0000), dbg_level, arg & 0xFFFF);
		dbg_zone = arg & (0xFFFF << 16);    
		dbg_level = arg & 0xFFFF;
		
		break;
//vvBLUETOOTH_DEBUGvv
	case IOCTL_READ_DSP_REG:
		{
			d_reg = (struct dbg_dsp_reg *)kzalloc(sizeof(struct dbg_dsp_reg), GFP_KERNEL);
			if(d_reg == NULL)
			{
				errcode = -EFAULT;
				break;
			}
			
			if(copy_from_user(d_reg, (char __user*)arg, sizeof(struct dbg_dsp_reg)))
			{
				errcode = -EFAULT;
				break;
			}
			
			if(Bluetooth_Adap_ReadDspReg(devExt, d_reg->offset, d_reg->len, d_reg->out)){
				printk("%s: Read DSP register failure!!\n", __FUNCTION__);
				errcode = -ENODEV;
				break;
			}else{
				if(copy_to_user((char __user*)arg, d_reg, sizeof(struct dbg_dsp_reg)))
				{
					errcode = -EFAULT;
				}
			}
		}
		break;
	case IOCTL_WRITE_DSP_REG:
		{
			d_reg = (struct dbg_dsp_reg *)kzalloc(sizeof(struct dbg_dsp_reg), GFP_KERNEL);
			if(d_reg == NULL)
			{
				errcode = -EFAULT;
				break;
			}
			
			if(copy_from_user(d_reg, (char __user*)arg, sizeof(struct dbg_dsp_reg)))
			{
				errcode = -EFAULT;
				break;
			}
			
			if(Bluetooth_Adap_WriteDspReg(devExt, d_reg->offset, d_reg->len, d_reg->out)){
				printk("%s: Write DSP register failure!!\n", __FUNCTION__);
				errcode = -ENODEV;
				break;
			}
		}
		break;
	case IOCTL_READ_USB_REG:
		{
			u_reg = (struct dbg_usb_reg *)kzalloc(sizeof(struct dbg_usb_reg), GFP_KERNEL);
			if(u_reg == NULL)
			{
				errcode = -EFAULT;
				break;
			}
			
			if(copy_from_user(u_reg, (char __user*)arg, sizeof(struct dbg_usb_reg)))
			{
				errcode = -EFAULT;
				break;
			}
			
			if(Bluetooth_Adap_ReadUsbReg(devExt, u_reg->addr, u_reg->out, u_reg->len)){
				printk("%s: Read USB register failure!!\n", __FUNCTION__);
				errcode = -ENODEV;
				break;
			}else{
				if(copy_to_user((char __user*)arg, u_reg, sizeof(struct dbg_usb_reg)))
				{
					errcode = -EFAULT;
				}	
			}
		}
		break;
	case IOCTL_WRITE_USB_REG:
		{
			u_reg = (struct dbg_usb_reg *)kzalloc(sizeof(struct dbg_usb_reg), GFP_KERNEL);
			if(u_reg == NULL)
			{
				errcode = -EFAULT;
				break;
			}
			
			if(copy_from_user(u_reg, (char __user*)arg, sizeof(struct dbg_usb_reg)))
			{
				errcode = -EFAULT;
				break;
			}
			
			if(Bluetooth_Adap_WriteUsbReg(devExt, u_reg->addr, &u_reg->len, sizeof(unsigned char))){
				printk("%s: Write USB register failure !!\n", __FUNCTION__);
				errcode = -ENODEV;
			}
		}
		break;
	case IOCTL_DUMP_LMP_PKT:
		{
			struct sk_buff *skb;
			struct lmp_dump *pOut;
			skb = (struct sk_buff *)Bluetooth_Adap_Get_Dumpdata(devExt);
			if(skb)
			{
				pOut = (struct lmp_dump *)skb->data;
				if(copy_to_user((char __user*)arg, pOut, sizeof(struct lmp_dump)))
				{
					errcode = -EFAULT;
				}
				kfree_skb(skb);
			}
		}
		break;
	case IOCTL_SYS_DIAG:
		{
			Bluetooth_Adap_DiagSys(devExt);
			break;
		}
	case IOCTL_DBG_CRASH:
		{
			printk("%s: Crash the system for debug\n", __FUNCTION__);
			BT_CRASH();
			break;
		}
	default:
		printk("%s: %s Unknown debug command 0x%x\n", __FUNCTION__, hdev->name, cmd);
	}
	
	if(d_reg)
		kfree(d_reg);
	if(u_reg)
		kfree(u_reg);
	return errcode;
}

void *register_HCI_device(void *devExt)
{
	struct hci_dev *hdev = NULL;
	struct usb_interface *intf = (struct usb_interface *)Bluetooth_Adap_Get_uintf(devExt);

       if (down_interruptible(&openLock) == -EINTR)
	   	return NULL;
	
	if(0 != Bluetooth_Adap_Open_Dev(devExt)){
		goto end;
	}
    
	/* Initialize and register HCI device */
	hdev = hci_alloc_dev();
	if (!hdev) {
		printk("%s: Can't allocate HCI device\n", __FUNCTION__);
		goto end;
	}

	hci_set_drvdata(hdev, devExt);
	SET_HCIDEV_DEV(hdev, &intf->dev);
	hdev->open		= hci_usb_open;
	hdev->close		= hci_usb_close;
	hdev->flush		= hci_usb_flush;
	hdev->send		= hci_usb_send_frame;
	//hdev->destruct	= hci_usb_destruct;
	hdev->notify		= hci_usb_notify;
	hdev->ioctl		= hci_usb_ioctl;
    
	//hdev->owner = THIS_MODULE;

	if (hci_register_dev(hdev) < 0) {
		printk("%s: Can't register HCI device\n", __FUNCTION__);
		hci_free_dev(hdev);
		hdev = NULL;
		goto end;
	}

	Bluetooth_Adap_Set_hdev(devExt, hdev);
end:    
	up(&openLock);
 	return hdev;
}


void unregister_HCI_device(void *devExt)
{
	struct hci_dev *hdev = NULL;
	
       if (down_interruptible(&openLock) == -EINTR)
	   	return;
	if (devExt == NULL)
	{
		printk("%s, devExt = NULL\n", __FUNCTION__);
		goto end;
	}

	hdev = (struct hci_dev *)Bluetooth_Adap_Get_hdev(devExt);
	if (hdev == NULL)
	{
		printk("%s, devExt->hdev = NULL\n", __FUNCTION__);
		goto end;
	}

	hci_unregister_dev(hdev);
	/*if (hci_unregister_dev(hdev) < 0)
		printk("%s: Can't unregister HCI device %s\n", __FUNCTION__, hdev->name);*/

	// Close Dev
	Bluetooth_Adap_Close_Dev(devExt);

	// Free hdev
	Bluetooth_Adap_Set_hdev(devExt, NULL);
	hci_free_dev(hdev);
	
end:
	up(&openLock);
}


static int _bluetooth_usb_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct usb_host_endpoint  *ep;
	struct usb_host_interface *uif;
	struct hci_dev *hdev = NULL;
	volatile USB_INFO usb_info;
	void *devExt;
	void * pBusInterface;
	int e;

	sema_init(&openLock,1);
  
	if (!id->driver_info) 
	{
		const struct usb_device_id *match;
		match = usb_match_id(intf, _3dsp_id_tbl);
		if (match)
			id = match;
	}

	if (intf->cur_altsetting->desc.bInterfaceNumber != TDSP_BT_INTERFACE_NUM)
	{
		printk("%s: intf %d is not 3dsp bluetooth interface\n", __FUNCTION__, intf->cur_altsetting->desc.bInterfaceNumber);
		return -ENODEV;
	}

	/* Find endpoints that we need */
	uif = intf->cur_altsetting;

	/*
	printk("%s: 3dsp usb bluetooth device found:) intf %d, alt configs %d, endpoints %d\n", __FUNCTION__, 
		uif->desc.bInterfaceNumber, intf->num_altsetting, uif->desc.bNumEndpoints);
	*/

	if (uif->desc.bNumEndpoints < EP_COUNT) 
	{
		printk("%s: endpoints not enough!!\n", __FUNCTION__);
			return -EIO;
	}

	/* Only for debug, print out the endpoint informations */
	for (e = 0; e < uif->desc.bNumEndpoints; e++) 
	{
		ep = &uif->endpoint[e];
		printk("%s: ep addr %d, type %d, in/out %s\n", __FUNCTION__, 
				ep->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK,
				ep->desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK,
				ep->desc.bEndpointAddress & USB_DIR_IN ? " IN" : "OUT");
	}

	printk("%s: sc is %x\n", __FUNCTION__, (unsigned int)&syscall);
	b_syscall_set((PSYS_CALL)&syscall);


    
	/* Hard code to get the ep configuration according HW setting */
	usb_info.EP_control = INDEX_EP_CTRL;
	usb_info.EP_bulkout_ACL = INDEX_EP_BULK_OUT_ACL;
	usb_info.EP_bulkout_SCO = INDEX_EP_BULK_OUT_SCO;
	usb_info.EP_bulkin_Inquiry = INDEX_EP_BULK_IN_INQ;
	usb_info.EP_bulkin_Rx = INDEX_EP_BULK_IN_DATA;
	usb_info.EP_interrupt = INDEX_EP_INTR;

	printk("%s: usb_info is %x\n", __FUNCTION__, (unsigned int)&usb_info);
	if(NULL == (devExt= Bluetooth_Adap_Create((PUSB_INFO)&usb_info, udev, intf)))
	{
		printk("%s: Usb device Init failed \n",__FUNCTION__);
		return -EIO;
	}

	//udev->quirks |= USB_QUIRK_RESET_RESUME;
	usb_set_intfdata(intf, devExt);

	if(combodrv){
		pBusInterface = Bus_new_interface(devExt);
		if (NULL == pBusInterface)
		{
			printk("%s: register bluetooth USB driver failed!!\n", __FUNCTION__ );	
			Bluetooth_Adap_Destory(devExt);
			return -EIO;		
		}
		Bluetooth_Adap_Set_BusInf(devExt, pBusInterface);
		if (0 != Bus_openInterface(pBusInterface))
		{
			
			Bluetooth_Adap_Set_BusInf(devExt, NULL);
			printk("%s: register BT USB driver to BUS Driver failed!! 2\n", __FUNCTION__);	
			Bluetooth_Adap_Destory(devExt);
		}
	}

	if(0 == combodrv)
	{
		if (!(hdev = register_HCI_device(devExt)))
		{
			printk("%s: register BT USB driver to BUS Driver failed!!\n", __FUNCTION__);	
			goto probe_error;	
		}
	}

	return 0;

probe_error:
	printk("%s: Error in probe\n", __FUNCTION__);
	return -EIO;
}

static void _bluetooth_usb_disconnect(struct usb_interface *intf)
{
	void	*devExt = NULL; 
	struct hci_dev *hdev = NULL;
	
	devExt = usb_get_intfdata(intf);
	usb_set_intfdata(intf, NULL);
	hdev = (struct hci_dev *)Bluetooth_Adap_Get_hdev(devExt);

	if(!hdev){
		printk("%s: NO hdev registered\n", __FUNCTION__);
	}else{
		printk("%s: Disconnect %s\n", __FUNCTION__, hdev->name);
        	down_interruptible(&openLock);
        	hci_usb_close(hdev);
		up(&openLock);
	}

	if(combodrv)
	{
		if (0 != Bus_closeInterface(Bluetooth_Adap_Get_BusInf(devExt)))
		{
			printk("%s: register BT USB driver to BUS Driver failed!!\n", __FUNCTION__);	
		}
		Bluetooth_Adap_Set_BusInf(devExt, NULL);
	}else{
		unregister_HCI_device(devExt);
	}

	if(devExt){
		printk("%s: Free Device Context\n", __FUNCTION__);
		Bluetooth_Adap_Destory(devExt);
	}
	printk("%s: Out...", __FUNCTION__);
}

static int _bluetooth_usb_suspend(struct usb_interface *intf, pm_message_t message)
{
	void *devExt = NULL; 
	struct hci_dev *hdev = NULL;
	struct usb_device *udev = NULL;

	printk("Suspend %d\n", message.event);

	devExt = usb_get_intfdata(intf);
	if(!(hdev = Bluetooth_Adap_Get_hdev(devExt))){
		return 0;
	}
    
	hci_suspend_dev(hdev);

	Bluetooth_Adap_Suspend(devExt);
	if(PM_EVENT_SUSPEND != message.event 
		&& FW_WORK_MODE_SINGLE == Bluetooth_Adap_GetWorkMod(devExt)){
		udev = (struct usb_device *)Bluetooth_Adap_GetDev(devExt);
		udev->state = USB_STATE_SUSPENDED;
	}
	
	return 0;
}

static int _bluetooth_usb_resume(struct usb_interface *intf)
{
	void *devExt = NULL;
	struct hci_dev *hdev = NULL;

	devExt = usb_get_intfdata(intf);
	hdev = (struct hci_dev *)Bluetooth_Adap_Get_hdev(devExt);
	
	if(!hdev){
		return 0;
	}

	Bluetooth_Adap_Resume(devExt);
	
	hci_resume_dev(hdev);
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)
static int _bluetooth_usb_reset_resume(struct usb_interface *intf)
{
	return _bluetooth_usb_resume(intf);
}

static int _bluetooth_usb_pre_reset(struct usb_interface *intf)
{
	return 0;
}

static int _bluetooth_usb_post_reset(struct usb_interface *intf)
{
	return 0;
}
#endif

/*Driver Information*/
static struct usb_driver _bluetooth_usb_driver = {
	.name		= "3DSP_BLUETOOTH_USB",
	.id_table   = _3dsp_id_tbl,
	.probe      = _bluetooth_usb_probe,
	.disconnect	= _bluetooth_usb_disconnect,
	.suspend	= _bluetooth_usb_suspend,
	.resume		= _bluetooth_usb_resume,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)	
	.reset_resume	= _bluetooth_usb_reset_resume,
	.pre_reset	= _bluetooth_usb_pre_reset,
	.post_reset	= _bluetooth_usb_post_reset,	
#endif	
};

static int __init _bluetooth_usb_init(void)
{
	int err;
	
	_bluetooth_syscall_set();
	if ((err = usb_register(&_bluetooth_usb_driver)) < 0)
	{   
		printk("%s: Failed to register Bluetooth Usb driver!!\n", __FUNCTION__ );
	}
	return err;
}

static void __exit _bluetooth_usb_exit(void)
{
	usb_deregister(&_bluetooth_usb_driver);
}

int hci_notify_frame(void *databuf)
{
	struct sk_buff *skb = (struct sk_buff *)databuf;	
	hci_recv_frame(skb);

	return 0;
}

MODULE_DEVICE_TABLE(usb, _3dsp_id_tbl);
MODULE_LICENSE("GPL");

module_init(_bluetooth_usb_init);
module_exit(_bluetooth_usb_exit);
