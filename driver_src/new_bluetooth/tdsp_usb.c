 /***********************************************************************
  * Copyright(C) 2006 3DSP Corporation. All Rights Reserved.
  
  * FILENAME		:tdsp_usb.c         VERSION:1.0
  * CREATE DATE	:2009/06/08
  * PURPOSE:	This file includes functions related to usb syscalls which is used 
  *             by usb linux driver.  
  *
  * AUTHORS:
  *
  * DECLARATION:  This document contains confidential proprietary information that
  *               is solely for authorized personnel. It is not to be disclosed to
  *               any unauthorized person without prior written consent of 3DSP
  *               Corporation.		
  ***********************************************************************/
 

#include "tdsp_usb.h"
#include "tdsp_memory.h"

/* URB complete proxy */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20)
struct pt_regs;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
static void tdsp_usb_callback_bulkin_data(struct urb *purb)
#else
static void tdsp_usb_callback_bulkin_data(struct urb *purb, struct pt_regs *pt)
#endif
{
	Bluetooth_Adap_Read_Compelet((void*)purb);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
static void tdsp_usb_callback_bulkin_inq(struct urb *purb)
#else
static void tdsp_usb_callback_bulkin_inq(struct urb *purb, struct pt_regs *pt)
#endif
{
	Bluetooth_Adap_Inq_Compelet((void*)purb);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
static void tdsp_usb_callback_bulkout(struct urb *purb)
#else
static void tdsp_usb_callback_bulkout(struct urb *purb, struct pt_regs *pt)
#endif
{
	Bluetooth_Adap_Write_Compelet((void*)purb);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
static void tdsp_usb_callback_int(struct urb *purb)
#else
static void tdsp_usb_callback_int(struct urb *purb, struct pt_regs *pt)
#endif
{
	Bluetooth_Adap_Int_Compelet((void*)purb);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
static void tdsp_usb_callback_ctrl(struct urb *purb)
#else
static void tdsp_usb_callback_ctrl(struct urb *purb, struct pt_regs *pt)
#endif
{
	Bluetooth_Adap_ctrl_Compelet((void*)purb);
}

void* tdsp_usb_urb_alloc(void)
{
	return (usb_alloc_urb(0, GFP_KERNEL));
}
void tdsp_usb_urb_free(void* purb)
{
	usb_free_urb((struct urb*)purb);
}
int tdsp_usb_urb_unlink(void* purb)
{
	return (usb_unlink_urb((struct urb*)purb));
}
void tdsp_usb_urb_kill(void* purb)
{
	usb_kill_urb((struct urb*)purb);
}
int tdsp_usb_get_status(void* purb)
{
	struct urb* u = (struct urb*)purb;   
	return (u->status);
}
void* tdsp_usb_get_context(void* purb)
{
	struct urb* u = (struct urb*)purb;  
	return (u->context);
}
int tdsp_usb_get_len(void* purb)
{
	struct urb* u = (struct urb*)purb;  
	return (u->actual_length);
}
void* tdsp_usb_alloc_ctrlreq(void)
{
	struct usb_ctrlrequest * ctrl_req;
	ctrl_req = (struct usb_ctrlrequest *)kmalloc(sizeof(struct usb_ctrlrequest), GFP_KERNEL);
	if(NULL == ctrl_req)
	{
		printk("%s :can not alloc memory for ctrl req\n",__FUNCTION__);
	}
	return ctrl_req;
}
void tdsp_usb_free_ctrlreq(void * set_up)
{
	struct usb_ctrlrequest * ctrl_req = (struct usb_ctrlrequest * )set_up;
	if(NULL == ctrl_req)
	{
		printk("%s: ctlr req is NULL\n",__FUNCTION__);
		return;
	}
	kfree(ctrl_req);
}
void tdsp_usb_set_ctrlreq(void * ctrl_req, PTDSP_USB_CTRLREQ req)
{
	struct usb_ctrlrequest*	set_up = (struct usb_ctrlrequest*)ctrl_req;
	if(NULL == set_up)
	{
		printk("%s: ctlr req is NULL\n",__FUNCTION__);    
	}
	set_up->bRequestType = USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
	set_up->bRequest = req->request;
	set_up->wValue = cpu_to_le16p(&req->value);
	set_up->wIndex = cpu_to_le16p(&req->index);
	set_up->wLength = cpu_to_le16p((unsigned short *)&req->len);
}
int tdsp_usb_ctrl_req(void * urb,
						void * usbdev,
						unsigned char ep,
						void * buffer,
						int len,
						void* context,
						void* request)
{
	int ret;
	int pipe = usb_sndctrlpipe((struct usb_device *)usbdev, ep);
	usb_fill_control_urb((struct urb*)urb,
					(struct usb_device *)usbdev,
    					pipe,
    					(unsigned char *)request,	//Need to be confirmed for this var
    					buffer,
    					len,
    					tdsp_usb_callback_ctrl,
    					(void *)context);
	ret = usb_submit_urb((struct urb*)urb, GFP_ATOMIC);
	return ret;
}

int tdsp_usb_rcvint(void * urb,
                     void * usbdev,
                     unsigned char ep,
                     void * buffer,
                     int len,
                     void* context,
                     int interval)
{
	int ret;
	int pipe;
	pipe = usb_rcvintpipe((struct usb_device *)usbdev,ep);
	usb_fill_int_urb((struct urb*)urb,
				(struct usb_device *) usbdev,
				pipe,
    				buffer,
    				len,
    				tdsp_usb_callback_int,
    				context,
    				7);
	ret = usb_submit_urb((struct urb*)urb, GFP_ATOMIC);
	return ret;
}    
int tdsp_usb_bulkin(void *purb, void *usbdev, unsigned char ep,
				void * buffer, int len, void* context)
{
	int ret;
	int pipe;
	struct urb* u = (struct urb*)purb;
	pipe = usb_rcvbulkpipe((struct usb_device *)usbdev,ep);

	if(ep == INDEX_EP_BULK_IN_DATA)
		usb_fill_bulk_urb(	u,
					usbdev,
					pipe,
					buffer,
					len,
					tdsp_usb_callback_bulkin_data,
					context);
	else
		usb_fill_bulk_urb(	u,
					usbdev,
					pipe,
					buffer,
					len,
					tdsp_usb_callback_bulkin_inq,
					context);

	u->transfer_flags &= ~URB_SHORT_NOT_OK;
	ret = usb_submit_urb(u, GFP_ATOMIC);
	return ret;
}   

int tdsp_usb_bulkout(void * urb,
                         void * usbdev,
                         unsigned char ep,
                         void * buffer,
                         int len,
                         void* context)
{
    int ret;
    int pipe;
    pipe = usb_sndbulkpipe((struct usb_device *)usbdev,ep);
	usb_fill_bulk_urb((struct urb*)urb,
				        usbdev,
				        pipe,
				        buffer,
				        len,
				        tdsp_usb_callback_bulkout,
				        context);
    ret = usb_submit_urb((struct urb*)urb, GFP_ATOMIC);
	return ret;
} 

unsigned int tdsp_usb_get_sndbulkpipe(void* hdev, unsigned int ep)
{
	if(hdev == NULL)
	{
		printk("%s: device is NULL\n",__FUNCTION__);
	}
	return (usb_sndbulkpipe((struct usb_device *)hdev, ep));
}

int tdsp_usb_ctrl_msg_b(void *dev,
							int requesttype, 
							int request, 
							int value, 
							int index, 
							char *buff, 
							int size, 
							int timeout, 
							unsigned int direct)
{
	int ret;
	int pipe;
	if(direct == VENDOR_CMD_IN)
		pipe = usb_rcvctrlpipe((struct usb_device *)dev, INDEX_EP_CTRL);
	else
		pipe = usb_sndctrlpipe((struct usb_device *)dev, INDEX_EP_CTRL);

	ret = usb_control_msg((struct usb_device *)dev, 
						pipe,
						request, 
						requesttype, 	
						value, 
						index, 
						buff, 
						size, 
						timeout 
						);
	return ret;
}

int tdsp_usb_submit_bulkouturb(void* purb, 
									void* udev, 
									unsigned int pipe, 
									void* buffer, 
									int buffer_len, 
									void* context)
{
	struct urb *urb = (struct urb *)purb;
	usb_fill_bulk_urb(urb,
                                (struct usb_device *)udev,
                                pipe,
                                buffer,
                                buffer_len,
                                tdsp_usb_callback_bulkout,
                                context);
	//urb->transfer_flags |= URB_ASYNC_UNLINK;
	return (usb_submit_urb(urb, GFP_ATOMIC));
}

int tdsp_usb_status_nodev(int status)
{
	return (status == -ENODEV);
}
int tdsp_usb_status_inval(int status)
{
	return (status == -EINVAL);
}
int tdsp_usb_status_shutdown(int status)
{
	return (status == -ESHUTDOWN);
}
int tdsp_usb_status_proto(int status)
{
	return (status == -EPROTO);
}
int tdsp_usb_status_connreset(int status)
{
	return (status == -ECONNRESET);
}
int tdsp_usb_status_noent(int status)
{
	return (status == -ENOENT);
}
unsigned char tdsp_usb_get_reqtype(unsigned int direct)
{
	if(direct == VENDOR_CMD_IN)
		return (unsigned char)(USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE);
	else
		return (unsigned char)(USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE);
}
