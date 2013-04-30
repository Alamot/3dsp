#ifndef __TDSP_USB_H__
#define __TDSP_USB_H__

#include <linux/usb.h>
#include "usbbt_lib.h"

/* Bluetooth USB interface number */
#define	TDSP_BT_INTERFACE_NUM	1
#define	EP_COUNT				5

#define	INDEX_EP_CTRL			0
#define	INDEX_EP_BULK_IN_DATA	5
#define	INDEX_EP_BULK_OUT_ACL	6
#define	INDEX_EP_BULK_IN_INQ	7
#define	INDEX_EP_BULK_OUT_SCO	8
#define	INDEX_EP_INTR			10

#define VENDOR_CMD_IN					0x01
#define VENDOR_CMD_OUT					0x00

void * tdsp_usb_urb_alloc(void);
void tdsp_usb_urb_free(void * urb);
int tdsp_usb_urb_unlink(void * urb);
void tdsp_usb_urb_kill(void * urb);
int tdsp_usb_get_status(void * urb);
void* tdsp_usb_get_context(void * urb);
int tdsp_usb_get_len(void* urb);
void* tdsp_usb_alloc_ctrlreq(void);
void tdsp_usb_set_ctrlreq(void * ctrl_req,PTDSP_USB_CTRLREQ req);
void tdsp_usb_free_ctrlreq(void * set_up);
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
unsigned int tdsp_usb_get_sndbulkpipe(void* hdev, unsigned int ep);
int tdsp_usb_submit_bulkouturb(void* purb, void* udev, unsigned int pipe, void* buffer, int buffer_len, void* context);

int tdsp_usb_status_nodev(int status);
int tdsp_usb_status_inval(int status);
int tdsp_usb_status_shutdown(int status);
int tdsp_usb_status_proto(int status);
int tdsp_usb_status_connreset(int status);
int tdsp_usb_status_noent(int status);

int tdsp_usb_ctrl_msg_b(void *dev, int requesttype, int request, int value, int index, char *buff, int size, int timeout, unsigned int direct);
unsigned char tdsp_usb_get_reqtype(unsigned int direct);

#endif

