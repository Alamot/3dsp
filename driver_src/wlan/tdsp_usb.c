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
 
#include <linux/sched.h>
#include "tdsp_usb.h"
#include "tdsp_memory.h"

static void tdsp_usb_callback(struct urb *urb)
{
    Usb_Completion_Imp((void *) urb);
}

void * tdsp_usb_alloc_urb(void)
{
    return (void *)usb_alloc_urb(0, GFP_ATOMIC);
}

void tdsp_usb_free_urb(void * urb)
{
    usb_free_urb((struct urb*)urb);
}

void tdsp_usb_unlink_urb(void * urb)
{
    usb_unlink_urb((struct urb*)urb);
}

void tdsp_usb_kill_urb(void * urb)
{
    usb_kill_urb((struct urb*)urb);
}

int tdsp_usb_get_status(void * urb)
{
    struct urb* u = (struct urb*)urb;   
    return u->status;
}

void* tdsp_usb_get_context(void * urb)
{
    struct urb* u = (struct urb*)urb;  
    return u->context;
}

int tdsp_usb_is_srmv(int status)
{
	if((status == -ENODEV)
		|| (status == -EINVAL)
		|| (status == -ESHUTDOWN) 
		|| (status == (-71)))
	{
		return 1;
    }
    return 0;
}

int tdsp_usb_get_len(void* urb)
{
    struct urb* u = (struct urb*)urb;  
    return u->actual_length;
}

int tdsp_usb_ctrl_msg(void * usbdev,
                             unsigned char ep,
                             unsigned char type,
                             void * buffer,
                             int len,
 						     void* request)
{
    int ret;
    int pipe;
    PTDSP_USB_CTRLREQ req = (PTDSP_USB_CTRLREQ)request;
    // TODO: need to check input para validity
    if(type == USB_WRITE_OP)
    {
        type = USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
        pipe = usb_sndctrlpipe((struct usb_device *)usbdev, ep);
    }
    else 
    {   
        type = USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
        pipe = usb_rcvctrlpipe((struct usb_device *)usbdev, ep);
    }

    ret = usb_control_msg(usbdev, 
						pipe,
						req->request,					  //request value
						type,					 //request type
						req->value,					//valuse
						req->index,					//index
						buffer,		//data buffer
						len,
						0);				//timeout is 10s
   return ret;
}

void* tdsp_usb_alloc_ctrlreq(void)
{
    struct usb_ctrlrequest * ctrl_req;
    ctrl_req = (struct usb_ctrlrequest *)tdsp_memory_alloc(sizeof(struct usb_ctrlrequest));
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
    tdsp_memory_free(ctrl_req);
}

void tdsp_usb_set_ctrlreq(void * ctrl_req,void * req)
{
    struct usb_ctrlrequest*	set_up = (struct usb_ctrlrequest*)ctrl_req;
    PTDSP_USB_CTRLREQ req_cntxt= (PTDSP_USB_CTRLREQ)req;
    if(NULL == set_up)
    {
        printk("%s: ctlr req is NULL\n",__FUNCTION__);    
    }
    if(NULL == ctrl_req)
    {
         printk("%s: input req is NULL\n",__FUNCTION__);    
    }
    set_up->bRequestType = USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
	set_up->bRequest = req_cntxt->request;
	set_up->wValue = cpu_to_le16p(&req_cntxt->value);
	set_up->wIndex = cpu_to_le16p(&req_cntxt->index);
	set_up->wLength = cpu_to_le16p((unsigned short *)&req_cntxt->len);
    
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
    struct usb_ctrlrequest*	set_up = (struct usb_ctrlrequest*)request;
	usb_fill_control_urb((struct urb*)urb,
    					usbdev,
    					pipe,
    					(unsigned char *)set_up,	//Need to be confirmed for this var
    					buffer,
    					len,
    					tdsp_usb_callback,
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
				    usbdev,
    				pipe,
    				buffer,
    				len,
    				tdsp_usb_callback,
    				context,
    				interval);
    ret = usb_submit_urb((struct urb*)urb, GFP_ATOMIC);
	return ret;
}    
int tdsp_usb_bulkin(void * urb,
                         void * usbdev,
                         unsigned char ep,
                         void * buffer,
                         int len,
                         void* context)
{
    int ret;
    int pipe;
    struct urb* u;
    pipe = usb_rcvbulkpipe((struct usb_device *)usbdev,ep);
	usb_fill_bulk_urb((struct urb*)urb,
				        usbdev,
				        pipe,
				        buffer,
				        len,
				        tdsp_usb_callback,
				        context);
    u = (struct urb*)urb;	
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
				        tdsp_usb_callback,
				        context);
    ret = usb_submit_urb((struct urb*)urb, GFP_ATOMIC);
	return ret;
} 



