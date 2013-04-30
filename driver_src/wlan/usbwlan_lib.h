
#ifndef _USB_WLAN_LIB_H_
#define _USB_WLAN_LIB_H_

//#include "tdsp_basetypes.h"

#pragma pack(1)

#define USB_WRITE_OP			 	((unsigned char)(0x00))
#define USB_READ_OP			 	    ((unsigned char)(0x01))

#define BIT_EXT_INDEX     0x03
#define BIT_GROUP_KEY     0x04
#define BIT_SET_TX_KEY    0x08
#define BIT_RX_SEQ_VALID  0x10

#define MAC_ADDR_IS_BCAST(addr) (addr[0] == 0xff && addr[1] == 0xff && addr[2] == 0xff && \
		addr[3] == 0xff && addr[4] == 0xff && addr[5] == 0xff) 

#define WPA_VERSION_NONE 0
#define WPA_VERSION_WPA  1
#define WPA_VERSION_WPA2 2
#if 0
#define ALG_NONE         0
#define ALG_OPEN         1
#define ALG_SHARED_KEY   2
#endif

#define KEY_MNG_NONE          0
#define KEY_MNG_1X            1
#define KEY_MNG_PSK           2

/////////////auth alg//////////////////////////////////////
#define AUTH_ALG_OPEN           0
#define AUTH_ALG_SHARED_KEY     1
#define AUTH_ALG_NONE           2

/////////////auth mode//////////////////////////////////////
#define AUTH_MODE_OPEN                 0
#define AUTH_MODE_SHARED_KEY     1
#define AUTH_MODE_WPA                  2 
#define AUTH_MODE_WPA_PSK          3 
#define AUTH_MODE_WPA2                4 
#define AUTH_MODE_WPA2_PSK        5 
///////////wep mode///////////////////////////////////////////
#define WEP_MODE_WEP                   0
#define WEP_MODE_TKIP                  1
#define WEP_MODE_AES                   2
#define WEP_MODE_NONE                  3

#define WLAN_MACMODE_NONE	    0
#define WLAN_MACMODE_IBSS_STA	1
#define WLAN_MACMODE_ESS_STA	2
#define WLAN_MACMODE_ESS_AP	    3

#if 0
#define CIPHER_NONE              0
#define CIPHER_WEP               1
#define CIPHER_TKIP              2
#define CIPHER_CCMP              3
#endif

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


typedef struct _TDSP_WORKLET
{
    void * worklet;
}TDSP_WORKLET, *PTDSP_WORKLET;

typedef struct _TDSP_TIMER
{
    void* timer;
}TDSP_TIMER,*PTDSP_TIMER;


typedef struct _NET_STAT
{
    void* stat;
}NET_STAT,PNET_STAT;

typedef struct _TDSP_USB_CTRLREQ
{
   unsigned char  request;
   unsigned short value;
   unsigned short index;
   unsigned int   len;
}TDSP_USB_CTRLREQ,*PTDSP_USB_CTRLREQ;


typedef struct _SYS_CALL 
{
    //begin for spinlock related syscalls
    void (*spin_lock_init)(PTDSP_SPINLOCK lock);
    void (*spin_lock)(PTDSP_SPINLOCK lock);
    void (*spin_lock_bh)(PTDSP_SPINLOCK lock);
    void (*spin_lock_irqsave)(PTDSP_SPINLOCK lock,unsigned long *flags);
    void (*spin_unlock)(PTDSP_SPINLOCK lock);
    void (*spin_unlock_bh)(PTDSP_SPINLOCK lock);
    void (*spin_unlock_irqrestore)(PTDSP_SPINLOCK lock,unsigned long flags);
    void (*spin_lock_kill)(PTDSP_SPINLOCK lock);

    //begin for memory related syscalls
    void* (*memory_alloc)(unsigned int required_size);
    void (*memory_free)(void* p);
    void (*memory_set)(void* p, unsigned char value, unsigned int len);
    void (*memory_copy)(void* p1,const void* p2,unsigned int len);
    void* (*memory_vmalloc)(unsigned long required_size);
	void (*memory_vfree)(void *p);

    int (*memory_cmp)(const void* dest,const void* src,unsigned int len);
   
    //begin for event related syscalls
    void (*event_init)(PTDSP_EVENT event);
    void (*event_set)(PTDSP_EVENT event);
    void (*event_reset)(PTDSP_EVENT event);
    int  (*event_wait)(PTDSP_EVENT event,unsigned int timeout);
    void  (*event_kill)(PTDSP_EVENT event);

    //begin for tasklet related syscalls
    void (*tasklet_init)(PTDSP_TASKLET t,void (*func)(unsigned long), unsigned long data);
    void (*tasklet_schedule)(PTDSP_TASKLET t);
    void (*tasklet_enable)(PTDSP_TASKLET t);
    void (*tasklet_disable)(PTDSP_TASKLET t);
    void (*tasklet_kill)(PTDSP_TASKLET t);

    //begin for worklet related syscalls
    void (*worklet_init)(PTDSP_WORKLET w, void (*func)(unsigned long), unsigned long para);
    void (*worklet_kill)(PTDSP_WORKLET w);
    void (*worklet_schedule)(PTDSP_WORKLET w);
    unsigned char (*worklet_running)(PTDSP_WORKLET w);

    //begin for timer related syscalls
    void (*timer_init)(PTDSP_TIMER timer,void (*func)(unsigned long), unsigned long para);

    void (*timer_stop)(PTDSP_TIMER timer);

    void (*timer_start)(PTDSP_TIMER t, unsigned int sec);

    void (*timer_reset)(PTDSP_TIMER t, unsigned int timeout);

    void (*timer_kill)(PTDSP_TIMER timer);

    //begin for time related syscalls
    void (*sleep)(unsigned int delay_ms);
    unsigned long long (*ms_to_ticks)(unsigned int ms);
   // unsigned int (*get_current_time)(void);
    unsigned char (*get_random_byte)(void);
    unsigned int  (*get_random_dword)(void);
    unsigned char  (*time_downout_duration)(void);
    //begin for usb related syscalls
    void* (*urb_alloc)(void);
    void (*urb_free)(void * urb);
    void (*urb_unlink)(void * urb);
    void (*urb_kill)(void * urb);
    int (*urb_getstatus)(void * urb);
    void* (*urb_getcntxt)(void * urb);
    int (*usb_issrmv)(int status);
    int (*urb_getlen)(void* urb);
    void * (*usb_alloc_ctrlreq)(void);
    void (*usb_set_ctrlreq)(void * ctrl_req,void* req);
    void (*usb_free_ctrlreq)(void *);
    int (*usb_ctrlmsg)(void * usbdev,
                         unsigned char ep,
                         unsigned char type,
                         void * buffer,
                         int len,
					     void* reqeust);

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

    void* (*skb_alloc)(unsigned int buf_len,
                              void** para);
    
    void (*skb_sbmt)(void* net_dev, 
                            void* pkt_buff,
                            unsigned int buff_len);
    void (*skb_free)(void* pkt_buff);
    int (*netq_ifstop)(void* net_dev);
    void (*netq_start)(void* net_dev);
    void (*netq_stop)(void* net_dev);

    void  (*iw_send_bssid_to_up)(void *dev, unsigned char *bssid);
    int (*wlan_netdev_setup)(void* adapt);
    void (*wlan_netdev_destroy)(void* adapt);

    //begin for bus related interface
    void (*bus_set_fw_ver)(void *pIntf, unsigned long c8051_version, unsigned long dsp_version);
    unsigned char (*bus_get_hkey_flag)(void *pIntf);
    unsigned char (*bus_get_antenna_flag)(void *pIntf);
    //begin for print related syscalls
    int   (*print)(const char *fmt, ...);
    
	void* (*openfile)(const char* filename);
	void (*closefile)(void* file_p);
	int (*open_success)(void* file_p);
	unsigned long (*readfile)(void* file_p, void* buffer, unsigned long buff_len, unsigned long offset);

}SYS_CALL,*PSYS_CALL;




typedef struct _USB_INFO
{
	char EP_control;		//ep info
	char EP_bulkout_HEAD;
	char EP_bulkout_BODY;
	char EP_bulkin;
	char EP_interrupt;
	void	*usb_dev;	 /* this usb_device */
	void	*usb_intf;	 /* this interface */
	
}USB_INFO,*PUSB_INFO;	

typedef struct _CAPABILITY
{
	unsigned short          ess             : 1;
	unsigned short         ibss             : 1;
	unsigned short         cf_pollable      : 1;
	unsigned short          cf_pollreq      : 1;
	unsigned short          privacy         : 1;
	unsigned short          short_preamble  : 1;
	unsigned short            pbcc          : 1;
	unsigned short          channel_agility : 1;
	unsigned short          reserved1       : 2;
	unsigned short          short_slot_time : 1;
    unsigned short          reserved2       : 2;
	unsigned short          dsss_ofdm	    : 1;
	unsigned short          reserved3       : 2;
} CAPABILITY_T, *PCAPABILITY_T;

typedef struct _NOFIXED_IE
{
   unsigned char id;
   unsigned char len;
   unsigned char data[360+4*10+4*10];
}NOFIXED_IE,*P_NOFIXED_IE;


typedef struct _STAT_DATA
{

	unsigned int tx_good;
	unsigned int tx_err;
	unsigned int tx_payload;
	unsigned int rx_good;
	unsigned int rx_err;
	unsigned int rx_payload;
	unsigned int rx_mc;
	unsigned int rx_dropped;
}STAT_DATA,*PSTAT_DATA;

#define NO_FIXED_IE_LEN       360+4*10+4*10
//#define PAIR_CIPHER_SUITE_NUM       10
//#define AUTH_MGT_SUITE_NUM          10
//#define MAX_BEACON_RSP_INFO_LENGTH  (360+4*PAIR_CIPHER_SUITE_NUM+4*AUTH_MGT_SUITE_NUM) 
typedef struct _SCAN_RES
{

    unsigned char			bssid[6];
	unsigned char          	ssid[32+1];      /* first byte is length */
    unsigned char			channel;
    unsigned char           rssi;
    unsigned short	        beaconinterval;
    CAPABILITY_T            cap;
	unsigned char          	suprate[12+1]; /* first byte is length    */
    unsigned char           IEs[NO_FIXED_IE_LEN];
	unsigned char           is80211g;// Justin: 0625.  must use type enum NDIS_802_11_NETWORK_TYPE
	unsigned short	offset_wpa;
	unsigned short offset_wpa2;
    //NOFIXED_IE   wpa;
    
}SCAN_RES,*PSCAN_RES;

typedef struct _RTS_DATA
{
    char rts_disable;
    unsigned int rts_value;
}RTS_DATA,*PRTS_DATA;

typedef struct _FRAG_DATA
{
    char frag_disable;
    unsigned int frag_value;
}FRAG_DATA,*PFRAG_DATA;


typedef enum _AUTH_MODE
{
    AUTH_WPA_VERSION =0,
    AUTH_CIPHER_PAIRWISE,
    AUTH_CIPHER_GROUP,
    AUTH_KEY_MGMT,
    AUTH_80211_AUTH_ALG,
}AUTH_MODE;

#if 0
#define CIPHER_WEP            0
#define CIPHER_TKIP           1
#define CIPHER_CCMP           2
#define CIPHER_NONE           3
#endif

/*
 * DOT11 Type
 */
typedef enum
{
	IEEE802_11_A,
	IEEE802_11_B,
	IEEE802_11_G
}dot11_type_t;



typedef struct _RATE_DATA
{
    char fixed;
    unsigned int rate_value;
}RATE_DATA,*PRATE_DATA;



typedef struct _WEP_KEY
{
    unsigned char len;
    unsigned char key[16];
}WEP_KEY,*PWEP_KEY;

typedef struct _ENCODE_FLAG
{
    unsigned index:2;
    unsigned group_key:1;
    unsigned set_tx_key:1;
    unsigned rx_seq_valid:1;
   
}ENCODE_FLAG;


typedef struct _ADD_KEY
{
	unsigned char			KeyRSC[8];
 	unsigned char		    BSS_ID[6];
    unsigned char			is_group_key;
	unsigned char			is_tx_key;
 	unsigned char			keyRSC_valid;
	unsigned char			wep_mode;
 	unsigned char			KeyIndex;           
	unsigned char		    KeyLength;          // length of key in bytes
	unsigned char           KeyMaterial[32];     // variable length depending on above field
}ADD_KEY, *PADD_KEY;

#pragma pack()

void* Adap_Create(PUSB_INFO usbinfo);
int	Adap_Initialize(void * adapt);
void Adap_Halt( void* para );
void 			Adap_UnInitialize(void * adapt);
void 			Adap_Destroy(void * adapt);
unsigned char Adap_Tx_Available(void * adapt);
int Adap_Send(void* adapt, unsigned char* data, unsigned int data_len);
void Adap_Set_BusInf(void * adapt, void * pBusInf);
void* Adap_Get_BusInf(void * adapt);
extern void syscall_set(PSYS_CALL sc);
extern void tdsp_set_debug(void);
extern void Usb_Completion_Imp(void* pUrb);
extern void* Adap_Get_Mac_Addr(void* adapt);
extern void Adap_Set_Netdev(void* adapt,void* netdev);
extern void* Adap_Get_Netdev(void* adapt);
void* Adap_Get_Netstat(void* adapt);
void  Adap_Get_StatData(void* adapt,PSTAT_DATA data);
void Adap_Set_Netstat(void* adapt,void* netstat);
void Adap_Set_Removed(void * adapt ,unsigned char removed);
unsigned int Adap_Get_DotType(void * adapt);
unsigned char Adap_Get_Mode(void *  adapt);
int Adap_Set_Defult_Channel(void *  adapt, int channel);
unsigned char Adap_Get_Channel(void * adapt);
void Adap_Set_Mode(void * adapt,unsigned char mode);
unsigned char Adap_Get_Channel_Num(void * adapt);
int Adap_Start_Scan(void * adapt);
void Adap_Get_Scan_Result(void * adapt, unsigned int* count,PSCAN_RES* scan_res);
void Adap_Get_BSSID(void * adapt,unsigned char * bssid);
void Adap_Disassociate(void * adapt);
int Adap_Set_SSID(void * adapt, 
    unsigned char* p_ssid,
    unsigned char ssid_len);
void Adap_Get_SSID(void * adapt, 
    unsigned char * p_ssid, 
    unsigned char* ssid_len);
void Adap_Set_Rts(void * adapt,int value);
void Adap_Get_Rts(void * adapt, PRTS_DATA data);
void Adap_Set_Frag(void * adapt,int frag);
void Adap_Get_Frag(void * adapt, PFRAG_DATA data);
void Adap_Set_Rate(void * adapt, int rate, unsigned char fixed);
void Adap_Get_Rate(void * adapt, PRATE_DATA data);
unsigned char  Adap_Get_Rssi(void * adapt);
int Adap_Set_Auth(void * adpat,  unsigned char auth_mode, unsigned char value);
unsigned char Adap_Get_Auth_Alg(void * adapt);
unsigned char Adap_Get_Key_Mng(void * adapt);
void Adap_Disalbe_Encode(void * adapt, unsigned char index);
void Adap_Set_Encode(void * adapt, unsigned char index, 
    unsigned char* key, unsigned char len);
void Adap_Get_Encode(void * adapt, unsigned char index,PWEP_KEY wep);

int Adap_Remove_Key_Ext(void * adapt, 
    unsigned char index,unsigned char group);
int Adap_Add_Key_Ext(void * adapt, unsigned char index, 
    PADD_KEY add_key);
unsigned char Adap_Is_Encode_Enabled(void * adpat);
unsigned char Adap_Get_Encode_Mode(void * adapt, unsigned char group);
int Adap_Get_Encode_Key(void * adapt, unsigned char index,unsigned char* key);
void Adap_Set_Domain(void * adapt, unsigned char domain);
unsigned int Adap_Get_Domain(void * adapt);
unsigned int Adap_Get_Dsp_Version(void * adapt);
unsigned char Adap_Has_Joined(void * adapt);

extern unsigned char  Adap_Driver_isWork(void * adapt);

unsigned char Adap_Has_Connected(void * adapt);

unsigned char Adap_Is_Privacy_On(void * adapt);

void Adap_Suspend(void * adapt, int type);
void Adap_Resume(void * adapt, int type);
void Adap_Rest_Resume(void * adapt, int type);
unsigned char Adap_Use_WPA(void * adapt);
void  Adap_Print_USBReg(void * adapt);
void Tx_Print_Status(void* adapt);
#endif

