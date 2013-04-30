/*************************************************************************
 *
 *	(c) 2004-05, 3DSP Corporation, all rights reserved.  Duplication or
 *	reproduction of any part of this software (source code, object code or
 *	comments) without the expressed written consent by 3DSP Corporation is
 *	forbidden.  For further information please contact:
 *
 *	3DSP Corporation
 *	16271 Laguna Canyon Rd
 *	Irvine, CA 92618
 *	www.3dsp.com 
 *
 *************************************************************************
 *
 * [lwlan_ioctl.h] description:
 *		This header file contains some MACRO define (IOCTL) and some data-struct
 *		define used by IOCTL or SME.
 *  
 * exports:
 *
 * make'ing:
 *
 * TODO:
 * 
 * see also:
 *
 * This source to be viewed with tabspace 4 (":se ts=4" for ex/vi)
 *
 ************************************************************************/
#ifndef _LWLAN_IOCTL_H_
#define _LWLAN_IOCTL_H_

#ifndef IFNAMSIZ
#define IFNAMSIZ 32
#endif

extern const struct iw_handler_def _wlan_dev_iw_handler_def;

/* 3dsp private ioctl cmd */
#define PLACE_HOLDER_00			SIOCIWFIRSTPRIV		// just place holder

#define DSP_RESET					SIOCIWFIRSTPRIV+1
#define DSP_RESTART 				SIOCIWFIRSTPRIV+2

#define DSP_GET_PHY_MAC			SIOCIWFIRSTPRIV+3
#define DSP_SET_PHY_MAC 			SIOCIWFIRSTPRIV+4

#define PLACE_HOLDER_05			SIOCIWFIRSTPRIV+5
#define PLACE_HOLDER_06	   		SIOCIWFIRSTPRIV+6

#define PLACE_HOLDER_07			SIOCIWFIRSTPRIV+7
#define PLACE_HOLDER_08	   		SIOCIWFIRSTPRIV+8

#define PLACE_HOLDER_09			SIOCIWFIRSTPRIV+9	// just place holder
#define PLACE_HOLDER_10			SIOCIWFIRSTPRIV+10	// just place holder
#define PLACE_HOLDER_11			SIOCIWFIRSTPRIV+11	// just place holder
#define PLACE_HOLDER_12	   		SIOCIWFIRSTPRIV+12	// just place holder

#define DSP_CTLDEV_GET_BYTES		SIOCIWFIRSTPRIV+13
#define DSP_CTLDEV_SET_BYTES   		SIOCIWFIRSTPRIV+14

#define DSP_CTLDEV_GET_INT			SIOCIWFIRSTPRIV+15
#define DSP_CTLDEV_SET_INT   		SIOCIWFIRSTPRIV+16

#define DSP_CTLDEV_HELP			SIOCIWFIRSTPRIV+17

/* all macro defined bellow this are only for DATA dev */

#define DSP_SET_WEP_KEY			SIOCIWFIRSTPRIV+18
#define DSP_GET_WEP_KEY			SIOCIWFIRSTPRIV+19

#define DSP_SET_PEER_MAC   			SIOCIWFIRSTPRIV+20
#define DSP_GET_PEER_MAC   			SIOCIWFIRSTPRIV+21

#define PLACE_HOLDER_22 			SIOCIWFIRSTPRIV+22
#define DSP_GET_WPA_PARAM			SIOCIWFIRSTPRIV+23
#define PLACE_HOLDER_24			SIOCIWFIRSTPRIV+24	// just place holder
#define PLACE_HOLDER_25			SIOCIWFIRSTPRIV+25	// just place holder
#define PLACE_HOLDER_26 			SIOCIWFIRSTPRIV+26	// just place holder
#define PLACE_HOLDER_27			SIOCIWFIRSTPRIV+27	// just place holder
#define PLACE_HOLDER_28 			SIOCIWFIRSTPRIV+28	// just place holder

#define DSP_DATADEV_GETPARAM		SIOCIWFIRSTPRIV+29
#define DSP_DATADEV_SETPARAM   	SIOCIWFIRSTPRIV+30
#define DSP_DATADEV_HELP			SIOCIWFIRSTPRIV+31

enum {
	DSP_PARAM__DRIVER_VER = 1,			/* driver version */
	DSP_PARAM__DSP_VER = 2,				/* dsp version */
};


enum {
	DSP_PARAM__REG_DOMAIN,			/* reg domain */
	DSP_PARAM__TX_PWR,				/* tx power */
	DSP_PARAM__DBG_CMD,				/* debug commond */
};


enum {
	//DSP_PARAM__CTS_2_SELF = 1,			/* rts to self */
	//DSP_PARAM__BEACON = 2,				/* beacon interval */
	//DSP_PARAM__DTIM = 3,				/* dtim */
	DSP_PARAM__AUTH_MODE = 4,			/* authentication mode */
	DSP_PARAM__RNCRYPT_MODE = 5,		/* encrypt mode */
	DSP_PARAM__TX_POWER = 6,			/* tx power level */
	//DSP_PARAM__BROAD_SSID = 7,		/* broadcast ssid */
	//DSP_PARAM__DEBUG = 8,				/* debug code , print some register */
	//Spear add the type below @08/02/28
	DSP_PARAM__RSSI = 9,					/* receive signal strenth */
};

// It seemed that we can't define this macro bigger than 2048,	fdeng
#define HELP_BUF_LEN				2000		

void  iw_send_bssid_to_up(void *dev, unsigned char *bssid);

#endif//#ifndef _TDSP_IOCTL_H_
