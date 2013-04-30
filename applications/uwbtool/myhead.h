#ifndef MYHEAD_H
  #define MYHEAD_H
/************************************************************************** 
 * FILENAME:     callback.h          
 * CREATE DATE:  2008/09/05 
 * PURPOSE:      the headfile of callback.c 
 *                ... 
 *  
 * AUTHORS:      3dsp <ritow.qi@3dsp.com.cn> 
 * 
 * NOTES:        description of constraints when using functions of this file 
 * 
 **************************************************************************/

#define TRUE 1
#define FALSE 0

#define TDSP_SERIAL_NO_BT        1
#define TDSP_SERIAL_NO_WLAN      2
#define TDSP_SERIAL_NO_COEXIST      (TDSP_SERIAL_NO_BT + TDSP_SERIAL_NO_WLAN)
#define TDSP_SERIAL_NO_NONE      0xffff
#define TDSP_SERIAL_NO_NOCARD     0x10000

#define TDSP_BUS_DEVICE_NAME	"//dev//tdspusbbus"

#define TDSPSHELLFILEFORBT "/usr/local/3DSP/usb/btusbcfg.sh"
#define TDSPWBCONFIGFILE "/usr/local/3DSP/usb/wbusb.conf"

/*the value of g_state*/

#define BLUETOOTH_ENABLE_WLAN_DISABLE    0
#define BLUETOOTH_DISABLE_WLAN_ENABLE    1
#define BLUETOOTH_ENABLE_WLAN_ENABLE     2
#define BLUETOOTH_DISABLE_WLAN_DISABLE   3
#define BLUETOOTH_WLAN_ERROR   0xff

/*the current menu mode !*/
#define MODE_NULL      0
#define MODE_BLUETOOTH 1
#define MODE_WLAN      2
#define MODE_COEXIST     3
#define MODE_UNPLUG    4

/*for the bus power state: g_buspowerstate*/
#define	PowerOff 			0
#define	PowerOn			1
#define	PowerOff2On		2
#define	PowerOn2Off 		3
#define	PowerUnKnown		4

#define TDSP_MAGIC   0x6b
#define TDSP_PLUGIN			_IO(TDSP_MAGIC,0x0)
#define TDSP_UNPLUG			_IO(TDSP_MAGIC,0x1)
#define TDSP_EJECT_HARDWARE			_IO(TDSP_MAGIC,0x2)
#define TDSP_DONT_DISPLAY_IN_UI_DEVICE	_IO(TDSP_MAGIC,0x3)
#define TDSP_DISPLAY_IN_USE		_IO(TDSP_MAGIC,0x4)
#define TDSP_RELOAD_BT_CODE			_IO(TDSP_MAGIC,0x5)
#define TDSP_RELOAD_WLAN_CODE		_IO(TDSP_MAGIC,0x6)
#define TDSP_RELOAD_GPS_CODE			_IO(TDSP_MAGIC,0x7)
#define TDSP_RELOAD_CODE_FROM_FILE		_IO(TDSP_MAGIC,0xe)
#define TDSP_RELOAD_COMBO_CODE		_IO(TDSP_MAGIC,0xf)
#define TDSP_SET_INIT_FLAG			_IO(TDSP_MAGIC,0x10)
#define TDSP_READ_REGISTER       	_IO(TDSP_MAGIC,0x11)
#define TDSP_WRITE_REGISTER 		_IO(TDSP_MAGIC,0x12)
#define TDSP_WRITE_EEPROM			_IO(TDSP_MAGIC,0x1d)
#define TDSP_QUERY_AUTO_ENUM_FLAG		_IO(TDSP_MAGIC,0x1e)
#define TDSP_QUERY_RF_TYPE			_IO(TDSP_MAGIC,0x1f)
#define TDSP_QUERY_DEVICE_STATE		_IO(TDSP_MAGIC,0x20)
#define TDSP_QUERY_DEVICE_VERSION		_IO(TDSP_MAGIC,0x21)
#define TDSP_QUERY_BUS_VERSION		_IO(TDSP_MAGIC,0x22)
#define TDSP_QUERY_FIRMWARE_VERSION		_IO(TDSP_MAGIC,0x23)
#define TDSP_QUERY_DSP_START_FLAG		_IO(TDSP_MAGIC,0x24)
#define TDSP_QUERY_DEVICE_COMNUM		_IO(TDSP_MAGIC,0x25)
#define TDSP_GET_CHIP_ID			_IO(TDSP_MAGIC,0x26)
#define TDSP_GET_SUB_SYSTEM_ID_INFO		_IO(TDSP_MAGIC,0x27)
#define TDSP_QUERY_DEVICE_COMPANYID		_IO(TDSP_MAGIC,0x28)
#define TDSP_READ_ENUM_MODE			_IO(TDSP_MAGIC,0x29)
#define TDSP_WRITE_ENUM_MODE			_IO(TDSP_MAGIC,0x2a)
#define TDSP_QUERY_RESET_FLAG		_IO(TDSP_MAGIC,0x2b)
#define TDSP_POWER_ON			_IO(TDSP_MAGIC,0x2c)
#define TDSP_POWER_OFF		_IO(TDSP_MAGIC,0x2d)
#define TDSP_QUERY_BUS_POWER_STATE		_IO(TDSP_MAGIC,0x2e)
#define TDSP_SET_DEVICE_STATE		_IO(TDSP_MAGIC,0x2f)
#define TDSP_SET_HOT_FLAG		_IO(TDSP_MAGIC,0x30)
#define TDSP_QUERY_8051_VERSION		_IO(TDSP_MAGIC,0x31)


#define TDSPWLANID "Tdspbus\\TdspWlan\0"        // WLAN
#define TDSPBTID "Tdspbus\\TdspBluetooth\0"     // Bluetooth

//
//  Data structure used in PlugIn and UnPlug ioctls
//

typedef struct _BUSENUM_PLUGIN_HARDWARE
{
  //
  // sizeof (struct _BUSENUM_HARDWARE)
  //
  unsigned long Size;

  //
  // Unique serial number of the device to be enumerated.
  // Enumeration will be failed if another device on the 
  // bus has the same serail number.
  //

  unsigned long SerialNo;

  //
  // An array of (zero terminated wide character strings). The array itself
  //  also null terminated (ie, MULTI_SZ)
  //

  char HardwareIDs[];

} BUSENUM_PLUGIN_HARDWARE, *PBUSENUM_PLUGIN_HARDWARE;

typedef struct _BUSENUM_UNPLUG_HARDWARE
{
  //
  // sizeof (struct _REMOVE_HARDWARE)
  //

  unsigned long Size;

  //
  // Serial number of the device to be plugged out    
  //

  unsigned long SerialNo;

  unsigned long Reserved[2];

} BUSENUM_UNPLUG_HARDWARE, *PBUSENUM_UNPLUG_HARDWARE;

typedef struct _BUSENUM_EJECT_HARDWARE
{
  //
  // sizeof (struct _EJECT_HARDWARE)
  //

  unsigned long Size;

  //
  // Serial number of the device to be ejected
  //

  unsigned long SerialNo;

  unsigned long Reserved[2];

} BUSENUM_EJECT_HARDWARE, *PBUSENUM_EJECT_HARDWARE;

typedef struct _LOAD_EEPROM_STRUC
{
  unsigned long offset;
  unsigned long length;
  char buf[];
} LOAD_EEPROM_STRUC, *PLOAD_EEPROM_STRUC;

typedef struct _RW_REGISTER_STRUC
{
  unsigned long offset;
  unsigned long length;
  unsigned char buf[];
} RW_REGISTER_STRUC, *PRW_REGISTER_STRUC;

typedef enum _DEVICE_PNP_STATE
{
  NotStarted = 0,               // Not started yet
  Started,                      // Device has received the START_DEVICE IRP
  StopPending,                  // Device has received the QUERY_STOP IRP
  Stopped,                      // Device has received the STOP_DEVICE IRP
  RemovePending,                // Device has received the QUERY_REMOVE IRP
  SurpriseRemovePending,        // Device has received the SURPRISE_REMOVE IRP
  Deleted,                      // Device has received the REMOVE_DEVICE IRP
  UnKnown                       // Unknown state
} DEVICE_PNP_STATE;

typedef struct _stdspversion
{
  unsigned int pos[5];
  unsigned char btver[64];
  unsigned char wlanver[64];
  unsigned char busver[64];
  unsigned long firmwarever;
  unsigned long ver8051;
} TDSPVERSION, *PTDSPVERSION;

struct globalargs_t
{
  int iquery;                   /* -q option */
  int iremove;                  /* -r option */
  char *chadd;                  /* -a option */
  char *chwrite;                /* -w option */
  char *chget;
  char *chput;
  int iversion;                 /* -v option */
  int randomized;               /* --randomize option */
};

extern int ver_diag_opened;
extern DEVICE_PNP_STATE g_dsppnpstate;
extern int g_isclickinginprogress;
extern int g_buspowerstate;
int g_downfromfile;

void init_tdsp_otherenv(char *shellfile);
int tdsp_query_power_state();
int tdsp_query_device();
int tdsp_query_reset_flag();
int tdsp_download_dsp_code(unsigned long serialno);
int tdsp_add_device(char *devicename, unsigned long namelen, unsigned long serialno);
int tdsp_remove_device(unsigned long serialno);
int unplugdevices();
void tdsp_power_on();
void tdsp_power_off();
void tdsp_refresh_version();
int get_value_from_conffile(FILE * fp, char * keystring, char * valstring);
void adjust_func_state();
void wait_device_unplug_stable();
int write_enum_mode(unsigned long mode);
int tdsp_download_dsp_code_from_file(unsigned long serialno);
int tdsp_is_download_dspcode_ok();
int tdsp_query_pnp_state(unsigned long mode);
int tdsp_set_init_flag(unsigned long mode);
void update_ver_string();
void logging(char * msg, int line);

char m_chagdesc[255];
char m_chgonlydesc[255];
char m_chbt1desc[255];
char m_chbt2desc[255];
char m_chsystemtraytips[255];
char m_chcompanyid[255];
int m_nwlancfgexitnormally;
int m_nshowwarning;
char m_chcoexistwarning[255];
char m_chversion[255];
char m_chreleasedate[255];
char m_chproductid[255];
int m_nHotKeyFlag;

#ifdef DEBUG
#define debug(...) \
do \
{ \
    printf("%s %s(Line %d): ",__FILE__,__FUNCTION__,__LINE__); \
    printf(__VA_ARGS__); \
}while(0)
#else
#define debug(x...)
#endif

#define WTRGLEN 8

#endif
