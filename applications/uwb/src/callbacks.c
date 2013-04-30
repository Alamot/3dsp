#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <unistd.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "main.h"

gboolean g_bDirectlyFlag = 0;

void on_bluetooth_activate(GtkMenuItem * menuitem, gpointer user_data)
{
//  adjust_icon(0);
  if (g_ismenuclicked == FALSE)
  {
    THISFUNCBECALLED;
    debug("--------------menu not be click\n");
    return;
  }
  g_ismenuclicked = FALSE;

  if (g_isclickinginprogress == TRUE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;
  THISFUNCBECALLED;

  gray_wb_menu();
  adjust_menu_item(MODE_NULL);
  adjust_func_state();
  if (!g_iscardin)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    adjust_tooltip_text(BLUETOOTH_ENABLE_WLAN_DISABLE);
    debug("call on_bluetooth_active, return for the g_iscardin = FALSE\n");
    g_isclickinginprogress = FALSE;
    return;
  }
  if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE)
  {
    adjust_menu_item(MODE_BLUETOOTH);
    debug("call onbluetoothactive, return for the last state already is on_bluetooth\n");
    enable_wb_menu();
    adjust_tooltip_text(BLUETOOTH_ENABLE_WLAN_DISABLE);
    g_isclickinginprogress = FALSE;
    return;
  }

  if (g_bDirectlyFlag)
  {
    if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
    {
      if (FALSE == unplugdevices())
      {
        debug("unplugdivice fail ! \n");
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_ENABLE_WLAN_DISABLE);
        g_isclickinginprogress = FALSE;
        return;
      }
      wait_device_unplug_stable();
    }

    if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE || g_state == BLUETOOTH_DISABLE_WLAN_DISABLE)
    {
      tdsp_download_dsp_code(1);
      usleep(500000);
      tdsp_add_device(TDSPBTID, sizeof(TDSPBTID), TDSP_SERIAL_NO_BT);
      usleep(500000);

      if (tdsp_is_download_dspcode_ok() != TRUE)
      {
        adjust_func_state();
        adjust_menu_item(MODE_UNPLUG);
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_ENABLE_WLAN_DISABLE);
        g_isclickinginprogress = FALSE;
        return;
      }
      g_state = BLUETOOTH_ENABLE_WLAN_DISABLE;
    }
    if (g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
    {
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
      debug("--g_busresetflag = %ld,  and the loopcount iiTimes = %d\n", g_busresetflag, iiTimes);

      tdsp_remove_device(TDSP_SERIAL_NO_WLAN);
      usleep(500000);

      int iTimes = 0;
      do
      {
        tdsp_query_power_state();
        if (g_buspowerstate != PowerOn)
        {
          g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
          adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_DISABLE);
          g_isclickinginprogress = FALSE;
          g_message("return becaus g_buspowerstate != PowerOn\n");
          return;
        }
        tdsp_query_device();
        usleep(500000);
        if (g_currentserialno == 0x1)
          break;
        iTimes++;
      }
      while (g_currentserialno != 0x1 && iTimes < 600);
    }
  }
  else
  {

    if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE || g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
    {
      if (FALSE == unplugdevices())
      {
        debug("unplugdivice fail ! \n");
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_ENABLE_WLAN_DISABLE);
        g_isclickinginprogress = FALSE;
        return;
      }
      wait_device_unplug_stable();
    }

    write_enum_mode(1);
    tdsp_download_dsp_code(1);
    usleep(500000);
    tdsp_add_device(TDSPBTID, sizeof(TDSPBTID), TDSP_SERIAL_NO_BT);
    usleep(500000);
    if (tdsp_is_download_dspcode_ok() != TRUE)
    {
      g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
      adjust_func_state();
      adjust_menu_item(MODE_UNPLUG);
      enable_wb_menu();
      adjust_tooltip_text(BLUETOOTH_ENABLE_WLAN_DISABLE);
      g_isclickinginprogress = FALSE;
      return;
    }

  }

  g_state = BLUETOOTH_ENABLE_WLAN_DISABLE;
  adjust_tooltip_text(BLUETOOTH_ENABLE_WLAN_DISABLE);
  adjust_menu_item(MODE_BLUETOOTH);
  enable_wb_menu();
  refresh_gui_and_timeout();
  show_notification(_("uWB Notice"), _("Switch to Bluetooth state"), NULL, SWITCH_NOTIFY_TIME, NULL);
  call_shell_for_btEnv(TDSPSHELLFILEFORBT);

  g_isclickinginprogress = FALSE;
  return;
//  enable_blinking();
}

void on_wlan1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  if (g_ismenuclicked == FALSE)
  {
    THISFUNCBECALLED;
    debug("--------------menu not be click\n");
    return;
  }
  g_ismenuclicked = FALSE;
  if (g_isclickinginprogress == TRUE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;
  THISFUNCBECALLED;
  gray_wb_menu();
  adjust_menu_item(MODE_NULL);
  adjust_func_state();
  if (g_iscardin == FALSE)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    g_isclickinginprogress = FALSE;
    debug(" call the function on_wlan_active, return for g_iscardin=FALSE\n");
    enable_wb_menu();
    adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_ENABLE);
    adjust_menu_item(MODE_UNPLUG);
    return;
  }

  if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
  {
    debug("\nWLAN menu have been selected already , out ! menuitem = 0x%lx\n\n", (long) menuitem);
    adjust_menu_item(MODE_WLAN);
    enable_wb_menu();
    adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_ENABLE);
    g_isclickinginprogress = FALSE;
    return;
  }
  usleep(200000);
  if (g_bDirectlyFlag)
  {
    if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE)
    {
      if (FALSE == unplugdevices())
      {
        debug("unplugdivice fail ! \n");
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_ENABLE);
        g_isclickinginprogress = FALSE;
        return;
      }
      wait_device_unplug_stable();
    }

    if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE || g_state == BLUETOOTH_DISABLE_WLAN_DISABLE)
    {
      write_enum_mode(2);
      tdsp_download_dsp_code(2);
      usleep(500000);
      tdsp_add_device(TDSPWLANID, sizeof(TDSPWLANID), TDSP_SERIAL_NO_WLAN);
      usleep(500000);
      if (tdsp_is_download_dspcode_ok() != TRUE)
      {
        g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
        adjust_func_state();
        adjust_menu_item(MODE_UNPLUG);
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_ENABLE);
        g_isclickinginprogress = FALSE;
        return;
      }
      g_state = BLUETOOTH_DISABLE_WLAN_ENABLE;
    }

    if (g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
    {
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
      debug("--g_busresetflag = %ld,  and the loopcount iiTimes = %d\n", g_busresetflag, iiTimes);
      tdsp_remove_device(TDSP_SERIAL_NO_BT);
      usleep(500000);
      int iTimes = 0;
      do
      {
        tdsp_query_power_state();
        if (g_buspowerstate != PowerOn)
        {
          g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
          adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_DISABLE);
          g_isclickinginprogress = FALSE;
          g_message("return becaus g_buspowerstate != PowerOn\n");
          return;
        }
        tdsp_query_device();
        usleep(500000);
        if (g_currentserialno == 0x2)
          break;
        iTimes++;
      }
      while (g_currentserialno != 0x2 && iTimes < 600);
      write_enum_mode(2);
    }
  }
  else
  {
    if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE || g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
    {
      if (FALSE == unplugdevices())
      {
        g_isclickinginprogress = FALSE;
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_ENABLE);
        return;
      }
      wait_device_unplug_stable();
    }
    write_enum_mode(2);
    tdsp_download_dsp_code(2);
    usleep(500000);
    tdsp_add_device(TDSPWLANID, sizeof(TDSPWLANID), TDSP_SERIAL_NO_WLAN);
    usleep(1000000);
    if (tdsp_is_download_dspcode_ok() != TRUE)
    {
      g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
      adjust_func_state();
      adjust_menu_item(MODE_UNPLUG);
      enable_wb_menu();
      adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_ENABLE);
      g_isclickinginprogress = FALSE;
      return;
    }
  }

  g_state = BLUETOOTH_DISABLE_WLAN_ENABLE;
  adjust_menu_item(MODE_WLAN);
  adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_ENABLE);
  enable_wb_menu();
  refresh_gui_and_timeout();
  g_isclickinginprogress = FALSE;
  show_notification(_("uWB Notice"), _("Switch to WLAN state"), NULL, SWITCH_NOTIFY_TIME, NULL);
  return;
}

void on_coexist1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  if (g_ismenuclicked == FALSE)
  {
    THISFUNCBECALLED;
    debug("--------------menu not be click\n");
    return;
  }
  g_ismenuclicked = FALSE;
  if (g_isclickinginprogress == TRUE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;
  THISFUNCBECALLED;
  gray_wb_menu();
  adjust_menu_item(MODE_NULL);
  adjust_func_state();
  if (!g_iscardin)
  {
    g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
    g_isclickinginprogress = FALSE;
    debug("on_coexist_active, return for nocardin\n");
    enable_wb_menu();
    return;
  }
  if (g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    adjust_menu_item(MODE_COEXIST);
    g_isclickinginprogress = FALSE;
    debug("on_coexist_active, return for its current state also is COEXIST\n");
    enable_wb_menu();
    return;
  }
  usleep(200000);

  if (g_bDirectlyFlag)
  {
    if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE)
    {
      write_enum_mode(3);

      tdsp_download_dsp_code(2);
      usleep(500000);
      tdsp_set_init_flag(1);
      usleep(500000);
      tdsp_add_device(TDSPWLANID, sizeof(TDSPWLANID), TDSP_SERIAL_NO_WLAN);
      usleep(1000000);
      if (tdsp_is_download_dspcode_ok() != TRUE)
      {
        adjust_func_state();
        adjust_menu_item(MODE_UNPLUG);

        debug("on_coexist_active, return for download_dspcode fail\n");
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_DISABLE);
        g_isclickinginprogress = FALSE;
        return;
      }
    }
    if (g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
    {
      write_enum_mode(3);

      tdsp_download_dsp_code(1);
      usleep(500000);
      tdsp_set_init_flag(2);
      usleep(500000);
      tdsp_add_device(TDSPBTID, sizeof(TDSPBTID), TDSP_SERIAL_NO_BT);
      usleep(1000000);
      if (tdsp_is_download_dspcode_ok() != TRUE)
      {
        adjust_func_state();
        adjust_menu_item(MODE_UNPLUG);

        debug("on_coexist_active, return for download_dspcode fail\n");
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_DISABLE);
        g_isclickinginprogress = FALSE;
        return;
      }
    }

    if (g_state == BLUETOOTH_DISABLE_WLAN_DISABLE)
    {
      write_enum_mode(3);

      tdsp_download_dsp_code(2);
      usleep(200000);
      tdsp_add_device(TDSPWLANID, sizeof(TDSPWLANID), TDSP_SERIAL_NO_WLAN);

      usleep(1000000);
      if (tdsp_is_download_dspcode_ok() != TRUE)
      {
        adjust_func_state();
        adjust_menu_item(MODE_UNPLUG);

        debug("on_coexist_active, return for download_dspcode fail\n");
        enable_wb_menu();
        adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_DISABLE);
        g_isclickinginprogress = FALSE;
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
            debug("on_coexist_active, return for power fail\n");
            enable_wb_menu();
            g_isclickinginprogress = FALSE;
            return;
          }
          usleep(500000);
          continue;
        }
        else
          break;
      }

      tdsp_set_init_flag(2);
      usleep(500000);
      tdsp_download_dsp_code(1);
      usleep(200000);
      tdsp_add_device(TDSPBTID, sizeof(TDSPBTID), TDSP_SERIAL_NO_BT);
      usleep(1000000);
      if (tdsp_is_download_dspcode_ok() != TRUE)
      {
        g_isclickinginprogress = FALSE;
        debug("on_coexist_active, return for power fail\n");
        enable_wb_menu();
        return;
      }

    }

  }
  else
  {

    if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE || g_state == BLUETOOTH_DISABLE_WLAN_ENABLE)
    {
      if (FALSE == unplugdevices())
      {
        g_isclickinginprogress = FALSE;
        debug("on_coexist_active, return for unplugdevices fail\n");
        enable_wb_menu();
        return;
      }
      wait_device_unplug_stable();
    }
    write_enum_mode(3);
    tdsp_download_dsp_code(2);
    usleep(200000);
    tdsp_add_device(TDSPWLANID, sizeof(TDSPWLANID), TDSP_SERIAL_NO_WLAN);
    usleep(1000000);
    if (tdsp_is_download_dspcode_ok() != TRUE)
    {
      g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
      adjust_func_state();
      adjust_menu_item(MODE_UNPLUG);
      debug("on_coexist_active, return for download_dspcode fail\n");
      enable_wb_menu();
      adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_DISABLE);
      g_isclickinginprogress = FALSE;
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
          debug("on_coexist_active, return for power fail\n");
          enable_wb_menu();
          return;
        }
        usleep(500000);
        continue;
      }
      else
        break;
    }

    tdsp_set_init_flag(2);
    usleep(500000);
    tdsp_download_dsp_code(1);
    usleep(500000);
    tdsp_add_device(TDSPBTID, sizeof(TDSPBTID), TDSP_SERIAL_NO_BT);
    usleep(1000000);
    if (tdsp_is_download_dspcode_ok() != TRUE)
    {
      g_isclickinginprogress = FALSE;
      debug("on_coexist_active, return for power fail\n");
      enable_wb_menu();
      return;
    }

  }
  refresh_gui_and_timeout();
  g_state = BLUETOOTH_ENABLE_WLAN_ENABLE;
  adjust_tooltip_text(BLUETOOTH_ENABLE_WLAN_ENABLE);
  adjust_menu_item(MODE_COEXIST);
  enable_wb_menu();
  refresh_gui_and_timeout();

  show_notification(_("uWB Notice"), _("Switch to Coexist state"), NULL, SWITCH_NOTIFY_TIME, NULL);
  g_isclickinginprogress = FALSE;
  call_shell_for_btEnv(TDSPSHELLFILEFORBT);
  return;
}

void on_unplug1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  if (g_ismenuclicked == FALSE)
  {
    THISFUNCBECALLED;
    debug("--------------menu not be click\n");
    return;
  }
  g_ismenuclicked = FALSE;
  if (g_isclickinginprogress == TRUE)
  {
    debug("g_isclickinginprogress == TRUE");
    return;
  }
  g_isclickinginprogress = TRUE;
  debug("+ call the function: on_unplug_active()\n");
  gray_wb_menu();
  adjust_menu_item(MODE_NULL);
  adjust_func_state();
  if (g_state == BLUETOOTH_DISABLE_WLAN_DISABLE)
  {
    debug("\nUnplug menu have been selected already, out! menuitem = 0x%lx\n", (long) menuitem);
    adjust_menu_item(MODE_UNPLUG);
    enable_wb_menu();
    g_isclickinginprogress = FALSE;
    return;
  }

  if (g_state == BLUETOOTH_ENABLE_WLAN_DISABLE || g_state == BLUETOOTH_DISABLE_WLAN_ENABLE || g_state == BLUETOOTH_ENABLE_WLAN_ENABLE)
  {
    if (FALSE == unplugdevices())
    {
      debug("unplugdivice fail ! \n");
      enable_wb_menu();
      g_isclickinginprogress = FALSE;
      return;
    }
    wait_device_unplug_stable();
  }

  adjust_tooltip_text(BLUETOOTH_DISABLE_WLAN_DISABLE);
  adjust_menu_item(MODE_UNPLUG);
  g_state = BLUETOOTH_DISABLE_WLAN_DISABLE;
  enable_wb_menu();
  refresh_gui_and_timeout();
  g_isclickinginprogress = FALSE;
  show_notification(_("uWB Notice"), _("Switch to Unplug state"), NULL, SWITCH_NOTIFY_TIME, NULL);
//  show_notification(_("wbusb Notice"), _("switch to unplug state"), _("OK"), 0, NULL);
  return;
}

void on_about1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
//  enable_blinking();
//  adjust_icon(red);
  about_callback(NULL, NULL);
}

void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  show_notification(_("uWB Notice"), _("uWB Exit Normally"), NULL, 0, NULL);
  sleep(2);
  gtk_main_quit();
}
