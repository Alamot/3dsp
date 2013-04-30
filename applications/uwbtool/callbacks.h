#ifndef CALLBACK_H
  #define CALLBACK_H

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

extern int g_state;
extern int g_iscardin;
extern int g_allowedmodes[3];

void on_bluetooth_activate();

void on_wlan_activate();

void on_coexist_activate();

void on_unplug_activate();

void on_about_activate();

void on_exit_activate();

void on_poweron_activate();

void on_poweroff_activate();

#endif
