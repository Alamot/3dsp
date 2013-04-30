/*********************************************************************** 
 * FILENAME:     callback.c          
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "callbacks.h"
#include "myhead.h"

void on_bluetooth_activate()
{

  if (g_isclickinginprogress != FALSE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;

  debug("+ call the function: on_bluetooth_active()\n");
  
#ifndef DISABLE_CONFIG_FILE
  
  if (g_allowedmodes[0] == 0)
  {
    printf("the mode: bt(bluetooth) is disabled!\n");
    return;
  }
#elif DISABLE_BT_ONLY
    printf("the mode: bt(bluetooth) is disabled!\n");
	return;
#endif

  adjust_func_state();
  if (!g_iscardin)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    printf("call on_bluetooth_active, return for the g_iscardin = FALSE\n");
    g_isclickinginprogress = FALSE;
    return;
  }
  if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE)
  {
    printf("call onbluetoothactive, the last state already is on_bluetooth\n");
    g_isclickinginprogress = FALSE;
    return;
  }
  if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE
      || g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    if (FALSE == unplugdevices())
    {
      printf("unplugdivice fail ! \n");
      g_isclickinginprogress = FALSE;
      return;
    }
    wait_device_unplug_stable();
  }

  write_enum_mode(1);
  if (g_downfromfile == FALSE)
  {
    tdsp_download_dsp_code(1);
  }
  else
  {
    tdsp_download_dsp_code_from_file(1);
  }
  usleep(500000);
  tdsp_add_device(TDSPBTID, sizeof(TDSPBTID), TDSP_SERIAL_NO_BT);
  usleep(500000);

  if (tdsp_is_download_dspcode_ok() == FALSE)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    adjust_func_state();
    g_isclickinginprogress = FALSE;
    return;
  }
  g_state = BLUETOOTH_ENABLE_WLAN_DISABLE;
  g_isclickinginprogress = FALSE;
  init_tdsp_otherenv(TDSPSHELLFILEFORBT);
  return;
}

void on_wlan_activate()
{

  if (g_isclickinginprogress != FALSE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;

  debug("+ call the function: on_wlan_active()\n");
  
#ifndef DISABLE_CONFIG_FILE
  if (g_allowedmodes[1] == 0)
  {
    printf("the mode: wlan is disabled!\n");
    return;
  }
#elif DISABLE_WLAN_ONLY
    printf("the mode: wlan is disabled!\n");
	return;
#endif

  adjust_func_state();
  if (g_iscardin == FALSE)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    g_isclickinginprogress = FALSE;
    printf(" call the function on_wlan_active, return for g_iscardin=FALSE\n");
    return;
  }

  if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
  {
    printf("\nWLAN menu have been selected already , out !\n");
    g_isclickinginprogress = FALSE;
    return;
  }
  usleep(500000);

  if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE || g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    if (FALSE == unplugdevices())
    {
      g_isclickinginprogress = FALSE;
      return;
    }
    wait_device_unplug_stable();
  }
  write_enum_mode(2);
  if (g_downfromfile == FALSE)
    tdsp_download_dsp_code(2);
  else
    tdsp_download_dsp_code_from_file(2);
  usleep(500000);
  tdsp_add_device(TDSPWLANID, sizeof(TDSPWLANID), TDSP_SERIAL_NO_WLAN);
  usleep(1000000);

  if (tdsp_is_download_dspcode_ok() == FALSE)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    adjust_func_state();
    g_isclickinginprogress = FALSE;
    return;
  }

  g_state = BLUETOOTH_DISABLE_WLAN_ENABLE;
  g_isclickinginprogress = FALSE;
  return;
}

/* 
// To coexist, bt first:
void on_coexist_activate()
{

  if (g_isclickinginprogress != FALSE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;

  debug("+ call funtion: on_coexist_active:\n");
  if (g_allowedmodes[2] == 0)
  {
    printf("the mode: combo is disabled!\n");
    return;
  }

  adjust_func_state();
  if (!g_iscardin)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    g_isclickinginprogress = FALSE;
    printf("on_coexist_active, return for nocardin\n");
    return;
  }
  if (g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    g_isclickinginprogress = FALSE;
    printf("on_coexist_active, return for its current state also is COEXIST\n");
    return;
  }

  usleep(500000);

  if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE|| g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
  {
    if (FALSE == unplugdevices())
    {
      g_isclickinginprogress = FALSE;
      printf("on_coexist_active, return for unplugdevices fail\n");
      return;
    }
    wait_device_unplug_stable();
  }

  write_enum_mode(3);
  if (g_downfromfile == FALSE)
    tdsp_download_dsp_code(1);
  else
    tdsp_download_dsp_code_from_file(1);
  usleep(500000);
  tdsp_add_device(TDSPBTID, sizeof(TDSPBTID), TDSP_SERIAL_NO_BT);
  usleep(1000000);
  if (tdsp_is_download_dspcode_ok() == FALSE)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    adjust_func_state();
    g_isclickinginprogress = FALSE;
    printf("on_coexist_active, return for download_dspcode fail\n");
    return;
  }

  while (1)
  {
    tdsp_query_pnp_state(1);
    if (g_dsppnpstate != Started)
    {
      tdsp_query_power_state();
      if (g_buspowerstate != PowerOn)
      {
        g_isclickinginprogress = FALSE;
        printf("on_coexist_active, return for power fail\n");
        return;
      }
      usleep(500000);
      continue;
    }
    else
    {
      break;
    }
  }

  tdsp_set_init_flag(1);

  usleep(500000);
  if (g_downfromfile == FALSE)
    tdsp_download_dsp_code(2);
  else
    tdsp_download_dsp_code_from_file(2);
  usleep(500000);
  tdsp_add_device(TDSPWLANID, sizeof(TDSPWLANID), TDSP_SERIAL_NO_WLAN);
  usleep(1000000);

  if (tdsp_is_download_dspcode_ok() == FALSE)
  {
    g_isclickinginprogress = FALSE;
    printf("on_coexist_active, return for power fail\n");
    return;
  }
  g_state = BLUETOOTH_ENABLE_WLAN_ENABLE;
  g_isclickinginprogress = FALSE;
  init_tdsp_otherenv(TDSPSHELLFILEFORBT);
  return;
}

*/

// To coexist, WLAN first:
void on_coexist_activate()
{

  if (g_isclickinginprogress != FALSE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;

  debug("+ call funtion: on_coexist_active:\n");
  
#ifndef DISABLE_CONFIG_FILE
  if (g_allowedmodes[2] == 0)
  {
    printf("the mode: combo is disabled!\n");
    return;
  }
#elif DISABLE_COMBO
    printf("the mode: combo is disabled!\n");
	return;
#endif

  adjust_func_state();
  if (!g_iscardin)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    g_isclickinginprogress = FALSE;
    printf("on_coexist_active, return for nocardin\n");
    return;
  }
  if (g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    g_isclickinginprogress = FALSE;
    printf("on_coexist_active, return for its current state also is COEXIST\n");
    return;
  }

  usleep(500000);

  if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE|| g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
  {
    if (FALSE == unplugdevices())
    {
      g_isclickinginprogress = FALSE;
      printf("on_coexist_active, return for unplugdevices fail\n");
      return;
    }
    wait_device_unplug_stable();
  }

  write_enum_mode(3);

  if (g_downfromfile == FALSE)
    tdsp_download_dsp_code(2);
  else
    tdsp_download_dsp_code_from_file(2);
  usleep(200000);
  tdsp_add_device(TDSPWLANID, sizeof(TDSPWLANID), TDSP_SERIAL_NO_WLAN);
  usleep(1000000);

  if (tdsp_is_download_dspcode_ok() == FALSE)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    adjust_func_state();
    g_isclickinginprogress = FALSE;
    printf("on_coexist_active, return for download_dspcode fail\n");
    return;
  }

  while (1)
  {
    tdsp_query_pnp_state(2);
    if (g_dsppnpstate != Started)
    {
      tdsp_query_power_state();
      if (g_buspowerstate != PowerOn)
      {
        g_isclickinginprogress = FALSE;
        printf("on_coexist_active, return for power fail\n");
        return;
      }
      usleep(500000);
      continue;
    }
    else
    {
      break;
    }
  }

  tdsp_set_init_flag(2);
  usleep(500000);
  
  if (g_downfromfile == FALSE)
    tdsp_download_dsp_code(1);
  else
    tdsp_download_dsp_code_from_file(1);
  usleep(200000);
  tdsp_add_device(TDSPBTID, sizeof(TDSPBTID), TDSP_SERIAL_NO_BT);
  usleep(1000000);
  
  if (tdsp_is_download_dspcode_ok() == FALSE)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    adjust_func_state();
    g_isclickinginprogress = FALSE;
    printf("on_coexist_active, return for download_dspcode fail\n");
    return;
  }
  tdsp_set_init_flag(1);
  
  g_state = BLUETOOTH_ENABLE_WLAN_ENABLE;
  g_isclickinginprogress = FALSE;
  init_tdsp_otherenv(TDSPSHELLFILEFORBT);
  return;
}

void on_unplug_activate()
{

  if (g_isclickinginprogress != FALSE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;

  debug("+ call the function: on_unplug_active()\n");
  adjust_func_state();
  if (g_state == BLUETOOTH_DISABLE_WLAN_DISABLE)
  {
//      printf ("\nUnplug menu have been selected already, out!\n");
    g_isclickinginprogress = FALSE;
    return;
//      goto first_label;
  }
#if 0
#else
  if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE
      || g_state == BLUETOOTH_DISABLE_WLAN_ENABLE
      || g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    if (FALSE == unplugdevices())
    {
      debug("unplugdivice fail ! \n");
      g_isclickinginprogress = FALSE;
      return;
    }
    wait_device_unplug_stable();
  }
#endif

  g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
  g_isclickinginprogress = FALSE;
  return;
}

void on_about_activate()
{

#if 1

  update_ver_string();

#endif
}

void on_poweron_activate()
{

  logging("calling on on_poweron_active  ", __LINE__);

  tdsp_power_on();
}

void on_poweroff_activate()
{

  debug("calling on on_poweroff_active  ", __LINE__);

  tdsp_power_off();

}
