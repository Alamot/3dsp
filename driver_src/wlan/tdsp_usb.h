#ifndef __TDSP_USB_H__
#define __TDSP_USB_H__

#include <linux/usb.h>
#include "usbwlan_lib.h"

void * tdsp_usb_alloc_urb(void);
void tdsp_usb_free_urb(void * urb);
void tdsp_usb_unlink_urb(void * urb);
void tdsp_usb_kill_urb(void * urb);
int tdsp_usb_get_status(void * urb);
void* tdsp_usb_get_context(void * urb);
int tdsp_usb_is_srmv(int status);
int tdsp_usb_get_len(void* urb);
void* tdsp_usb_alloc_ctrlreq(void);
void tdsp_usb_set_ctrlreq(void * ctrl_req,void * req);
void tdsp_usb_free_ctrlreq(void * set_up);
int tdsp_usb_ctrl_msg(void * usbdev,
                             unsigned char ep,
                             unsigned char type,
                             void * buffer,
                             int len,
 						     void* request);

int tdsp_usb_ctrl_req(void * urb,
                          void * usbdev,
                          unsigned char ep,
                          void * buffer,
                          int len,
                          void* context,
                          void* request);

int tdsp_usb_rcvint(void * urb,
                     void * usbdev,
                     unsigned char ep,
                     void * buffer,
                     int len,
                     void* context,
                     int interval);

int tdsp_usb_bulkin(void * urb,
                             void * usbdev,
                             unsigned char ep,
                             void * buffer,
                             int len,
                             void* context);

int tdsp_usb_bulkout(void * urb,
                             void * usbdev,
                             unsigned char ep,
                             void * buffer,
                             int len,
                             void* context);

#endif

