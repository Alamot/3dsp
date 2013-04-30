#ifndef CALLBACKS_H
#define CALLBACKS_H
/************************************************************************** 
 * FILENAME:     callback.h          
 * CREATE DATE:  2009/12/24 
 * PURPOSE:      the headfile of callback.c 
 *                ... 
 *  
 * AUTHORS:      3dsp <ritow.qi@3dsp.com.cn> 
 * 
 * NOTES:        description of constraints when using functions of this file 
 * 
 **************************************************************************/

#include <gtk/gtk.h>

void on_bluetooth_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_about1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_wlan1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_coexist1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_unplug1_activate(GtkMenuItem * menuitem, gpointer user_data);

#endif
