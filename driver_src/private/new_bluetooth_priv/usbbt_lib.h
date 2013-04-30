
#ifndef _USB_BT_LIB_H_
#define _USB_BT_LIB_H_

#pragma pack(1)

typedef struct _TDSP_SK_BUFF_HEAD
{
    void * buffhead;
}TDSP_SK_BUFF_HEAD, *PTDSP_SK_BUFF_HEAD;

typedef struct _TDSP_SK_BUFF
{
    void * skbuff;
}TDSP_SK_BUFF, *PTDSP_SK_BUFF;

typedef struct _TDSP_WORKQUEUE
{
    void * workqueue;
}TDSP_WORKQUEUE, *PTDSP_WORKQUEUE;

typedef struct _TDSP_WORK
{
    void * work;
}TDSP_WORK, *PTDSP_WORK;

typedef struct __TDSP_SPINLOCK
{
   void * lock;
}TDSP_SPINLOCK,*PTDSP_SPINLOCK;

typedef struct _TDSP_EVENT
{
    void* event;
}TDSP_EVENT,*PTDSP_EVENT;

typedef struct _TDSP_TASKLET
{
    void* tasklet;
}TDSP_TASKLET,*PTDSP_TASKLET;

typedef struct _TDSP_TIMER
{
    void* timer;
}TDSP_TIMER,*PTDSP_TIMER;

typedef struct _TDSP_USB_CTRLREQ
{
   unsigned char  request;
   unsigned short value;
   unsigned short index;
   unsigned int   len;
}TDSP_USB_CTRLREQ,*PTDSP_USB_CTRLREQ;


typedef struct _SYS_CALL 
{
	/* begin for spinlock related syscalls */
	void (*spin_lock_init)(PTDSP_SPINLOCK lock);
	void (*spin_lock)(PTDSP_SPINLOCK lock);
	void (*spin_lock_bh)(PTDSP_SPINLOCK lock);
	void (*spin_lock_irqsave)(PTDSP_SPINLOCK lock,unsigned long *flags);
	void (*spin_unlock)(PTDSP_SPINLOCK lock);
	void (*spin_unlock_bh)(PTDSP_SPINLOCK lock);
	void (*spin_unlock_irqrestore)(PTDSP_SPINLOCK lock,unsigned long *flags);
	void (*spin_lock_kill)(PTDSP_SPINLOCK lock);

	/* begin for memory related syscalls */
	void* (*memory_alloc)(unsigned int required_size);
	void* (*memory_alloc_atomic)(unsigned int required_size);
	void (*memory_free)(void* p);
	void (*memory_set)(void* p, unsigned char value, unsigned int len);
	void* (*memory_copy)(void* p1,const void* p2,unsigned int len);
	int (*memory_cmp)(const void* dest,const void* src,unsigned long len);
	void* (*memory_vmalloc)(unsigned long required_size);
	void (*memory_vfree)(void *p);
	void* (*memory_move)(void *dest, void *src, unsigned long size);
	void* (*zero_memory)(void *dest,unsigned long size);
   
	/* begin for event related syscalls */
	void (*event_init)(PTDSP_EVENT event);
	void (*event_set)(PTDSP_EVENT event);
	long  (*event_wait)(PTDSP_EVENT event,long timeout);
	void  (*event_kill)(PTDSP_EVENT event);
	void (*event_flag_set)(PTDSP_EVENT event, unsigned long flag);

	/* begin for timer related syscalls */
	void (*timer_init)(PTDSP_TIMER timer,void (*func)(unsigned long), unsigned long para);
	void (*timer_start)(PTDSP_TIMER t, unsigned int sec);
	void (*timer_reset)(PTDSP_TIMER t, unsigned int timeout);
	void (*timer_stop)(PTDSP_TIMER timer);
	void (*timer_stop_sync)(PTDSP_TIMER timer);
	void (*timer_kill)(PTDSP_TIMER timer);
	unsigned long (*ms_to_ticks)(unsigned long ms);
	unsigned long (*get_ticks)(void);
	void (*usdelay)(unsigned long usecs);
	void (*msdelay)(unsigned int msecs);
	
	/* begin for tasklet related syscalls */
	void (*tasklet_init)(PTDSP_TASKLET t,void (*func)(unsigned long), unsigned long data);
	void (*tasklet_schedule)(PTDSP_TASKLET t);
	void (*tasklet_enable)(PTDSP_TASKLET t);
	void (*tasklet_disable)(PTDSP_TASKLET t);
	void (*tasklet_kill)(PTDSP_TASKLET t);

	/* begin for usb related syscalls */
	void* (*usb_urb_alloc)(void);
	void (*usb_urb_free)(void * urb);
	int (*usb_urb_unlink)(void * urb);
	int (*usb_get_status)(void * urb);
	void* (*usb_get_context)(void * urb);
	int (*usb_get_len)(void* urb);
	void (*usb_urb_kill)(void * urb);

	void * (*usb_alloc_ctrlreq)(void);
	void (*usb_set_ctrlreq)(void * ctrl_req, PTDSP_USB_CTRLREQ req);
	void (*usb_free_ctrlreq)(void *);
	
	int (*usb_ctrlreq)(void * urb,
					void * usbdev,
					unsigned char ep,
					void * buffer,
					int len,
					void* context,
					void* request);

	int (*usb_rcvint)(void * urb,
					void * usbdev,
					unsigned char ep,
					void * buffer,
					int len,
					void* context,
					int interval);
	int (*usb_bulkin)(void * urb,
					void * usbdev,
					unsigned char ep,
					void * buffer,
					int len,
					void* context);
	int (*usb_bulkout)(void * urb,
					void * usbdev,
					unsigned char ep,
					void * buffer,
					int len,
					void* context);

	/* begin for bus related interface */
	void (*bus_set_fw_ver)(void *pIntf, unsigned long c8051_version, unsigned long dsp_version);
	unsigned char (*bus_get_hkey_flag)(void *pIntf);

	/* begin for print related syscalls */
	int   (*print)(const char *fmt, ...);

/*   ------------------ Addtional Functions --------------------   */
	int (*hci_notify_frame)(void *databuf);
	
	void* (*workthread_create)(char *threadname);
	void (*delaywork_init)(PTDSP_WORK w, void (*func)(unsigned long), unsigned long para);
	void (*work_init)(PTDSP_WORK w, void (*func)(unsigned long), unsigned long para); 
	int (*queue_delayedwork)(PTDSP_WORKQUEUE queue, PTDSP_WORK work, unsigned long delay);
	void (*cancel_delayedwork)(PTDSP_WORKQUEUE queue, PTDSP_WORK work);
	int (*queue_work)(PTDSP_WORKQUEUE queue, PTDSP_WORK work);
	void (*destory_workqueue)(PTDSP_WORKQUEUE queue);
	void (*delaywork_release)(PTDSP_WORKQUEUE queue, PTDSP_WORK work);
	void (*work_release)(PTDSP_WORK work);

	void (*skb_queue_init)(PTDSP_SK_BUFF_HEAD skb_head);
	void (*skb_queue_release)(PTDSP_SK_BUFF_HEAD skb_head);
	unsigned int (*skb_get_queue_len)(PTDSP_SK_BUFF_HEAD skb_head);
	void (*skb_put_queue_tail)(PTDSP_SK_BUFF_HEAD skb_head, PTDSP_SK_BUFF newsk);
	int (*skb_is_queue_empty)(PTDSP_SK_BUFF_HEAD skb_head);
	void* (*skb_dequeue_skb)(PTDSP_SK_BUFF_HEAD skb_head);
	void* (*skb_alloc_atomic)(unsigned long size);
	void (*skb_kfree)(PTDSP_SK_BUFF newsk);
	void* (*put_to_skb)(PTDSP_SK_BUFF newsk, unsigned long size);
	unsigned int (*skb_get_len)(PTDSP_SK_BUFF newsk);
	void* (*skb_get_dataptr)(PTDSP_SK_BUFF newsk);
	void* (*bt_skb_alloc_atomic)(unsigned long size);
	void (*skb_set_dev)(PTDSP_SK_BUFF newsk, void* dev);
	void (*skb_set_btpkt_type)(PTDSP_SK_BUFF newsk, unsigned long type);
	
	unsigned int (*usb_get_sndbulkpipe)(void* hdev, unsigned int ep);
	int (*usb_submit_bulkouturb)(void* purb, void* udev, unsigned int pipe, void* buffer, int buffer_len, void* context);
	int (*usb_ctrl_msg_b)(void *dev, int requesttype, int request, int value, int index, char *buff, int size, int timeout, unsigned int direct);
	unsigned char (*usb_get_reqtype)(unsigned int direct);
	int (*usb_status_nodev)(int status);
	int (*usb_status_inval)(int status);
	int (*usb_status_shutdown)(int status);
	int (*usb_status_proto)(int status);
	int (*usb_status_connreset)(int status);
	int (*usb_status_noent)(int status);

	void* (*openfile)(const char* filename);
	void (*closefile)(void* file_p);
	int (*open_success)(void* file_p);
	unsigned long (*readfile)(void* file_p, void* buffer, unsigned long buff_len, unsigned long offset);

	unsigned int (*isin_atomic)(void);

	unsigned int (*get_combodrv)(void);
}SYS_CALL,*PSYS_CALL;

typedef struct _USB_INFO
{
	unsigned int EP_control;
	unsigned int EP_bulkout_ACL;
	unsigned int EP_bulkout_SCO;
	unsigned int EP_bulkin_Inquiry;
	unsigned int EP_bulkin_Rx;
	unsigned int EP_interrupt;
	
}USB_INFO,*PUSB_INFO;	

#pragma pack()

void b_syscall_set(volatile PSYS_CALL sc);
void* Bluetooth_Adap_Create(PUSB_INFO pusb_info, void *udev, void *intf);
void Bluetooth_Adap_Destory(void *pdevExt);
void Bluetooth_Adap_Set_BusInf(void * pdevExt, void * pBusInf);
void* Bluetooth_Adap_Get_BusInf(void * pdevExt);
int Bluetooth_Adap_Close_Dev(void *pdevExt);
int Bluetooth_Adap_Open_Dev(void *pdevExt);
void Bluetooth_Adap_Set_hdev(void *pdevExt, void *hdev);
void* Bluetooth_Adap_Get_hdev(void *pdevExt);
void* Bluetooth_Adap_Get_uintf(void *pdevExt);
void Bluetooth_Adap_Resume(void *pdevExt);
void Bluetooth_Adap_Suspend(void *pdevExt);
int Bluetooth_Adap_IsOpen(void *pdevExt);
void Bluetooth_Adap_SetOpen(void *pdevExt);
void Bluetooth_Adap_Init(void *pdevExt);
void Bluetooth_Adap_Set_FirmVer(void *pdevExt);
void Bluetooth_Adap_Exit_Lmp_Dump(void *pdevExt);
void Bluetooth_Adap_ClearOpen(void *pdevExt);
unsigned int Bluetooth_Adap_IsInD3(void *pdevExt);
void Bluetooth_Adap_SendCmd(void *pdevExt, void *skb);
void Bluetooth_Adap_SendData(void *pdevExt, void *skb);
void Bluetooth_Adap_DiagSys(void *pdevExt);
int Bluetooth_Adap_WriteUsbReg(void *pdevExt, unsigned long addr, unsigned long *plen, unsigned long size);
int Bluetooth_Adap_ReadUsbReg(void *pdevExt, unsigned long addr, unsigned char *out, unsigned long len);
int Bluetooth_Adap_ReadDspReg(void *pdevExt, unsigned long offset, unsigned long len, unsigned char *out);
int Bluetooth_Adap_WriteDspReg(void *pdevExt, unsigned long offset, unsigned long len, unsigned char *out);
void Bluetooth_Adap_Read_Compelet(void *purb);
void Bluetooth_Adap_Inq_Compelet(void *purb);
void Bluetooth_Adap_Write_Compelet(void *purb);
void Bluetooth_Adap_Int_Compelet(void *purb);
void Bluetooth_Adap_ctrl_Compelet(void *purb);
void* Bluetooth_Adap_Get_Dumpdata(void *pdevExt);
unsigned char Bluetooth_Adap_GetWorkMod(void *pdevExt);
void* Bluetooth_Adap_GetDev(void *pdevExt);
//^^BLUETOOTH_DEBUG^^
extern unsigned int dbg_zone;
extern unsigned int dbg_level;
//vvBLUETOOTH_DEBUGvv

#endif

