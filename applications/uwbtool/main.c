/*********************************************************************** 
 * FILENAME:     main.c          
 * CREATE DATE:  2008/09/05 
 * PURPOSE:      uwbtool, run under shell 
 *                ... 
 * 
 * AUTHORS:      3dsp <ritow.qi@3dsp.com.cn> 
 * 
 * NOTES:        description of constraints when using functions of this file 
 * 
 ***********************************************************************/

/* 
 * REVISION HISTORY 
 *   ... 
 * 
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include "myhead.h"
#include "getopt.h"
#include "BuffPrint.h"
#include "callbacks.h"

#define UWBTOOLVER "0.3.02"

int g_state = 10;               /*undefined */
int g_buspowerstate;
unsigned long g_currentserialno;
unsigned long g_busresetflag;
int g_iscardin;
int g_isclickinginprogress = FALSE;
DEVICE_PNP_STATE g_dsppnpstate;
unsigned long g_subsystemid;
int g_allowedmodes[3];
int g_defaultmode;
unsigned long g_rawenummode = 0;
char g_chversionstring[256] = { "" };
char g_eeprombuf[256] = { "" };
int g_downfromfile = FALSE;

void logging(char *msg, int line)
{
#if 0
  FILE *fp;
  fp = fopen("/tmp/uwblog.txt", "a+");
  fprintf(fp, "%s at line %d\n", msg, line);
  fclose(fp);
#endif
}

int maincontrol()
{
#ifndef DISABLE_CONFIG_FILE
  int fd;
  int count;
  count = 0;
  char *wbconfig = TDSPWBCONFIGFILE;

  struct flock lock;

  fd = open(wbconfig, O_RDWR);
  if (fd < 0)
  {
    perror("lockfile");
    return 1;
  }

  lock.l_type = F_WRLCK;
  lock.l_whence = 0;
  lock.l_start = 0;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLK, &lock) < 0)
  {
    switch (errno)
    {
    case EAGAIN:
    case EACCES:
      fcntl(fd, F_GETLK, &lock);
      debug("pid: %ld process find pid: %ld lock the file %s\n",
            (long) getpid(), (long) lock.l_pid, wbconfig);
      printf("a 'uwb' or 'uwbtool' is already runing!\n");
      return 1;
    }
    perror("function fcntl call fail");
    return 1;
  }

  debug("pid: %ld process lock the file successfully!\n", (long) getpid());
#endif
  return 0;
}

void init_tdsp_otherenv(char *shellfile)
{
  char strshell[256];
  printf("\n");
  strcpy(strshell, "/bin/bash ");
  strcat(strshell, shellfile);
  strcat(strshell, " 1");
  system(strshell);
  printf("call after system\n");
  sleep(2);
  return;
}

int open_device(char *devicename)
{
  int fd;
  fd = open(devicename, O_RDONLY);
// debug ("open_device success, and its handle: fd = 0x%x\n",fd); 
  if (fd == -1)
  {
    g_iscardin = FALSE;
  }
  return (fd);
}

void close_device(int fd)
{
  // debug ("close_device: fd = 0x%x\n",fd);  
  close(fd);
}

int ioctl_device(int fd, int cmd, void *data)
{
  debug("ioctl_device: fd = 0x%x, cmd = 0x%x\n", fd, cmd);
  return (ioctl(fd, cmd, data));
}

int tdsp_query_power_state()
{
  int fd;
  int ret;
  debug("call the function: tdsp_query_power_state()\n");
  g_buspowerstate = PowerOff;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    debug(":open file error, fail!\n");
    perror("open error");
    return TDSP_SERIAL_NO_NOCARD;
  }
  if (ioctl_device(fd, TDSP_QUERY_BUS_POWER_STATE, &ret) != 0)
  {
    close_device(fd);
    debug(": ioctl error, fail!\n");
    perror("ioctl error");
    return TDSP_SERIAL_NO_NOCARD;
  }
  close_device(fd);
  debug(": ret = 0x%x\n", ret);
  g_buspowerstate = ret;
  return ret;
}

int tdsp_query_device()
{
  debug("call the function: tdsp_query_device \n");
  int fd;
  int ret;
  g_iscardin = FALSE;
  g_currentserialno = TDSP_SERIAL_NO_NOCARD;

  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return TDSP_SERIAL_NO_NOCARD;
  }
  if (ioctl_device(fd, TDSP_DISPLAY_IN_USE, &ret) == -1)
  {
    close_device(fd);
    perror("ioctl error");
    return TDSP_SERIAL_NO_NOCARD;
  }
  // debug ("tdsp_query_device: ret = 0x%x\n",ret); 
  close_device(fd);
  g_currentserialno = ret;
  g_iscardin = TRUE;
  debug("\ttdsp_query_device: g_iscardin = %d and g_currentserialno = 0x%x\n",
        g_iscardin, g_currentserialno);

  return ret;
}

int tdsp_query_reset_flag()
{
  int fd;
  int ret;
  g_busresetflag = 0;
  debug("call the function: tdsp_query_reset_flag()\n");
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return -1;
  }
  if (ioctl_device(fd, TDSP_QUERY_RESET_FLAG, &ret) == -1)
  {
    close_device(fd);
    perror("ioctl error");
    return -1;
  }
  close_device(fd);
  debug
    ("the function: tdsp_query_reset_flag: ret =  %d (0 is right, 1 is no right)\n",
     ret);
  g_busresetflag = ret;
  return ret;
}

int tdsp_query_pnp_state(unsigned long mode)
{
  debug("call the function: tdsp_query_pnp_state(%ld) \n", mode);
  int fd;
  int ret = mode;
  g_dsppnpstate = UnKnown;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("openerror");
    return 4;
  }
  if (ioctl_device(fd, TDSP_QUERY_DEVICE_STATE, &ret) == -1)
  {
    close_device(fd);
    perror("ioctlerror");
    return 4;
  }
  close_device(fd);

  g_dsppnpstate = (DEVICE_PNP_STATE) ret;
  debug("tdsp_query_pnp_state success: g_dsppnpstate = 0x%x\n", ret);
  return ret;
}

int tdsp_download_dsp_code(unsigned long serialno)
{
  debug("call the function: tdsp_download_dsp_code(%ld)\n ", serialno);
  // return TRUE;
  int fd;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    printf("open error");
    return FALSE;
  }
  unsigned long dwioctlcode;
  if (serialno == TDSP_SERIAL_NO_BT)
  {
    dwioctlcode = TDSP_RELOAD_BT_CODE;
  }
  else if (serialno == TDSP_SERIAL_NO_WLAN)
  {
    dwioctlcode = TDSP_RELOAD_WLAN_CODE;
  }
  else if (serialno == TDSP_SERIAL_NO_COEXIST)
  {
    dwioctlcode = TDSP_RELOAD_COMBO_CODE;
  }
  else
  {
    debug("the serialno is incorrect !");
    close_device(fd);
    return FALSE;
  }

  if (ioctl_device(fd, dwioctlcode, NULL) == -1)
  {
    debug("DOWNLOAD DSP CODE Failed");
    close_device(fd);
    return FALSE;
  }
  usleep(200000);
  debug("Download DSP code success! \n");
  close_device(fd);
  return TRUE;
}

int tdsp_download_dsp_code_from_file(unsigned long serialno)
{
  debug("call the function: tdsp_download_dsp_code_from_file(%ld)\n ",
        serialno);
  // return TRUE;
  int fd;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    printf("open error");
    close_device(fd);
    return FALSE;
  }
  if (ioctl_device(fd, TDSP_RELOAD_CODE_FROM_FILE, &serialno) == -1)
  {
    debug("DOWNLOAD DSP CODE FROM FILE  Failed\n");
    close_device(fd);
    return FALSE;
  }
  usleep(200000);
  debug("Download DSP code from file success! \n");
  close_device(fd);
  return TRUE;
}

int tdsp_set_init_flag(unsigned long mode)
{
  debug("call the function:tdsp_set_init_flag( %ld)\n", mode);
  int fd;
  int ret = mode;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("openerror");
    return FALSE;
  }
  if (ioctl_device(fd, TDSP_SET_INIT_FLAG, &ret) == -1)
  {
    close_device(fd);
    perror("ioctlerror");
    return FALSE;
  }
  close_device(fd);

  debug("\ttdsp_set_init_flag success\n");
  return TRUE;
}

int set_hotkey_flag(unsigned long mode)
{
  debug("call the function: set_hotkey_flag(%d)!\n", mode);
  int fd;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return -1;
  }
  if (ioctl_device(fd, TDSP_SET_HOT_FLAG, &mode) == -1)
  {
    close_device(fd);
    perror("set_hotkey_flag: ioctl error");
    return -1;
  }
  debug("set_hotkey_flag: success!\n");
  close_device(fd);
  return 0;
}

int read_enum_mode()
{
  int fd;
  unsigned long ret;
  debug("call the function: read_enum_mode \n");
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return -1;
  }
  if (ioctl_device(fd, TDSP_READ_ENUM_MODE, &ret) != 0)
  {
    close_device(fd);
    perror("ioctl error");
    return -1;
  }
  debug("get read_enum_mode is: ret = 0x%x\n", ret);
  close_device(fd);
  g_rawenummode = ret;
  return ret;
}

int write_enum_mode(unsigned long mode)
{
  debug("call the function: tdsp_write_enum_mode(%d)!\n", mode);
  int fd;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return -1;
  }
  if (ioctl_device(fd, TDSP_WRITE_ENUM_MODE, &mode) == -1)
  {
    close_device(fd);
    perror("tdsp_write_enum_mode: ioctl error");
    return -1;
  }
  debug("tdsp_ write_enum_mode: success\n");
  close_device(fd);
  return 0;
}

/*plugin device, when success, it return the device's serialno, when failure,
return TDSP_SERIAL_NOCARD 0X10000*/
int tdsp_add_device(char *devicename, unsigned long namelen,
                    unsigned long serialno)
{
  int fd;
  int bytes;
  PBUSENUM_PLUGIN_HARDWARE hardware;
  debug("call the function: tdsp_add_device(%s, %ld, %ld)!\n", devicename,
        namelen, serialno);
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return TDSP_SERIAL_NO_NOCARD;
  }
  hardware = (PBUSENUM_PLUGIN_HARDWARE)
    malloc(bytes = (sizeof(BUSENUM_PLUGIN_HARDWARE) + namelen));
  if (hardware == NULL)
  {
    debug("tdsp_add_device: malloc hardware error\n");
    close_device(fd);
    return TDSP_SERIAL_NO_NOCARD;
  }
  hardware->Size = sizeof(BUSENUM_PLUGIN_HARDWARE);
  hardware->SerialNo = serialno;
  memcpy(hardware->HardwareIDs, devicename, namelen);
  // debug ("tdsp_add_device: namelen = %d, total len = %d, the serialno is:%d\n", namelen, bytes,serialno);  
  if (ioctl_device(fd, TDSP_PLUGIN, hardware) == -1)
  {
    free(hardware);
    close_device(fd);
    perror("tdsp_add_device: ioctl error");
    return TDSP_SERIAL_NO_NOCARD;
  }
  debug("tdsp_add_device: success!the serialno is %d\n", serialno);
  free(hardware);
  close_device(fd);
  return serialno;
}

/*remove device, when success, it return the device's serialno, when failure, return TDSP_SERIAL_NOCARD 0X10000*/
int tdsp_remove_device(unsigned long serialno)
{
  int fd;
  BUSENUM_UNPLUG_HARDWARE unplug;
  int bytes;

  debug("call the function: tdsp_remove_device( %ld)\n", serialno);
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return TDSP_SERIAL_NO_NOCARD;
  }

  unplug.Size = bytes = sizeof(unplug);
  unplug.SerialNo = serialno;
  if (ioctl_device(fd, TDSP_UNPLUG, &unplug) == -1)
  {
    close_device(fd);
    perror("tdsp_remove_device: ioctl error");
    return TDSP_SERIAL_NO_NOCARD;
  }
  close_device(fd);
  debug("tdsp_remove_device( %ld ): success\n", serialno);
  return serialno;
}

int tdsp_write_eeprom(unsigned long uloffset, unsigned long ullength,
                      char *chbuf)
{
  debug
    ("call the function: tdsp_write_eeprom( offset=%ld,length=%ld,buf=\"%s\")\n",
     uloffset, ullength, chbuf);
  int fd;
  int bytes;
  PLOAD_EEPROM_STRUC peeprom;

  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return FALSE;
  }
  peeprom = (PLOAD_EEPROM_STRUC)
    malloc(bytes = (sizeof(LOAD_EEPROM_STRUC) + ullength));
  if (peeprom == NULL)
  {
    debug("tdsp_write_eeprom: malloc peeprom error\n");
    close_device(fd);
    return FALSE;
  }
  peeprom->offset = uloffset;
  peeprom->length = ullength;
  memcpy(peeprom->buf, chbuf, ullength);
  debug("the peeprom: peeprom->offse=%d,tpeeprom->length=%ld,peeprom->buf=%s\n",
        peeprom->offset, peeprom->length, peeprom->buf);
  if (ioctl_device(fd, TDSP_WRITE_EEPROM, peeprom) == -1)
  {
    free(peeprom);
    close_device(fd);
    perror("tdsp_write_eeprom: ioctl error");
    return FALSE;
  }
  free(peeprom);
  close_device(fd);
  return TRUE;
}

int tdsp_read_register(unsigned long uloffset, unsigned long ullength,
                       char *chbuf)
{
  debug("call the function: tdsp_read_register( offset=%ld,length=%ld)\n",
        uloffset, ullength);
  int fd;
  int bytes;
  PRW_REGISTER_STRUC pregister;

  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return FALSE;
  }
  pregister = (PRW_REGISTER_STRUC)
    malloc(bytes = (sizeof(RW_REGISTER_STRUC) + ullength + 1));
  if (pregister == NULL)
  {
    debug("tdsp_read_register: malloc pregister error\n");
    close_device(fd);
    return FALSE;
  }
  pregister->offset = uloffset;
  pregister->length = ullength;

  debug("the pregister: pregister->offse=%d,pregister->length=%ld\n",
        pregister->offset, pregister->length);
  if (ioctl_device(fd, TDSP_READ_REGISTER, pregister) == -1)
  {
    free(pregister);
    close_device(fd);
    printf("tdsp_read_register: ioctl error");
    return FALSE;
  }
  close_device(fd);
  pregister->buf[ullength + 1] = '\0';
  if (chbuf != NULL)
    memcpy(chbuf, pregister->buf, ullength);
  BtPrintBuffer(pregister->buf, ullength);
  free(pregister);
  return TRUE;
}

int tdsp_write_register(unsigned long uloffset, unsigned long ullength,
                        char *chbuf)
{
  debug("call the function: tdsp_write_register( offset=%ld,length=%ld)\n",
        uloffset, ullength);
  int fd;
  int bytes;
  PRW_REGISTER_STRUC pregister;

  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return FALSE;
  }
  pregister = (PRW_REGISTER_STRUC)
    malloc(bytes = (sizeof(RW_REGISTER_STRUC) + ullength));
  if (pregister == NULL)
  {
    debug("tdsp_read_register: malloc pregister error\n");
    close_device(fd);
    return FALSE;
  }
  pregister->offset = uloffset;
  pregister->length = ullength;
  memcpy(pregister->buf, chbuf, ullength);
  debug("the pregister: pregister->offse=%d,pregister->length=%ld\n",
        pregister->offset, pregister->length);
  if (ioctl_device(fd, TDSP_WRITE_REGISTER, pregister) == -1)
  {
    free(pregister);
    close_device(fd);
    printf("tdsp_read_register: ioctl error");
    return FALSE;
  }
  free(pregister);
  close_device(fd);
  return TRUE;
}

int tdsp_is_download_dspcode_ok()
{
  debug("call the fuction tdsp_is_download_dspcode_ok()\n\t");
  tdsp_query_device();
  if (g_iscardin != FALSE && g_currentserialno != 0xffff)
  {
    debug
      ("\ttdsp_is_download_dspcode_ok(), g_currentserialno is: 0x%x, success!\n",
       g_currentserialno);
    return TRUE;
  }
  else
  {
    debug
      ("\ttdsp_is_download_dspcode_ok(), g_currentserialno is: 0x%x, fail!\n",
       g_currentserialno);
    return FALSE;
  }
}

int unplugdevices()
{
  debug("call the function: unplugdevices() \n");
  if (g_state == BLUETOOTH_DISABLE_WLAN_DISABLE)
  {
    return TRUE;
  }

  tdsp_query_device();
  if (g_currentserialno == TDSP_SERIAL_NO_NOCARD)
  {
    return FALSE;
    /*should kill the program! */
  }

  /*if get the current serial no is :TDSP_SERIAL_NO_NONE ! */
  if (g_currentserialno == TDSP_SERIAL_NO_NONE)
  {
    return TRUE;
  }

  int iiTimes = 0;
  do
  {
    if (tdsp_query_reset_flag() == -1)
      debug("have error in fuction: tdsp_query_reset_flag()\n");
    if (g_busresetflag == 1)
    {
      usleep(1000000);
    }
    iiTimes++;
  }
  while (g_busresetflag == 1 && iiTimes < 60);
  debug("--g_busresetflag = %d,  and the loopcount iiTimes = %d\n",
        g_busresetflag, iiTimes);

  write_enum_mode(0);

  if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE)
  {
    tdsp_remove_device(TDSP_SERIAL_NO_BT);
    usleep(500000);
  }

  if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
  {
    tdsp_remove_device(TDSP_SERIAL_NO_WLAN);
    usleep(500000);
  }

  if (g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    tdsp_remove_device(TDSP_SERIAL_NO_BT);
    usleep(500000);
    int iTimes = 0;
    do
    {
      tdsp_query_power_state();
      if (g_buspowerstate != PowerOn)
      {
        debug("return becaus g_buspowerstate != PowerOn\n");
        return FALSE;
      }
      tdsp_query_device();
      usleep(500000);
      if (g_currentserialno == 0xffff)
        break;
      iTimes++;
    }
    while ((g_currentserialno & 0x1) == 0x1 && iTimes < 600);

    tdsp_remove_device(TDSP_SERIAL_NO_WLAN);
    usleep(500000);
  }

  int iTimes = 0;
  do
  {
    tdsp_query_power_state();
    if (g_buspowerstate != PowerOn)
    {
      debug("return becaus g_buspowerstate != PowerOn\n");
      return FALSE;
    }

    tdsp_query_device();

    usleep(500000);
    iTimes++;
  }
  while (g_currentserialno != 0xffff && iTimes < 1200);
  debug
    ("after function unplugdevice, the variable: g_currentserialno is 0x%x\n",
     g_currentserialno);

  g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;

  return TRUE;
}

void wait_device_unplug_stable()
{
#define TIMEOUTS 5
  debug("call the function: wait_device_unplug_stable()\n");
  tdsp_query_pnp_state(1);
  int nWaitCount = 0;
  while (g_dsppnpstate != 0xFF && nWaitCount < TIMEOUTS)
  {
    usleep(500000);
    tdsp_query_pnp_state(1);
    nWaitCount++;
  }
  tdsp_query_pnp_state(2);
  nWaitCount = 0;
  while (g_dsppnpstate != 0xFF && nWaitCount < TIMEOUTS)
  {
    usleep(500000);
    tdsp_query_pnp_state(2);
    nWaitCount++;
  }
  return;
}

int initial_tdsp_wb()
{
  debug("call the function: initial_tdsp_wb \n");
  int nenummode;
  nenummode = read_enum_mode();
  if (nenummode == -1)
  {
    debug("read_enum_mode fail !");
    debug("the g_defaultmode is: %d \n", g_defaultmode);
    return -1;
  }
  if (nenummode != 0)
  {
    if (g_allowedmodes[nenummode - 1] != 1)
      nenummode = 0;
  }

  g_rawenummode = nenummode;
  if (nenummode == 0)
  {
    nenummode = g_defaultmode;
  }
  g_defaultmode = nenummode;
  debug("the g_defaultmode is: %d \n", g_defaultmode);

  return 0;
}

int get_config_from_file()
{
  FILE *fp;
  fp = fopen(TDSPWBCONFIGFILE, "r");
  if (fp == NULL)
  {
    debug("the config file 'wbusb.conf' is not exist, check '%s'!\n",TDSPWBCONFIGFILE);
    return FALSE;
  }

  char recstring[100], chtmp[10];
  debug("call the function: get_config_from_file\n");

  if (get_value_from_conffile(fp, "HOTKEYFLAG", recstring) != 0)
  {
    m_nHotKeyFlag = atoi(recstring);
  }
  if (m_nHotKeyFlag > 1 || m_nHotKeyFlag < 0)
  {
    m_nHotKeyFlag = 0;
    goto END;
  }
  /*
     if(get_value_from_conffile(fp, "DEVAG", recstring)!=0){
     strcpy(m_chagdesc,recstring);
     }   
     else 
     goto END;

     if(get_value_from_conffile(fp, "DEVGONLY", recstring)!=0){
     strcpy(m_chgonlydesc,recstring);
     }
     else 
     goto END;

     // *[BT] *  BTV1=3DSP BlueToothv.1.0 *BTV2=3DSP BlueToothv.2.0 
     if(get_value_from_conffile(fp, "BTV1", recstring)!=0){
     strcpy(m_chbt1desc,recstring);
     }
     else 
     goto END;

     if(get_value_from_conffile(fp, "BTV2", recstring)!=0){
     strcpy(m_chbt2desc,recstring);
     }
     else 
     goto END;

     // **      [WB]   **    
     if(get_value_from_conffile(fp, "SYSTEMTRAYTIPS", recstring)!=0){
     strcpy(m_chsystemtraytips,recstring);
     }
     else 
     goto END; */

     if(get_value_from_conffile(fp, "COMPANYID", recstring)!=0){
     strcpy(m_chcompanyid,recstring);
     }
     else 
     goto END;

/*	 if(get_value_from_conffile(fp, "WLANCFGEXITNORMALLY", recstring)!=0){
     m_nwlancfgexitnormally=atoi(recstring);
     }
     if(!(m_nwlancfgexitnormally==0||m_nwlancfgexitnormally==1)){
     goto END;
     }

     if(get_value_from_conffile(fp, "SHOWWARNING", recstring)!=0){
     m_nshowwarning=atoi(recstring);
     }

     if(get_value_from_conffile(fp, "COEXISTWARNING", recstring)!=0){
     strcpy(m_chcoexistwarning,recstring);
     }
     else 
     goto END;
   */
  if (get_value_from_conffile(fp, "DEFAULTMODE", recstring) != 0)
  {
    g_defaultmode = atoi(recstring);
  }
  if (g_defaultmode > 3 || g_defaultmode <= 0)
  {
    g_defaultmode = 0;
    goto END;
  }

  if (get_value_from_conffile(fp, "ALLOWEDMODES", recstring) != 0)
  {
    strcpy(chtmp, recstring);
  }
  else
    goto END;

  int nlen = strlen(chtmp);
  if (nlen > 3)
  {
    chtmp[3] = '\0';
  }
  debug("--------------------------%d,%d,%d,%d,%d\n", chtmp[0], chtmp[1],
        chtmp[2], chtmp[3], nlen);
  int i, index;
  for (i = 0; i < nlen; i++)
  {
    index = chtmp[i] - '0' - 1;
    if (index >= 3 || index < 0)
    {
      goto END;
    }
    g_allowedmodes[index] = 1;
  }
  debug
    ("g_allowedmodes[0]=%d,g_allowedmodes[1]=%d,g_allowedmodes[2]=%d,g_defaultmode=%d\n",
     g_allowedmodes[0], g_allowedmodes[1], g_allowedmodes[2], g_defaultmode);
  if (g_allowedmodes[g_defaultmode - 1] != 1)
    goto END;

 /* 
     if(get_value_from_conffile(fp, "VERSION", recstring)!=0){
     strcpy(m_chversion,recstring);
     }
     else 
     goto END;

     if(get_value_from_conffile(fp, "RELEASEDATE", recstring)!=0){
     strcpy(m_chreleasedate,recstring);
     }
     else 
     goto END;

     if(get_value_from_conffile(fp, "PRODUCTID", recstring)!=0){
     strcpy(m_chproductid,recstring);
     }
     else 
     goto END;

     debug("\nm_chagdesc:%s,\nm_chgonlydesc:%s,\nm_chbt1desc:%s,\nm_chbt2desc:%s,\nm_chsystemtraytips:%s,\nm_chcompanyid:%s,\nm_nwlancfgexitnormally:%d,\nm_nshowwarning:%d,\nm_chcoexistwarning:%s,\nm_chversion:%s,\nm_chreleasedate:%s,\nm_chproductid:%s\n",m_chagdesc,m_chgonlydesc,m_chbt1desc,m_chbt2desc,m_chsystemtraytips,m_chcompanyid,m_nwlancfgexitnormally,m_nshowwarning,m_chcoexistwarning,m_chversion,m_chreleasedate,m_chproductid);
   */
  fclose(fp);
  return TRUE;
END:
  fclose(fp);
  return FALSE;
}

void tdsp_power_on()
{
  int fd;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return;
  }

  debug("tdsp_bus_power_on ... wait ...\n");
  if (ioctl_device(fd, TDSP_POWER_ON, NULL) == -1)
  {
    close_device(fd);
    perror("tdsp_power_on: ioctl error  ");
    return;
  }
  debug("tdsp_power_on: success\n");
  close_device(fd);
  return;

}

void tdsp_power_off()
{
  int fd;
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return;
  }

  debug("tdsp_bus_power_off ... wait ...\n");
  if (ioctl_device(fd, TDSP_POWER_OFF, NULL) == -1)
  {
    close_device(fd);
    perror("tdsp_power_off: ioctl error ");
    return;
  }
  debug("tdsp_power_off: success\n");
  close_device(fd);
  return;
}

void adjust_func_state()
{
  debug("call the function: adjust_func_state()\n\t");
  tdsp_query_device();
  if (g_iscardin == FALSE)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
  }
  if (g_currentserialno == 0x1)
  {
    g_state = BLUETOOTH_ENABLE_WLAN_DISABLE;
  }
  else if (g_currentserialno == 0x2)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_ENABLE;
  }
  else if (g_currentserialno == 0x3)
  {
    g_state = BLUETOOTH_ENABLE_WLAN_ENABLE;
  }
  else if (g_currentserialno == 0xffff)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
  }
}

int hexstr2intnum(char *psrc, int nlen, int *num)
{
  int i = 0;
  *num = 0;
  for (i = 0; i < nlen; i++)
  {
    if (isxdigit(psrc[i]) == FALSE)
    {
      printf("error data, '%c' is not xdigit, at %d\n", psrc[i], i);
      return FALSE;
    }
    int nval = toupper(psrc[i]);
    if (nval >= 'A')
    {
      nval = nval - 'A' + 10;
    }
    else
    {
      nval = nval - '0';
    }

    *num += (int) (nval * pow(16, nlen - i - 1));
  }
  return TRUE;
}

char *trim_gets_string(char *valstring)
{
  char *p = NULL, *pret = NULL;
  for (p = valstring; p - valstring < strlen(valstring); p++)
  {
    if (*p == 32 || *p == 9)
    {
      pret = p + 1;
      continue;
    }
    break;
  }
  for (p = valstring; p - valstring < strlen(valstring); p++)
  {
    if (*p == 10 || *p == 13)
    {
      *p = 0;
      break;
    }
  }
  return pret;
}

int get_value_from_conffile(FILE * fp, char *keystring, char *valstring)
{
  FILE *ini;
  char *name = NULL, *value = NULL, line[100];
  ini = fp;
  rewind(ini);
  if (ini == NULL)
    ini = fopen(TDSPWBCONFIGFILE, "r");
  while (!feof(ini))
  {
    fgets(line, 100, ini);
    if (*line == ';')
    {
      memset(line, '0', 100);
      continue;
    }
    trim_gets_string(line);
    name = strtok(line, "=");
    value = strtok(0, "=");
    if (name && value)
    {
      if (strcasecmp(name, keystring) == 0)
      {
        strcpy(valstring, value);
        debug("%s=%s\n", keystring, valstring);
        return 1;
      }
    }
    else
      continue;
    if (strcasecmp(name, "MENU_BT") == 0)
      break;
  }
  printf("\n");
  return 0;
}

int get_tdsp_version(PTDSPVERSION ver)
{
  int fd;
  unsigned char verinfo[64];
  debug("call the function: get_tdsp_version ()\n");
  fd = open_device(TDSP_BUS_DEVICE_NAME);
  if (fd == -1)
  {
    perror("open error");
    return -1;
  }

  int bsuccess;
  bsuccess = FALSE;
  if (ver->pos[0] == 1)
  {
    long serial = 1;
    memset(verinfo, '\0', 64);
    memcpy(verinfo, &serial, sizeof(serial));
    if (ioctl_device(fd, TDSP_QUERY_DEVICE_VERSION, &verinfo) == -1)
    {
      close_device(fd);
      ver->pos[0] = 0;
      perror("ioctl error");
      return -1;
    }
    else
    {
      strcpy((char *) ver->btver, (char *) verinfo);
    }
  }
  if (ver->pos[1] == 1)
  {
    long serial = 2;
    memset(verinfo, '\0', 64);
    memcpy(verinfo, &serial, sizeof(serial));
    if (ioctl_device(fd, TDSP_QUERY_DEVICE_VERSION, &verinfo) == -1)
    {
      close_device(fd);
      ver->pos[1] = 0;
      perror("ioctl error");
      return -1;
    }
    else
    {
      strcpy((char *) ver->wlanver, (char *) verinfo);
    }
  }
  if (ver->pos[2] == 1)
  {
    memset(verinfo, '\0', 64);
    if (ioctl_device(fd, TDSP_QUERY_BUS_VERSION, &verinfo) == -1)
    {
      close_device(fd);
      ver->pos[2] = 0;
      perror("ioctl error");
      return -1;
    }
    else
    {
      strcpy((char *) ver->busver, (char *) verinfo);
    }
  }
  if (ver->pos[3] == 1)
  {
    unsigned long verinfo1;
    if (ioctl_device(fd, TDSP_QUERY_FIRMWARE_VERSION, &verinfo1) == -1)
    {
      close_device(fd);
      ver->pos[3] = 0;
      perror("ioctl error");
      return -1;
    }
    else
    {
      ver->firmwarever = verinfo1;
    }
  }
  if (ver->pos[4] == 1)
  {
    unsigned long verinfo2;
    if (ioctl_device(fd, TDSP_QUERY_8051_VERSION, &verinfo2) == -1)
    {
      close_device(fd);
      ver->pos[4] = 0;
      perror("ioctl error");
      return -1;
    }
    else
    {
      ver->ver8051= verinfo2;
    }
  }
  close_device(fd);
  return TRUE;
}

void update_ver_string()
{

  TDSPVERSION ver;
  memset(&ver, 0, sizeof(ver));
  ver.pos[0] = ver.pos[1] = 0;
  ver.pos[2] = ver.pos[3] = ver.pos[4] = 1;
  adjust_func_state();
  if (g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    ver.pos[0] = ver.pos[1] = 1;
  }
  else if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE)
  {
    ver.pos[0] = 1;
  }
  else if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
  {
    ver.pos[1] = 1;
  }
  if (FALSE == get_tdsp_version(&ver))
  {
    ver.pos[0] = ver.pos[1] = ver.pos[2] = ver.pos[3] = 0;
  }
  char chversion[256] = { "" };
  char chtmp[256];
  /*in the follow function: 2=major_version, 0=minor_version,7=sub_minor_version */
  //sprintf(chversion, "  \t%s wb %d.%d.%d\n", m_chcompanyid, 2, 0, 7);
  sprintf(chversion, "  \t%s wbtool %s\n", m_chcompanyid, UWBTOOLVER);
  if (ver.pos[0] == 1)
  {
//    sprintf(chtmp, "   bluetooth:  %s\n", (const char *) ver.btver);
    sprintf(chtmp, "   %-9s: %s\n", "bluetooth",(const char *) ver.btver);
  }
  else
  {
//    sprintf(chtmp, "   bluetooth:  %s\n", "invalid");
    sprintf(chtmp, "   %-9s: %s\n", "bluetooth","invalid");
  }
  strcat(chversion, chtmp);
  if (ver.pos[1] == 1)
  {
    sprintf(chtmp, "   %-9s: %s\n", "wlan",(const char *) ver.wlanver);
  }
  else
  {
    sprintf(chtmp, "   %-9s: %s\n", "wlan", "invalid");
  }
  strcat(chversion, chtmp);
  if (ver.pos[2] == 1)
  {
//    sprintf(chtmp, "   %sbus:  %s\n", m_chcompanyid, (const char *) ver.busver);
    sprintf(chtmp, "   %-5s%-4s: %s\n", m_chcompanyid, "bus",(const char *) ver.busver);
  }
  else
  {
//    sprintf(chtmp, "  bus driver:  %s\n", "invalid");
    sprintf(chtmp, "   busdriver:  %s\n", "invalid");
  }  
  strcat(chversion, chtmp);
  /*if (ver.pos[4] == 1)
  {
    sprintf(chtmp, "   %-5s%-4s: %s\n", m_chcompanyid, "8051",(const char *) ver.ver8051);
  }
  else
  {
    sprintf(chtmp, "   busdriver:  %s\n", "invalid");
  }*/
  if (ver.pos[4] == 1 && (ver.pos[0] + ver.pos[1] > 0))
  {
    strcpy(chtmp, "");
    char chversiontmp[10] = { "" };
    sprintf(chversiontmp, "%08lx", ver.ver8051);

    char chv[10];

    // board type

    char chboardtype[10] = "";
    chboardtype[0] = chversiontmp[0];
    chboardtype[1] = chversiontmp[1];
    chboardtype[2] = '\0';
    int board_type;
    hexstr2intnum(chboardtype, 2, &board_type);
    int major_version;
    chv[0] = chversiontmp[2];
    chv[1] = chversiontmp[3];
    chv[2] = '\0';
    hexstr2intnum(chv, 2, &major_version);
    char chtesttype[10] = "";
    chtesttype[0] = chversiontmp[4];
    chtesttype[1] = chversiontmp[5];
    chtesttype[2] = '\0';
    int test_type;
    hexstr2intnum(chtesttype, 2, &test_type);
    int minor_version;
    chv[0] = chversiontmp[6];
    chv[1] = chversiontmp[7];
    chv[2] = '\0';
    hexstr2intnum(chv, 2, &minor_version);
//  sprintf(chtmp,"%2s.%02d.%2s.%03d\n",chboardtype,major_version,chtesttype,minor_version);
    sprintf(chtmp, "   %-5s%-4s: %02d.%02d.%02d.%d\n", m_chcompanyid, "8051",board_type, major_version,test_type, minor_version);
  }
  else
  {
    sprintf(chtmp, "   firmware : %s\n", "invalid");
  }
  strcat(chversion, chtmp);
  if (ver.pos[3] == 1 && (ver.pos[0] + ver.pos[1] > 0))
  {
    strcat(chversion, "   firmware : ");
    strcpy(chtmp, "");
    char chversiontmp[50] = { "" };
    sprintf(chversiontmp, "%08lx", ver.firmwarever);

    char chv[10];

    // board type

    char chboardtype[10] = "";
    chboardtype[0] = chversiontmp[0];
    chboardtype[1] = chversiontmp[1];
    chboardtype[2] = '\0';
    int board_type;
    hexstr2intnum(chboardtype, 2, &board_type);
    int major_version;
    chv[0] = chversiontmp[2];
    chv[1] = chversiontmp[3];
    chv[2] = '\0';
    hexstr2intnum(chv, 2, &major_version);
    char chtesttype[10] = "";
    chtesttype[0] = chversiontmp[4];
    chtesttype[1] = chversiontmp[5];
    chtesttype[2] = '\0';
    int test_type;
    hexstr2intnum(chtesttype, 2, &test_type);
    int minor_version;
    chv[0] = chversiontmp[6];
    chv[1] = chversiontmp[7];
    chv[2] = '\0';
    hexstr2intnum(chv, 2, &minor_version);
//  sprintf(chtmp,"%2s.%02d.%2s.%03d\n",chboardtype,major_version,chtesttype,minor_version);
    sprintf(chtmp, "%2d.%02d.%02d.%03d\n", board_type, major_version,
            test_type, minor_version);
  }
  else
  {
    sprintf(chtmp, "   firmware : %s\n", "invalid");
  }
  strcat(chversion, chtmp);

  strcpy(g_chversionstring, chversion);
  debug("update_version() get g_chversionstring =\n");
  printf(" +------------------------------+ \n");
  printf("%s", g_chversionstring);
  printf(" +------------------------------+ \n");
  return;

//   sprintf(chtmp,"%s  bluetooth and wlan card",m_chcompanyid);

}

void display_usage(void)
{
  printf("Application options\n\
  -q, --query                    query the current device, such as:\n\
                                 -q or --query\n\
  -a, --add=DEVICE               add a device, such as: bt, wlan, combo\n\
  -d, --download=DEVICE          add a device, download code from file\n\
                                 such as: bt, wlan, combo\n\
  -r, --remove                   remove all the 3dsp device,such as:\n\
                                 -r or --remove\n\
  -v, --version                  query the card version, such as:\n\
                                 -v or --version\n");
/*
  -w, --write=EEPROM             write the eeprom,such as:\n\
                                 -w bd ************\n\
                                 -w key ********************************\n\
                                 -w xxxx **\n\
  -g, --get=REGOFFSET            read the register by 'offset length',such as:\n\
                                 -g xxxx ** or -g xxxx 0x**\n\
                                 --get=xxxx **\n\
  -p, --put=REGOFFSET            write the register by 'offset data',such as:\n\
                                 -p xxxx ********\n\
                                 --put=xxxx ********\n\
  -v, --version                  query the card version, such as:\n\
                                 -q or --version\n");*/
  return;
}

#define HAVE_NOT_THIS_FUNCTION(_sss) do\
{\
	printf(#_sss);\
	printf("sorry, this function is not accessable temporary\n");\
	return 0;\
}while(0)

int get_eeprom_data_2write(char *greet, int length)
{
  if (greet == NULL)
  {
    printf("NULL please input the data you want to write \n");
    return FALSE;
  }

  char *greetings;
  greetings = greet;
  debug("the greetings is:%s\n", greetings);

  int ilen = strlen(greetings);
  if (ilen != length)
  {
    printf("the data length is %d, is not %d \n", ilen, length);
    return FALSE;
  }
  printf("the data you want to write into eeprom is: %s\n", greetings);
  int ii, num;
  char *pchar = greetings;
  for (ii = 0; ii < length; ii += 2, pchar += 2)
  {
    debug("-----:%c,%c-----\n", greetings[ii], greetings[ii + 1]);
    if (isxdigit(greetings[ii]) == FALSE)
    {
      printf("error data, is not xdigit, at %d\n", ii);
      return FALSE;
    }
    if (isxdigit(greetings[ii + 1]) == FALSE)
    {
      printf("error data, is not xdigit, at %d\n", ii + 1);
      return FALSE;
    }
    hexstr2intnum(pchar, 2, &num);
    g_eeprombuf[ii / 2] = num;
    debug("%d--:ox%x\n", ii / 2, g_eeprombuf[ii / 2]);
  }
  g_eeprombuf[ii / 2] = 0;
  return TRUE;
}

struct globalargs_t globalargs;

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    debug("have no arguments!\n");
    display_usage();
    return 1;
  }
  if (*(*(argv + 1)) != '-')
  {
    printf("the argument must start with '-' .\n");
    return 1;
  }

  // if(maincontrol()) {return 0;}

  debug("the argc is: %d, and the argv is: %s \n", argc, *argv);

  globalargs.iquery = 0;
  globalargs.iremove = 0;
  globalargs.chadd = NULL;
  globalargs.chwrite = NULL;
  globalargs.chget = NULL;
  globalargs.chput = NULL;
  globalargs.iversion = 0;
  globalargs.randomized = 0;

  char *greet = NULL;

  static const struct option longopts[] = {
    {"query", no_argument, NULL, 'q'},
    {"add", required_argument, NULL, 'a'},
    {"download", required_argument, NULL, 'd'},
    {"remove", no_argument, NULL, 'r'},
    {"write", required_argument, NULL, 'w'},
    {"get", required_argument, NULL, 'g'},
    {"put", required_argument, NULL, 'p'},
    {"version", no_argument, NULL, 'v'},
    {"randomize", no_argument, NULL, 0},
    {"help", no_argument, NULL, 'h'},
    {NULL, no_argument, NULL, 0}
  };
  static const char *optstring = "qrd:a:w:g:p:vh?";

  int opt = 0;
  int longindex = 0;
  opt = getopt_long(argc, argv, optstring, longopts, &longindex);

  while (opt != -1)
  {
    switch (opt)
    {
    case 'q':
      globalargs.iquery = 1;    /* true */
      debug("case q:-----------\n");
      break;
    case 'r':
      globalargs.iremove = 1;
      debug("case r -----------\n");
      break;
    case 'd':
      g_downfromfile = TRUE;
    case 'a':
      globalargs.chadd = optarg;
      debug("case a ----------: %s\n", globalargs.chadd);
      break;
    case 'w':
      globalargs.chwrite = optarg;
      debug("case w----------:%s \n", globalargs.chwrite);
	  HAVE_NOT_THIS_FUNCTION("");
      break;
    case 'g':
      globalargs.chget = optarg;
      debug("case g----------:%s \n", globalargs.chget);
	  HAVE_NOT_THIS_FUNCTION("");
      break;
    case 'p':
      globalargs.chput = optarg;
      debug("case p----------:%s \n", globalargs.chput);
	  HAVE_NOT_THIS_FUNCTION("");
      break;
    case 'v':
      globalargs.iversion = 1;
      debug("case v -----------\n");
      break;
    case 'h':                  /* fall-through is intentional */
    case '?':
      display_usage();
      debug("case h or ?-------------\n");
      return 0;
      break;
    case 0:                    /* long option without a short arg */
      if (strcmp("randomize", longopts[longindex].name) == 0)
      {
        globalargs.randomized = 1;
      }
      debug("case 0 -----------\n");
      return 0;
      break;
    default:
      /* you won't actually get here. */
      debug("case default -----------\n");
      return 0;
      break;
    }
    break;
  }
  if (getopt_long(argc, argv, optstring, longopts, &longindex) != -1)
  {
    display_usage();
    return 0;
  }
  debug("the length of the second argv is--:%d\n", strlen(*(argv + 1)));
  if (*(*(argv + 1) + 1) != '-' && strlen(*(argv + 1)) != 2)
  {
    display_usage();
    return 0;
  }
/*---------------------------------------------------------------------------------*/
  g_allowedmodes[0] = 0;
  g_allowedmodes[1] = 0;
  g_allowedmodes[2] = 0;
  g_defaultmode = 3;
  get_config_from_file();
  // initial_tdsp_wb();
  tdsp_query_power_state();
  if (g_buspowerstate != PowerOn)
  {
    printf("please make sure the 3DSP_card(usb) is  inserted!\n");
    return 0;
  }
  if (set_hotkey_flag((unsigned long) m_nHotKeyFlag) != 0)
    debug("set the hotkey flag of uwb fail!\n");
/*---------------------------------------------------------------------------------*/
  int ioffset;
  long length;

  if (globalargs.iquery && argc == 2)
  {
    debug("call the query device\n");
    tdsp_query_device();
    if (g_iscardin == 0)
      printf("no card in, please insert the tdsp card!\n");
    else
    {
      switch (g_currentserialno)
      {
      case TDSP_SERIAL_NO_BT:
        printf("the current device is bluetooth \n");
        break;
      case TDSP_SERIAL_NO_WLAN:
        printf("the current device is wlan \n");
        break;
      case TDSP_SERIAL_NO_COEXIST:
        printf("the current device is bluetooth&wlan \n");
        break;
      case TDSP_SERIAL_NO_NONE:
        printf("the current device is none\n");
        break;
      default:
        printf("the current device is undefined \n");
        break;
      }
    }
  }
  else if (globalargs.iquery && argc != 2)
  {
    printf("call the query device, don't need the argument\n");
  }
  else if (globalargs.iremove && argc == 2)
  {
    printf("call the remove device\n");
    if (maincontrol())
    {
      return 0;
    }
    on_unplug_activate();
    if (tdsp_query_device() == TDSP_SERIAL_NO_NONE)
    {
      printf("^_^ remove devices success!\n");
    }
    else
    {
      printf("*_* remove devices fail!\n");
    }
  }
  else if (globalargs.iremove && argc != 2)
  {
    printf("call the remove device don't need the argument\n");
  }
  else if (globalargs.chadd != NULL)
  {
    if (*(*(argv + 1) + 1) == '-')
    {
      if (argc != 2)
      {
        display_usage();
        return 0;
      }
    }
    else
    {
      if (argc != 3)
      {
        display_usage();
        return 0;
      }
    }

    if (strcasecmp(globalargs.chadd, "bt") == 0)
    {
      printf("the device you want to add:--- %s\n", globalargs.chadd);
      if (maincontrol())
      {
        return 0;
      }
      on_bluetooth_activate();
      if (tdsp_query_device() == TDSP_SERIAL_NO_BT)
      {
        printf("^_^ add bluetooth success!\n");
      }
      else
      {
        printf("*_* add bluetooth fail!\n");
      }
    }
    else if (strcasecmp(globalargs.chadd, "wlan") == 0)
    {
      printf("the device you want to add:---%s\n", globalargs.chadd);
      if (maincontrol())
      {
        return 0;
      }
      on_wlan_activate();
      if (tdsp_query_device() == TDSP_SERIAL_NO_WLAN)
      {
        printf("^_^ add wlan success!\n");
      }
      else
      {
        printf("*_* add wlan fail!\n");
      }
    }
    else if (strcasecmp(globalargs.chadd, "combo") == 0)
    {
      printf("the devices you want to add:---%s\n", globalargs.chadd);
      if (maincontrol())
      {
        return 0;
      }
      on_coexist_activate();
      if (tdsp_query_device() == TDSP_SERIAL_NO_COEXIST)
      {
        printf("^_^ add bluetooth&wlan success!\n");
      }
      else
      {
        printf("*_* add bluetooth&wlan fail!\n");
      }
    }
    else
    {
      printf
        ("*_* only 'bt', 'wlan', 'combo' is promit! but the device you want to add:---: %s\n",
         globalargs.chadd);
    }
  }
  else if (globalargs.chwrite != NULL)
  {
    if (*(*(argv + 1) + 1) == '-')
    {
      if (argc != 3)
      {
        display_usage();
        return 0;
      }
      greet = *(argv + 2);
    }
    else
    {
      if (argc != 4)
      {
        display_usage();
        return 0;
      }
      greet = *(argv + 3);
    }
    tdsp_query_device();
    if (g_iscardin == FALSE)
    {
      printf("no card in, please insert the tdsp card first!\n");
      return 0;
    }
    if (g_currentserialno == TDSP_SERIAL_NO_NONE)
    {
      printf("please add a device bt or wlan ... \n");
      return 0;
    }
    debug("write the eeprom \n");
    if (strcasecmp(globalargs.chwrite, "bd") == 0)
    {
      printf("write eeprom ---------------bd\n");

      if (get_eeprom_data_2write(greet, 12) == FALSE)
      {
        debug("");
        return 0;
      }
      if (tdsp_write_eeprom(671, 6, g_eeprombuf) == FALSE)
      {
        printf("write eeprom bd fail!\n");
      }
      else
      {
        printf("write eeprom bd success!\n");
      }
    }
    else if (strcasecmp(globalargs.chwrite, "key") == 0)
    {
      printf("write eeprom ---------------key\n");

      if (get_eeprom_data_2write(greet, 32) == FALSE)
      {
        debug("");
        return 0;
      }
      if (tdsp_write_eeprom(677, 16, g_eeprombuf) == FALSE)
      {
        printf("write eeprom key fail!\n");
      }
      else
      {
        printf("write eeprom key success!\n");
      }
    }
    else
    {
      printf("write eeprom ---------------other\n");
      int offset;
      offset = strtol(globalargs.chwrite, NULL, 0);
      if (offset >= 1024 || offset < 0)
      {
        printf("offset out of range! it must between 0~1023(decimal)\n");
        return 0;
      }
      if (get_eeprom_data_2write(greet, 2) == FALSE)
      {
        debug("");
        return 0;
      }
      printf("write eeprom offset=%d, content is: 0x%s\n", offset, greet);
      if (tdsp_write_eeprom(offset, 1, g_eeprombuf) == FALSE)
      {
        printf("write eeprom offset=%d fail!\n", offset);
      }
      else
      {
        printf("write eeprom offset=%d,content is: 0x%s,success!\n",
               offset, greet);
      }
    }
  }
  else if (globalargs.chget != NULL)
  {
    if (*(*(argv + 1) + 1) == '-')
    {
      if (argc != 3)
      {
        display_usage();
        return 0;
      }
      greet = *(argv + 2);
    }
    else
    {
      if (argc != 4)
      {
        display_usage();
        return 0;
      }
      greet = *(argv + 3);
    }
    tdsp_query_device();
    if (g_iscardin == FALSE)
    {
      printf("no card in, please insert the tdsp card first!\n");
      return 1;
    }
/*  if (g_currentserialno==TDSP_SERIAL_NO_NONE)
	       {printf("please add a device bt or wlan ... \n");
	       return;}*/
    debug("get the register %s  %s \n", globalargs.chget, greet);

    if (strlen(globalargs.chget) != 4)
    {
      printf("the offset is too short, its length must be 4 \n");
      return 1;
    }
    else
    {
      if (hexstr2intnum(globalargs.chget, 4, &ioffset) == FALSE)
        return 1;
      if (ioffset > 0x6000 || ioffset <= 0)
      {
        printf("offset out of range!\n");
        return 1;
      }
    }
    length = strtol(greet, NULL, 0);
    if (length > 1024 || length <= 0)
    {
      printf("length out of range! it must between 1~1024 \n");
      return 1;
    }
    else
    {
      debug("get register offset= %d, length = %ld\n", ioffset, length);
      tdsp_read_register((unsigned long) ioffset, length, NULL);
    }
  }
  else if (globalargs.chput != NULL)
  {
    if (*(*(argv + 1) + 1) == '-')
    {
      if (argc != 3)
      {
        display_usage();
        return 1;
      }
      greet = *(argv + 2);
    }
    else
    {
      if (argc != 4)
      {
        display_usage();
        return 1;
      }
      greet = *(argv + 3);
    }
    tdsp_query_device();
    if (g_iscardin == FALSE)
    {
      printf("no card in, please insert the tdsp card first!\n");
      return 1;
    }
/*  if (g_currentserialno==TDSP_SERIAL_NO_NONE)
	       {printf("please add a device bt or wlan ... \n");
	       return;}*/
    debug("write the register at offset= %s, the data= %s \n",
          globalargs.chput, greet);

    if (strlen(globalargs.chput) != 4)
    {
      printf("the offset is too short, its length must be 4 \n");
      return 1;
    }
    else
    {
      if (hexstr2intnum(globalargs.chput, 4, &ioffset) == FALSE)
        return 1;
      if (ioffset > 0x6000 || ioffset <= 0)
      {
        printf("offset out of range!it must less then 0x6000\n");
        return 1;
      }
    }

    length = strlen(greet);
    if (length != WTRGLEN)
    {
      length = WTRGLEN;
      printf("length error! its must be %d \n", WTRGLEN);
      return 1;
    }
    else
    {
      debug("put register-----------------!\n");
      if (get_eeprom_data_2write(greet, WTRGLEN) == FALSE)
      {
        debug("");
        return 1;
      }
      if (tdsp_write_register(ioffset, WTRGLEN, g_eeprombuf) == FALSE)
      {
        printf("write register fail!\n");
      }
      else
      {
        printf("write register success!\n");
      }
    }
  }
  else if (globalargs.iversion && argc == 2)
  {
    printf("Query the card version\n\n");
    update_ver_string();
  }
  else if (globalargs.iversion && argc != 2)
  {
    printf("query the card version don't need the argument\n");
  }
  else
  {
    debug("other! at the end of main()\n");
    printf
      ("use '--help' or '-r' to see a full list of available command line options.\n");
  }
  return 0;
}
