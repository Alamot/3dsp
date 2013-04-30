#ifndef MAIN_H
#define MAIN_H
/************************************************************************** 
 * FILENAME:     main.h          
 * CREATE DATE:  2009/12/24 
 * PURPOSE:      the headfile of main.c 
 *                ... 
 *  
 * AUTHORS:      3dsp <ritow.qi@3dsp.com.cn> 
 * 
 * NOTES:        description of constraints when using functions of this file 
 * 
 **************************************************************************/

#define TDSPUWBVERION "0.3.14"
#define UWBCONFFILE "/usr/local/3DSP/usb/wbusb.conf"
#define FIFO_FILE  "/tmp/TdspUsbMAFIFO"
#define TDSPPCIMA "tdspusbma"
#define SWITCH_NOTIFY_TIME 3000

#define TDSP_SERIAL_NO_BT        1
#define TDSP_SERIAL_NO_WLAN      2
#define TDSP_SERIAL_NO_COEXIST      (TDSP_SERIAL_NO_BT + TDSP_SERIAL_NO_WLAN)
#define TDSP_SERIAL_NO_NONE      0xffff
#define TDSP_SERIAL_NO_NOCARD    0x10000

//#define TDSP_BUS_DEVICE_NAME  "//dev//tdspbus"
#define TDSP_BUS_DEVICE_NAME	"//dev//tdspusbbus"

#define TDSPSHELLFILEFORBT "/usr/local/3DSP/usb/btusbcfg.sh"

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
#define MODE_COEXIST   3
#define MODE_UNPLUG    4

/*for tray icon*/
#define ICON_GREEN 0
#define ICON_RED 1
#define ICON_GRAY 2

/*for the bus power state: g_buspowerstate*/
#define	PowerOff 			0
#define	PowerOn				1
#define	PowerOff2On			2
#define	PowerOn2Off 		3
#define	PowerUnKnown		4

#define TDSP_MAGIC   0x6b
#define TDSP_PLUGIN					_IO(TDSP_MAGIC,0x0)
#define TDSP_UNPLUG					_IO(TDSP_MAGIC,0x1)
#define TDSP_EJECT_HARDWARE			_IO(TDSP_MAGIC,0x2)
#define TDSP_DONT_DISPLAY_IN_UI_DEVICE	_IO(TDSP_MAGIC,0x3)
#define TDSP_DISPLAY_IN_USE			_IO(TDSP_MAGIC,0x4)
#define TDSP_RELOAD_BT_CODE			_IO(TDSP_MAGIC,0x5)
#define TDSP_RELOAD_WLAN_CODE		_IO(TDSP_MAGIC,0x6)
#define TDSP_RELOAD_GPS_CODE		_IO(TDSP_MAGIC,0x7)
#define TDSP_RELOAD_CODE_FROM_FILE	_IO(TDSP_MAGIC,0xe)
#define TDSP_RELOAD_COMBO_CODE		_IO(TDSP_MAGIC,0xf)
#define TDSP_SET_INIT_FLAG			_IO(TDSP_MAGIC,0x10)
#define TDSP_QUERY_AUTO_ENUM_FLAG	_IO(TDSP_MAGIC,0x1e)
#define TDSP_QUERY_RF_TYPE			_IO(TDSP_MAGIC,0x1f)
#define TDSP_QUERY_DEVICE_STATE		_IO(TDSP_MAGIC,0x20)
#define TDSP_QUERY_DEVICE_VERSION	_IO(TDSP_MAGIC,0x21)
#define TDSP_QUERY_BUS_VERSION		_IO(TDSP_MAGIC,0x22)
#define TDSP_QUERY_FIRMWARE_VERSION	_IO(TDSP_MAGIC,0x23)
#define TDSP_QUERY_DSP_START_FLAG	_IO(TDSP_MAGIC,0x24)
#define TDSP_QUERY_DEVICE_COMNUM	_IO(TDSP_MAGIC,0x25)
#define TDSP_GET_CHIP_ID			_IO(TDSP_MAGIC,0x26)
#define TDSP_GET_SUB_SYSTEM_ID_INFO	_IO(TDSP_MAGIC,0x27)
#define TDSP_QUERY_DEVICE_COMPANYID	_IO(TDSP_MAGIC,0x28)
#define TDSP_READ_ENUM_MODE			_IO(TDSP_MAGIC,0x29)
#define TDSP_WRITE_ENUM_MODE		_IO(TDSP_MAGIC,0x2a)
#define TDSP_QUERY_RESET_FLAG		_IO(TDSP_MAGIC,0x2b)
#define TDSP_POWER_ON				_IO(TDSP_MAGIC,0x2c)
#define TDSP_POWER_OFF				_IO(TDSP_MAGIC,0x2d)
#define TDSP_QUERY_BUS_POWER_STATE	_IO(TDSP_MAGIC,0x2e)
#define TDSP_SET_DEVICE_STATE		_IO(TDSP_MAGIC,0x2f)
#define TDSP_SET_HOT_FLAG			_IO(TDSP_MAGIC,0x30)
#define TDSP_SET_ANTENNA			_IO(TDSP_MAGIC,0x32)

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

//
// Define main structure for WB
//

typedef struct _TDSP_WB_POPMENU
{
  GtkWidget *mainmenu;
  GtkWidget *bluetooth;
  GtkWidget *wlan;
  GtkWidget *coexist;
  GtkWidget *unplug;
  GtkWidget *about;
  GtkWidget *exit;
  GtkWidget *poweron;
  GtkWidget *poweroff;
} TDSP_WB_POPMENU, *PTDSP_WB_POPMENU;

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
  unsigned int pos[4];
  unsigned char btver[64];
  unsigned char wlanver[64];
  unsigned char busver[64];
  unsigned long firmwarever;
} TDSPVERSION, *PTDSPVERSION;

gchar m_chagdesc[255];
gchar m_chbt1desc[255];
gchar m_chbt2desc[255];
gchar m_chcoexistwarning[255];
gchar m_chcompanyid[255];
gchar m_chgonlydesc[255];
gchar m_chproductid[255];
gchar m_chreleasedate[255];
gchar m_chsystemtraytips[255];
gchar m_chversion[255];
gint m_nHotKeyFlag;
gint m_nAntennaFlag;
gint m_nshowwarning;
gint m_nwlancfgexitnormally;

extern DEVICE_PNP_STATE g_dsppnpstate;
extern gboolean g_iscardin;
extern gboolean g_isclickinginprogress;
extern gboolean g_ismenuclicked;
extern gint g_buspowerstate;
extern gint g_iconindex;
extern gint g_state;
extern gint g_allowedmodes[3];
extern gulong g_busresetflag;
extern gulong g_currentserialno;

extern GtkWidget *tdsp_text_view;
extern TDSP_WB_POPMENU tdspuwbmenu;

void send_signal_close_tdsppcima();
int tdsp_query_power_state();
int tdsp_query_device();
gint tdsp_query_reset_flag();
gboolean tdsp_download_dsp_code(unsigned long serialno);
int tdsp_add_device(char *devicename, unsigned long namelen, unsigned long serialno);
int tdsp_remove_device(unsigned long serialno);
gboolean unplugdevices();
void get_menuitem_from_menu_by_index(GtkMenu * menu1, int index, GtkMenuItem ** pMenuItem);
void gray_wb_menu();
void enable_wb_menu();
int init_popmenu_item_struct(GtkWidget * menu1);
void tdsp_power_on();
void tdsp_power_off();
void tdsp_init_menu();
void tdsp_refresh_version();
void adjust_menu_item(gint modemenuitem);
void adjust_func_state();
void wait_device_unplug_stable();
gboolean tdsp_is_download_dspcode_ok();
int write_enum_mode(unsigned long mode);
void call_shell_for_btEnv(char *shellfile);
gboolean tdsp_set_init_flag(gulong mode);
gint tdsp_query_pnp_state(gulong mode);
void about_callback(GtkWidget * item, gpointer user_data);
void adjust_tooltip_text(gint state);
void refresh_gui_and_timeout();

#if 0
#define logging0(fmt, x...) \
do \
 { \
 FILE *FP = fopen(".uwb.log", "w+");\
 if (NULL != FP)\
 {\
  fprintf(FP,fmt, ##x);\
  fclose(FP);\
 } \
}while(0)

#define logging1(fmt, x...) \
do \
 { \
 FILE *FP = fopen(".uwb.log", "a+");\
 if (NULL != FP)\
 {\
  fprintf(FP,fmt, ##x);\
  fclose(FP);\
 } \
}while(0)
#endif

#ifdef DEBUG
#define debug(fmt, x...) \
do \
{ \
    printf(fmt, ##x); \
}while(0)
#else
#define debug(fmt, x...)
#endif

#define THISFUNCBECALLED debug("call --> %s\tf:%s, line:%d\n",__func__,__FILE__,__LINE__)

#endif
