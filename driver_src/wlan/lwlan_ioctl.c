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
 * [lioctl.c] description:
 * 	This source file defines the ioctl(user IF) of wlan driver in linux.
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

#include <linux/version.h>
#include <linux/if_arp.h>
#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>

#include "lwlan_ioctl.h"
#include "lwlan_netdev.h"
#include "tdsp_memory.h"
#include "tdsp_string.h"
#include "usbwlan_lib.h"
#include "usbwlan_version.h"
//static char* TDSP_FILE_INDICATOR="IOTCL";

//#define DEBUG_OPEN // uncomment this line to enable debug
#ifdef DEBUG_OPEN
#define _ioctl_print(arg...)			printk(arg)
#else
#define	_ioctl_print(arg...)
#endif
 //Add Begin: jacqueline add for get scan result
#define ATH_DEFAULT_NOISE	-95	/* dBm */    

static inline
int _ioctl_channel_to_freq_a(int channel)
{
	return (5000 + (5 * channel));
}
static inline
int _ioctl_channel_to_freq_bg(int channel)
{
	int freq;

	if (channel == 14)
		freq = 2484;
	else
		freq = 2407 + (5 * channel);

	return freq;
}

static inline
int _ioctl_channel_to_freq(int channel)
{
	if (channel > 15)
	{
		return _ioctl_channel_to_freq_a(channel);
	}
	else
	{
		return _ioctl_channel_to_freq_bg(channel);
	}
}

static inline
int _ioctl_freq_to_channel_a(int freq)
{
	return ((freq - 5000) / 5);
}

static inline
int _ioctl_freq_to_channel_bg(int freq)
{
	unsigned char channel;

	if (freq == 2484)
		channel = 14;
	else
		channel = (freq - 2407) / 5;

	return channel;
}

static inline
int _ioctl_freq_to_channel( int freq)
{
	if (freq > 5000)
	{
		return _ioctl_freq_to_channel_a(freq);
	}
	else
	{
		return _ioctl_freq_to_channel_bg(freq);
	}
}


/*
 * The RSSI values reported in the TX/RX descriptors in the driver are the SNR 
 * expressed in dBm. Thus 'rssi' is signal level above the noise floor in dBm.
 *
 * Noise is measured in dBm and is negative unless there is an unimaginable 
 * level of RF noise. 
 *
 * The signal level is noise + rssi.
 *
 * Note that the iw_quality values are 1 byte, and can be signed, unsigned or 
 * negative depending on context.
 *
 */
static inline void
set_quality(struct iw_quality *iq, unsigned int rssi, int noise)
{    
	iq->qual = (rssi + 2) *12 / 10 -112;
	iq->noise = noise; 
	iq->level = ((((int)rssi + noise) <= 0) ? ((int)rssi + noise) : 0);
	iq->updated = IW_QUAL_ALL_UPDATED;
#if WIRELESS_EXT >= 19
	iq->updated |= IW_QUAL_DBM;
#endif
}

/* this func is used to get 11 protocol name that verifies the presence of Wireless Extensions */
int _ioctl_iw_get_name(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	void * pAdap = lwlan_get_netdev_priv(net_dev);
	unsigned char dot_type;
	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
//    _ioctl_print("%s: =>\n", __FUNCTION__);

	dot_type = Adap_Get_DotType(pAdap);
	switch(dot_type)
	{
		case IEEE802_11_A: 
			tdsp_strncpy(data->name, "802.11 a", IFNAMSIZ);
			break;
		case IEEE802_11_B:
			tdsp_strncpy(data->name, "802.11 b", IFNAMSIZ);
			break;
		case IEEE802_11_G:
			tdsp_strncpy(data->name, "802.11 g", IFNAMSIZ);
			break;	
		default: 
			_ioctl_print(" %s: Unrecognise 802.11 MODE, 11a ? 11b ? 11g? 11b+g?\n",__FUNCTION__);
			return -ENOTSUPP;
	}
	
	return 0;
	
}


/*1. If you are in managed mode, SIOCSIWFREQ usually should return
	  an error, because it is not allowed.
	  2. If you are in ad-hoc mode, SIOCSIWFREQ is used if a IBSS needs
	  to be created. After scanning is done, if you did not find any
	  network, you will create your own, and you will use the frequency that
	  was set. You should never return an error.
      3. If you are in Master mode... Well, this case is obvious ;-)*/

int _ioctl_iw_set_freq(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	void * pAdap = lwlan_get_netdev_priv(net_dev);
//	Uint flags;
	int channel;
	char expon;
	int  freq;
//	int err = -EINVAL;

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
	_ioctl_print("%s: =>\n", __FUNCTION__);

	// only supported in IBSS mode
	if (WLAN_MACMODE_IBSS_STA != Adap_Get_Mode(pAdap))
	{
		return 0;
	}


	
	if(	(data->freq.e == 0) 
	&&	(data->freq.m >= 0) 
	&&	(data->freq.m <= 1000)) 
	{
		channel = data->freq.m;
		freq = _ioctl_channel_to_freq( channel);
	}
	else
	{
		// copy from linux kernel.
		// we scale down to MHz then scale up to kHz
		// because we don't know the datile of data->freq structure;
		freq = data->freq.m;
		expon = 6 - data->freq.e;
		while (--expon >= 0)    /* scale down the frequency to MHz */
		{
			freq /= 10;
		}
		
		if(freq <= 1000)
		{
			return -EINVAL;
		}
		
		channel = _ioctl_freq_to_channel( freq);
	}

	freq*=1000;	 /* scale up the frequency to kHz, Because XP code use this unit*/

	_ioctl_print("_ioctl_iw_set_freq: freq=%u, channel=%u.\n",freq,channel);

    if(Adap_Set_Defult_Channel(pAdap, channel))
    {
        _ioctl_print("_ioctl_iw_set_freq: freq=%u, channel=%u not allowed.\n",freq,channel);
		return -EOPNOTSUPP;
    }
	return 0;
}



/*_ioctl_iw_get_freq: get freqency
 * if in IBSS mode, return the frequency IBSS used
 * if in BSS mode, only return freq station used when scan is doing/done
 */
int _ioctl_iw_get_freq(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	void * pAdap = lwlan_get_netdev_priv(net_dev);
	unsigned char channel;

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
//	_ioctl_print("%s: =>\n", __FUNCTION__);


    channel = Adap_Get_Channel(pAdap);

	data->freq.e = 1;
	data->freq.m = _ioctl_channel_to_freq( channel) * 100000;
	data->freq.flags = 1;

	return 0;
}


int _ioctl_iw_set_bss_type(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	void * pAdap = lwlan_get_netdev_priv(net_dev);

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
	
	if(1 == Adap_Has_Joined(pAdap))
	{
		return 0;
	}
	
	_ioctl_print("%s: =>\n", __FUNCTION__);

	switch(data->mode)
	{
		case IW_MODE_ADHOC:
			Adap_Set_Mode(pAdap,WLAN_MACMODE_IBSS_STA);
			_ioctl_print("_ioctl_iw_set_mode: IBSS mode.\n");
			break;

		case IW_MODE_INFRA:
		case IW_MODE_AUTO:
			Adap_Set_Mode(pAdap,WLAN_MACMODE_ESS_STA);
			_ioctl_print( "_ioctl_iw_set_mode: INFRA mode.\n");
			break;

		default:
			_ioctl_print("_ioctl_iw_set_mode: mode not supported!!!\n");
			return -EOPNOTSUPP;			
	}
	return 0;
}



int _ioctl_iw_get_bss_type(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	void * pAdap = lwlan_get_netdev_priv(net_dev);

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	if(Adap_Get_Mode(pAdap) == WLAN_MACMODE_IBSS_STA)
	{
		data->mode = IW_MODE_ADHOC ;
		_ioctl_print("_ioctl_iw_get_mode: mode = 2(ADHOC)\n");
	}
	else 
	{
		data->mode = IW_MODE_INFRA;
		_ioctl_print( "_ioctl_iw_get_mode: mode = 1(INFRA)\n");
	}		

	return 0;
}



int _ioctl_iw_get_range(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	void * pAdap = lwlan_get_netdev_priv(net_dev);
	struct iw_range *range = (struct iw_range *) extra;

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	

	data->data.length = sizeof(struct iw_range);
	tdsp_memset(range, 0, sizeof(struct iw_range));
	
	range->min_nwid = 0x0000;
	range->max_nwid = 0x0000;

	//get channel numbers accroding to domain region(initialized in sme_init())
	range->num_channels = Adap_Get_Channel_Num(pAdap);
	range->num_frequency = range->num_channels;

	range->max_qual.qual = 100;
	range->max_qual.level = 100;
	range->max_qual.noise = 0;
	range->max_qual.updated = IW_QUAL_NOISE_INVALID;

	/* FIXME: Needs still to be tuned. */
	range->avg_qual.qual = 50;
	range->avg_qual.level = 50;
	range->avg_qual.noise = 0;
	range->avg_qual.updated = IW_QUAL_NOISE_INVALID;

	range->sensitivity = 0;

	range->bitrate[0] =  1000000;
	range->bitrate[1] =  2000000;
	range->bitrate[2] =  5500000;
	range->bitrate[3] =  11000000;
	range->bitrate[4] =  6000000;
	range->bitrate[5] =  9000000;
	range->bitrate[6] =  12000000;
	range->bitrate[7] =  18000000;
	range->bitrate[8] =  24000000;
	range->bitrate[9] =  36000000;
	range->bitrate[10] = 48000000;
	range->bitrate[11] = 54000000;
	range->num_bitrates = 12;
	
	range->retry_capa = IW_RETRY_LIMIT ;
	range->retry_flags = IW_RETRY_LIMIT;

	range->min_retry = 1;
	range->max_retry = 65535;
	range->min_rts = 256;
	range->max_rts = 2347;
	range->min_frag = 256;
	range->max_frag = 2346;
	

	range->encoding_size[0] = 5;
	range->encoding_size[1] = 13;
	range->num_encoding_sizes = 2;
	range->max_encoding_tokens = 4;	//WEP_KEYS

#if WIRELESS_EXT > 15		// fdeng@080903,	2.4.x linux doesn't support this!
	range->enc_capa = IW_ENC_CAPA_WPA |  IW_ENC_CAPA_WPA2 |
			  IW_ENC_CAPA_CIPHER_TKIP | IW_ENC_CAPA_CIPHER_CCMP;
	// driver doesn't scan properly range->scan_capa = IW_SCAN_CAPA_ESSID;

#endif	

	range->pmp_flags = IW_POWER_ON;
	range->pmt_flags = IW_POWER_ON;
	range->pm_capa = 0;

	range->we_version_source = WIRELESS_EXT;
	range->we_version_compiled = WIRELESS_EXT;
	
	range->r_time_flags = 0;

	return 0;
}


//this iwconfig function trigger scanning
int _ioctl_iw_start_scan(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);

    return Adap_Start_Scan(pAdap);
}


#ifndef IWEVGENIE
 //Modify Begin: jacqueline add for get scan result,2008/08/12
 #if WIRELESS_EXT > 14
/*
 * Encode a WPA or RSN information element as a custom
 * element using the hostap format.
 */
static unsigned int
encode_ie(void *buf,  unsigned int bufsize, const unsigned char *ie,  unsigned int ielen,
	const char *leader,  unsigned int leader_len)
{
	unsigned char *p;
	int i;

	if (bufsize < leader_len)
		return 0;
	
	p = buf;
	tdsp_memcpy(p, leader, leader_len);
	bufsize -= leader_len;
	p += leader_len;
	
	for (i = 0; i < ielen && bufsize > 2; i++) {
		p += sprintf(p, "%02x", ie[i]);
		bufsize -= 2;
	}
	
	return (i == ielen ? p - (unsigned char *)buf : 0);
}
#endif /* WIRELESS_EXT > 14 */
#endif



#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#define _iwe_stream_add_event(stream, ends, iwe, event_len)	iwe_stream_add_event(info, stream, ends, iwe, event_len)
#define _iwe_stream_add_point(stream, ends, iwe, extra)		iwe_stream_add_point(info, stream, ends, iwe, extra)
#define _iwe_stream_add_value(event, value, ends, iwe, event_len)	iwe_stream_add_value(info, event, value, ends, iwe, event_len)
static int
_ioctl_sub_get_scan_res(void* pAdap,unsigned char * beg, unsigned char *  end, unsigned int *  p_actual_len, struct iw_request_info *info)
#else
#define _iwe_stream_add_event(stream, ends, iwe, event_len)		iwe_stream_add_event(stream, ends, iwe, event_len)
#define _iwe_stream_add_point(stream, ends, iwe, extra)			iwe_stream_add_point(stream, ends, iwe, extra)
#define _iwe_stream_add_value(event, value, ends, iwe, event_len)	iwe_stream_add_value(event, value, ends, iwe, event_len)
static int
_ioctl_sub_get_scan_res(void* pAdap,unsigned char *  beg, unsigned char *  end, unsigned int *  p_actual_len)
#endif
{
    PSCAN_RES scan_result;
	PCAPABILITY_T	p_capability;
	unsigned char *			cur_data = beg;
	unsigned char *			cur_val;
	unsigned char *			last_data;
	unsigned int			i,j,count;
    //ULONG tmpieslen;
	int			err = 0;
	struct iw_event	iwe;
	unsigned char			rate;
	//LONG lTmpRssi;
	
#if WIRELESS_EXT > 14
	#define MAX_IE_LENGTH 64 * 2 + 30
    unsigned short  ie_length;
	unsigned char buf[MAX_IE_LENGTH];
	#ifndef IWEVGENIE
		static const char rsn_leader[] = "rsn_ie=";
		static const char wpa_leader[] = "wpa_ie=";
	#endif
#endif

	_ioctl_print("%s: =>\n", __FUNCTION__);

    Adap_Get_Scan_Result(pAdap,&count,&scan_result);

	for (i = 0; i < count; i++)
	{
		if(cur_data >= end)
		{
			err = -E2BIG;
			break;
		}

		p_capability = (PCAPABILITY_T)&scan_result[i].cap;
		// BSSID
		tdsp_memset(&iwe, 0, sizeof(iwe));
		last_data = cur_data;
		iwe.cmd = SIOCGIWAP;
		iwe.u.ap_addr.sa_family = ARPHRD_ETHER;	
		tdsp_memcpy(iwe.u.ap_addr.sa_data, scan_result[i].bssid,6);		
		cur_data = _iwe_stream_add_event(cur_data, end, &iwe, IW_EV_ADDR_LEN);

		if(cur_data == last_data)
		{
			err = -E2BIG;
			break;
		}


		//SSID
		tdsp_memset(&iwe, 0, sizeof(iwe));
		last_data = cur_data;
		iwe.cmd = SIOCGIWESSID;
		iwe.u.data.flags = 1;
	    iwe.u.data.length = scan_result[i].ssid[0];
	    cur_data = _iwe_stream_add_point(cur_data,end, &iwe, (char *)(&scan_result[i].ssid[1]));
	
		if(cur_data == last_data)
		{
			err = -E2BIG;
			break;
		}

		//add mode(BSS/IBSS/ESS) info
		if(	p_capability->ess
		||	p_capability->ibss)  //ESS
       	{
			tdsp_memset(&iwe, 0, sizeof(iwe));
			last_data = cur_data;
			iwe.cmd = SIOCGIWMODE;
			iwe.u.mode = p_capability->ess ?IW_MODE_MASTER : IW_MODE_ADHOC;
			cur_data = _iwe_stream_add_event(cur_data, end, &iwe, IW_EV_UINT_LEN);
			
			if(cur_data == last_data)
			{
				err = -E2BIG;
				break;
			}
		}

		//add protocol info 11a/b/g
		tdsp_memset(&iwe, 0, sizeof(iwe));
		last_data = cur_data;
		iwe.cmd = SIOCGIWNAME;
		snprintf(iwe.u.name, IFNAMSIZ, "IEEE 802.11%c",  scan_result[i].is80211g == 1 ? 'g':'b');
		cur_data = _iwe_stream_add_event(cur_data, end, &iwe, IW_EV_CHAR_LEN);
		
		if(cur_data == last_data)
		{
			err = -E2BIG;
			break;
		}		


	       //add freq info
		tdsp_memset(&iwe, 0, sizeof(iwe));
		last_data = cur_data;
		iwe.cmd = SIOCGIWFREQ;
		iwe.u.freq.m = _ioctl_channel_to_freq(scan_result[i].channel) * 100000;
		iwe.u.freq.e = 1;
		cur_data = _iwe_stream_add_event(cur_data,end, &iwe, IW_EV_FREQ_LEN);		
		
		if(cur_data == last_data)
		{
			err = -E2BIG;
			break;
		}		
		
		//add quality info
		tdsp_memset(&iwe, 0, sizeof(iwe));
		last_data = cur_data;
		iwe.cmd = IWEVQUAL;
		set_quality(&iwe.u.qual, scan_result[i].rssi, ATH_DEFAULT_NOISE);
		cur_data = _iwe_stream_add_event(cur_data,end, &iwe, IW_EV_QUAL_LEN);

		if(cur_data == last_data)
		{
			err = -E2BIG;
			break;
		}		

		//add encode 
		tdsp_memset(&iwe, 0, sizeof(iwe));
		last_data = cur_data;
		iwe.cmd = SIOCGIWENCODE;
		
		if (p_capability->privacy)
		{
			iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		}
		else
		{
			iwe.u.data.flags = IW_ENCODE_DISABLED;
		}
		
		iwe.u.data.length = 0;
		cur_data = _iwe_stream_add_point(cur_data, end, &iwe, "");
		
		if(cur_data == last_data)
		{
			err = -E2BIG;
			break;
		}	

		//add rate info
		tdsp_memset(&iwe, 0, sizeof(iwe));
		last_data = cur_data;
		iwe.cmd = SIOCGIWRATE;
		cur_val = cur_data + IW_EV_LCP_LEN;
		/* NB: not sorted, does it matter? */
		for (j = 0; j < scan_result[i].suprate[0]; j++)
		{
			rate =scan_result[i].suprate[j+1] & 0x7f;
			if (rate != 0)
			{
				iwe.u.bitrate.value = rate * (1000000 / 2);
				cur_val = _iwe_stream_add_value(cur_data,cur_val, end, &iwe,IW_EV_PARAM_LEN);
			}
		}			
		
		/* remove fixed header if no rates were added */
		if ((cur_val - cur_data) > IW_EV_LCP_LEN)
		{
			cur_data = cur_val;
		} 
		else
		{
			/* We ran out of space in the buffer. */
			if (last_data == cur_data)
			{
				err = -E2BIG;
				break;
			}
		}

#if WIRELESS_EXT > 14

		//add beacon interval
		tdsp_memset(&iwe, 0, sizeof(iwe));
		last_data = cur_data;
		iwe.cmd = IWEVCUSTOM;
		snprintf(buf, sizeof(buf), "Beacon Interval:%u", scan_result[i].beaconinterval);
		iwe.u.data.length = tdsp_strlen(buf);
		cur_data = _iwe_stream_add_point(cur_data, end, &iwe, buf);

		if(cur_data == last_data)
		{
			err = -E2BIG;
			break;
		}	
 
	 #if 1//#ifdef DSP_WPA2
		//add RSN(WPA2) info
		if (scan_result[i].offset_wpa2 != 0xffff)
		{
			tdsp_memset(&iwe, 0, sizeof(iwe));
			last_data = cur_data;
			
			ie_length = *((unsigned char *)(&scan_result[i].IEs[scan_result[i].offset_wpa2 +1]));
				
		#ifdef IWEVGENIE		    
			if ((ie_length  + 2) > MAX_IE_LENGTH)
			{
				err = -E2BIG;
				break;
			}

			tdsp_memcpy(buf, &scan_result[i].IEs[scan_result[i].offset_wpa2], ie_length + 2);
			
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = ie_length + 2;
		#else	
			iwe.cmd = IWEVCUSTOM;
			iwe.u.data.length = encode_ie(	buf, sizeof(buf),&scan_result[i].IEs[scan_result[i].offset_wpa2], 
										ie_length + 2,rsn_leader, sizeof(rsn_leader) - 1);
		#endif

			if (iwe.u.data.length != 0)
			{
				cur_data = _iwe_stream_add_point(cur_data, end,&iwe, buf);

				if(cur_data == last_data)
				{
					err = -E2BIG;
					break;
				}	
			}
		}

		//add WPA info
		if (scan_result[i].offset_wpa != 0xffff)
		{
			tdsp_memset(&iwe, 0, sizeof(iwe));
			last_data = cur_data;
			
			ie_length = *((unsigned char *)(&scan_result[i].IEs[scan_result[i].offset_wpa+1]));

		#ifdef IWEVGENIE
			if ((ie_length  + 2) > MAX_IE_LENGTH)
			{
				err = -E2BIG;
				break;
			}

			tdsp_memcpy(buf, &scan_result[i].IEs[scan_result[i].offset_wpa], ie_length + 2);
			
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = ie_length + 2;
		#else
			iwe.cmd = IWEVCUSTOM;
			iwe.u.data.length = encode_ie(	buf, sizeof(buf),&scan_result[i].IEs[scan_result[i].offset_wpa], 
										ie_length + 2,wpa_leader, sizeof(wpa_leader) - 1);
		#endif

			if (iwe.u.data.length != 0)
			{
				cur_data = _iwe_stream_add_point(cur_data, end,&iwe, buf);

				if(cur_data == last_data)
				{
					err = -E2BIG;
					break;
				}	
			}
		}
	#endif //#ifdef ENCRYPTION	
	
#endif /* WIRELESS_EXT > 14 */
	}	
	
	*p_actual_len = cur_data - beg;
		
	//_ioctl_print("%s: scan return %d result,scan result is %d, err=%d\n", __FUNCTION__,i,count,err);

	return err;
}



//this iwconfig function gets scan results
int _ioctl_iw_get_scan_res(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	unsigned char *	end_buf;
	unsigned int	actual_len = 0;
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	int		res = 0;
	_ioctl_print("%s: =>\n", __FUNCTION__);

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	
	
	if (data->data.length == 0)
	{
		end_buf = (unsigned char *)extra + IW_SCAN_MAX_DATA;
	} 
	else
	{
		end_buf = (unsigned char *)extra + data->data.length;
	}
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
	res = _ioctl_sub_get_scan_res(pAdap, (unsigned char *)extra, end_buf, &actual_len, info);
#else
	res = _ioctl_sub_get_scan_res(pAdap, (unsigned char *)extra, end_buf, &actual_len);
#endif

	data->data.length = actual_len;
	
	return res;
}


//set ap addr and join it
//not support such operation, use set essid.

#if 0
int _ioctl_iw_set_ap(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	PDSP_ADAPTER_T pAdap = (PDSP_ADAPTER_T)lwlan_get_netdev_priv(net_dev);

	static const Uchar any[] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};
	static const Uchar off[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	DBG_WLAN__IOCTL(LEVEL_TRACE, "%s: =>\n", __FUNCTION__);
	
	// sanity checks
	if (wrqu->ap_addr.sa_family != ARPHRD_ETHER)
		return -EINVAL;

	//mutex_lock(&priv->action_mutex);

	if (!tdsp_memcmp(any, wrqu->ap_addr.sa_data, ETH_ALEN) ||
	    !tdsp_memcmp(off, wrqu->ap_addr.sa_data, ETH_ALEN)) {
		/* we disable mandatory BSSID association */
		DBG_WLAN__IOCTL(LEVEL_TRACE,"exit - disable mandatory BSSID\n");
		return -1;
	}

	//make sync to protect sme state to change in proper order
	while(!_is_sme_ready_process_new_state(&(wlan_data_p->sme_obj)))
		tdsp_sleep(2);
	
	if(!sme_alloc_app_msg(wlan_data_p, &p))
	{
		DBG_WLAN__IOCTL(LEVEL_TRACE, "app_sme_q is full!\n");
		kd_assert(0);
		return -1;
	}
	
	DBG_WLAN__IOCTL(LEVEL_TRACE,"SET BSSID -> %02X:%02X:%02X:%02X:%02X:%02X\n",
		     wrqu->ap_addr.sa_data[0] & 0xff,
		     wrqu->ap_addr.sa_data[1] & 0xff,
		     wrqu->ap_addr.sa_data[2] & 0xff,
		     wrqu->ap_addr.sa_data[3] & 0xff,
		     wrqu->ap_addr.sa_data[4] & 0xff,
		     wrqu->ap_addr.sa_data[5] & 0xff);
	
	//form app request msg
	p->msg_type = APP_START_JOIN;
	tdsp_memcpy(p->data.bssid, wrqu->ap_addr.sa_data, ETH_ALEN);

	//notify sme app msg arrived
	wlan_data_p->sme_obj.app_msg_count++;
	tdsp_thread_resume(&(wlan_data_p->sme_msg_thread.thread_handle));

	tdsp_sem_wait(&(wlan_data_p->sme_obj.app_thread_sem));
	
	if(p->result == FALSE)
	{
		DBG_WLAN__IOCTL(LEVEL_TRACE,"required bssid is invalid!\n");
		DBG_WLAN__IOCTL(LEVEL_TRACE, "%s: <=\n", __FUNCTION__);
		return -1;
	}
	else
	{
		DBG_WLAN__IOCTL(LEVEL_TRACE, "%s: <=\n", __FUNCTION__);
		return 0;
	}
}
#endif

int _ioctl_iw_get_ap(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    unsigned char bssid[6];
    void * pAdap = lwlan_get_netdev_priv(net_dev);
//	_ioctl_print("%s: =>\n", __FUNCTION__);

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
	tdsp_memset(bssid, 0, 6);
    Adap_Get_BSSID(pAdap, bssid);

	tdsp_memcpy(data->ap_addr.sa_data, bssid,6);
	data->ap_addr.sa_family = ARPHRD_ETHER;

	return 0;
}


	
//Spear add end
//set the ssid to associate
int _ioctl_iw_set_essid(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
    unsigned char * p_ssid=(unsigned char *)extra;
	unsigned int	ssid_len = data->essid.flags ? data->essid.length : 0;

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
	
	if (data->essid.flags > 0)
	{
		if(	data->essid.length == 0
		||	data->essid.length > IW_ESSID_MAX_SIZE) 
		{
			_ioctl_print("Invalid Parameter.data->essid.length=%d.\n", data->essid.length);
			return -EINVAL;
		}
	}
#ifdef DEBUG_OPEN
	if (ssid_len < IW_ESSID_MAX_SIZE)
	{
		char		ssid_print[IW_ESSID_MAX_SIZE + 1];
		tdsp_memset(ssid_print, 0, sizeof(ssid_print));
		tdsp_memcpy(ssid_print, p_ssid,ssid_len);

		_ioctl_print("START JOIN. len = %d, SSID=\'%s\' \n",ssid_len, ssid_print);
	}
	else
	{
		_ioctl_print("START JOIN. SSID len = %d\n", ssid_len);
	}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)	
    ssid_len -= 1;
#endif
	if(0 != Adap_Set_SSID(pAdap, p_ssid, ssid_len))
	{
		_ioctl_print("^^^^Oid_SetSsid: return by tkip counter measure ^^^^^\n ");	
		return  -EPERM;
	}
	return 0;
}


//get the ssid associated
int _ioctl_iw_get_essid(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
    unsigned char   ssid[32];
    unsigned char  ssid_len;
    _ioctl_print("%s: =>\n", __FUNCTION__);

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	Adap_Get_SSID(pAdap, ssid, &ssid_len);

    if(ssid_len > 0 && ssid_len <= 32)
    {
        data->data.length  = ssid_len;
        data->data.flags = 1; 	//active
        tdsp_memcpy(extra,ssid, ssid_len);
    }

    else
    {
        data->data.length = 0;
    }
	return 0;
}



int _ioctl_iw_set_rts(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	    void * pAdap = lwlan_get_netdev_priv(net_dev);


	_ioctl_print(" #Input para# rts->value=<%d>;rts->disabled=<%d>;",
				data->rts.value, data->rts.disabled);
	
	

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		

	if(	data->rts.disabled
	||	data->rts.value == -1)
	{
        
        Adap_Set_Rts(pAdap, -1);
	}
	else if( data->rts.value < 256
		  || data->rts.value > 2347) 
	{
	    _ioctl_print("_ioctl_iw_set_rts, invalid value %d.\n",data->rts.value);			
	    return -EINVAL;
	}        
    else
    {
        Adap_Set_Rts(pAdap, data->rts.value);
    }
	return 0;
}


int _ioctl_iw_get_rts(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    RTS_DATA rts;
	void * pAdap = lwlan_get_netdev_priv(net_dev);

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
    Adap_Get_Rts(pAdap,&rts);	
    
	data->rts.disabled =rts.rts_disable;
	data->rts.value = rts.rts_value;
	
	return 0;
}

int _ioctl_iw_set_frag(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	void * pAdap = lwlan_get_netdev_priv(net_dev);

	_ioctl_print( " #Input para# frag->value=<%d>;frag->disabled=<%d>;",
				data->frag.value, data->frag.disabled);
	

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	
	if(	data->frag.disabled
	||	data->frag.value == -1)
	{
		Adap_Set_Frag(pAdap, -1);
	}
	else if( data->frag.value < 256
		 ||	data->frag.value > 2346) 
	{
			_ioctl_print( "_ioctl_iw_set_frag, invalid value %d.\n",data->frag.value);			
			return -EINVAL;
	}
    else
    {
        Adap_Set_Frag(pAdap, data->frag.value);
    }
	
	return 0;		
}


int _ioctl_iw_get_frag(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	void * pAdap = lwlan_get_netdev_priv(net_dev);

    FRAG_DATA frag;
	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	
	Adap_Get_Frag(pAdap,&frag);	
    
	data->frag.disabled = frag.frag_disable;
	data->frag.value    = frag.frag_value;
	
	return 0;
}

int _ioctl_iw_set_sens(struct net_device *net_dev,
						struct iw_request_info *info,
						struct iw_param *data, 
						char *extra) 
{
	return -EOPNOTSUPP;
}

int _ioctl_iw_get_sens(struct net_device *net_dev,
						struct iw_request_info *info,
						struct iw_param *data, 
						char *extra) 
{
	return -EOPNOTSUPP;
}


int _ioctl_iw_set_tx_pow(struct net_device *net_dev,
						struct iw_request_info *info,
						struct iw_param *data, 
						char *extra) 
{
	return -EOPNOTSUPP;
}

int _ioctl_iw_get_tx_pow(struct net_device *net_dev,
						struct iw_request_info *info,
						struct iw_param *data, 
						char *extra) 
{
	return -EOPNOTSUPP;
}




int _ioctl_iw_set_rate(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
    unsigned char fix;

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	
	if (data->bitrate.fixed)
	{//do not use auto rate.
		_ioctl_print("_ioctl_iw_set_rate fixed rate = %d\n",data->bitrate.value);
        fix = 1;
	}
	else
	{//User want to use auto rate.
		_ioctl_print("_ioctl_iw_set_rate unfixed rate = %d\n",data->bitrate.value);
        fix = 0;
	}
	
	Adap_Set_Rate(pAdap,data->bitrate.value,fix);

	return 0;
}



int _ioctl_iw_get_rate(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	RATE_DATA rate;

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	Adap_Get_Rate(pAdap,&rate);	
    
	data->bitrate.value    = rate.rate_value;
	data->bitrate.fixed    = rate.fixed;
    
//	_ioctl_print("%s: =>rate:%d.%d, fixed=%d.\n", __FUNCTION__,data->bitrate.value/1000000,(data->bitrate.value/100000)%10,data->bitrate.fixed);
	
	return 0;
}


//the status will be displayed in /proc/net/wireless
struct iw_statistics* _ioctl_iw_get_wireless_stats(struct net_device *net_dev)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	struct iw_statistics  *p_wstats = Adap_Get_Netstat(pAdap);

	if (0 == Adap_Has_Connected(pAdap))
	{
		p_wstats->miss.beacon = 0;
		p_wstats->discard.retries = 0;
		p_wstats->discard.nwid = 0;
		p_wstats->discard.code = 0;
		p_wstats->discard.fragment = 0;
		p_wstats->discard.misc = 0;
		p_wstats->qual.qual = 0;
		p_wstats->qual.level = 0;
		p_wstats->qual.noise = 0;
		p_wstats->qual.updated = 7;
		p_wstats->qual.updated |= IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
		return p_wstats;
	}
	
	// TODO: wumin: add more info here 
	p_wstats->qual.level = (Adap_Get_Rssi(pAdap)+2) * 12 / 10;;
	p_wstats->qual.qual = 0;
	p_wstats->qual.noise = 0;
	p_wstats->qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_DBM |IW_QUAL_LEVEL_INVALID|IW_QUAL_NOISE_INVALID;
	p_wstats->discard.code = 0;
	p_wstats->discard.retries = 0;
	p_wstats->discard.nwid = 0;
	p_wstats->discard.fragment = 0;
	p_wstats->discard.misc = 0;	// FIXME
	p_wstats->miss.beacon = 0;	// FIXME
	
	return p_wstats;
}



int _ioctl_iw_set_auth(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	unsigned char value;
    unsigned char mode;
	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
					
	_ioctl_print("%s: =>\n", __FUNCTION__);
	_ioctl_print("flags is %d,value is %d.\n",data->param.flags & IW_AUTH_INDEX, data->param.value);
	switch (data->param.flags & IW_AUTH_INDEX)
	{
		case IW_AUTH_WPA_VERSION:
             mode = AUTH_WPA_VERSION;
             if( IW_AUTH_WPA_VERSION_WPA == data->param.value)
             {
                value = WPA_VERSION_WPA;
             }
             else if( IW_AUTH_WPA_VERSION_WPA2 == data->param.value)
             {
                value = WPA_VERSION_WPA2;
             }
             else
             {
                value = WPA_VERSION_NONE;
             }
			break;	
            
		case IW_AUTH_CIPHER_PAIRWISE:
        case IW_AUTH_CIPHER_GROUP:
            if((IW_AUTH_CIPHER_WEP40 == data->param.value)
               || (IW_AUTH_CIPHER_WEP104 == data->param.value))
            {
                value = WEP_MODE_WEP;
            }
            else if(IW_AUTH_CIPHER_TKIP == data->param.value)
            {
                value = WEP_MODE_TKIP;
            }
            else if(IW_AUTH_CIPHER_CCMP == data->param.value)
            {
                value = WEP_MODE_AES;
            }
            else 
            {
                value = WEP_MODE_NONE;
            }
            mode = (IW_AUTH_CIPHER_PAIRWISE == (data->param.flags & IW_AUTH_INDEX)) ? AUTH_CIPHER_PAIRWISE : AUTH_CIPHER_GROUP;
			break;
			
		case IW_AUTH_KEY_MGMT:
            if(IW_AUTH_KEY_MGMT_802_1X == data->param.value)
			{
				value = KEY_MNG_1X;			//EAP		
			}
			else if(IW_AUTH_KEY_MGMT_PSK == data->param.value)
			{
				value  = KEY_MNG_PSK;			//PSK
			}
			else
			{
                value = KEY_MNG_NONE;  //NONE
            }
			mode = AUTH_KEY_MGMT;
			break;

		case IW_AUTH_80211_AUTH_ALG:
            if(IW_AUTH_ALG_OPEN_SYSTEM == data->param.value)
			{
				value  = AUTH_ALG_OPEN;				
			}
			else if(IW_AUTH_ALG_SHARED_KEY == data->param.value)
			{
				value  = AUTH_ALG_SHARED_KEY;
			}
            else
            {
                value = AUTH_ALG_NONE;
            }
            mode = AUTH_80211_AUTH_ALG;
         	break;

		case IW_AUTH_WPA_ENABLED:
		case IW_AUTH_TKIP_COUNTERMEASURES:
		case IW_AUTH_DROP_UNENCRYPTED:
		case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		case IW_AUTH_PRIVACY_INVOKED:
			_ioctl_print("%s: Set %d to %d.\n", __FUNCTION__,data->param.flags & IW_AUTH_INDEX, data->param.value);
            return 0;
		default:
			_ioctl_print("%s: Set %d to %d.\n", __FUNCTION__,data->param.flags & IW_AUTH_INDEX, data->param.value);
			return -EINVAL;
	}

	return Adap_Set_Auth(pAdap,mode, value);
}


static int _ioctl_iw_get_auth(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	struct iw_param 			*param = &data->param;	
	int						ret = 0;
	int  auth_alg;

	if(0 == Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
//	_ioctl_print("%s: =>\n", __FUNCTION__);

	switch (param->flags & IW_AUTH_INDEX) 
	{
		case IW_AUTH_CIPHER_PAIRWISE:
		case IW_AUTH_CIPHER_GROUP:
		case IW_AUTH_KEY_MGMT:
		case IW_AUTH_WPA_VERSION:
			/* wpa_supplicant will control these internally */
			ret = -EOPNOTSUPP;
			break;

		case IW_AUTH_TKIP_COUNTERMEASURES:
		case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		case IW_AUTH_ROAMING_CONTROL:
		case IW_AUTH_PRIVACY_INVOKED:
			break;

		case IW_AUTH_DROP_UNENCRYPTED:
			param->value = Adap_Is_Privacy_On(pAdap);
			break;

		case IW_AUTH_80211_AUTH_ALG:

            auth_alg = Adap_Get_Auth_Alg(pAdap);
			if(auth_alg == AUTH_ALG_OPEN)
			{
				param->value = IW_AUTH_ALG_OPEN_SYSTEM;
			}
			else if(auth_alg == AUTH_ALG_SHARED_KEY)
			{
				param->value = IW_AUTH_ALG_SHARED_KEY;			
			}
			else
			{	
				ret = -EINVAL;
			}
			
			break;

		case IW_AUTH_WPA_ENABLED:
			param->value = Adap_Get_Key_Mng(pAdap);
			break;

		default:
			ret =  -EOPNOTSUPP;
			break;
	}


	return ret;
}



/*
 * Input an string form wepkey and convert to binary form.
 */
int  _ioctl_key_string2hex(const char *string_buf, char *hex_buf, int max_len)
{
	unsigned int i;
	unsigned int val;
	unsigned char c ;
	const char *bufp = string_buf;
	
	if(0 == max_len) 
		return(1);
	
	for(i = 0; i < max_len; i++) {
		c = *bufp++;
		if ( c >= '0' && c <= '9') val = c - '0';
		else if (c >= 'a' && c <= 'f') val = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F') val = c - 'A' + 10;
		else  
        {   _ioctl_print("%s: the char is %d\n",__FUNCTION__,c);    
            break;
        }
		val <<= 4;
		c = *bufp++;
		if ( c >= '0' && c <= '9') val |= c - '0';
		else if (c >= 'a' && c <= 'f') val |= c - 'a' + 10;
		else if (c >= 'A' && c <= 'F') val |= c - 'A' + 10;
		else break;
     		
		hex_buf[i] = (unsigned char) (val & 0377);
	}

	if(i == max_len) 
		return(1);
	else 
		return(0);
	
	
}


//set wep keys
int _ioctl_iw_set_encode(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	struct iw_point *erq = &data->encoding;	
	unsigned char					index;
//	int						ret = 0;
	

	if(!Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	_ioctl_print("%s: => index:%u, disabled:%u, length:%u.\n!",__FUNCTION__,
		erq->flags & IW_ENCODE_INDEX, erq ->flags & IW_ENCODE_DISABLED,
		erq->length);

	//DBG_PRINT_BUFF("Content of Key:", extra, erq->length);
	
    index = erq->flags & IW_ENCODE_INDEX;
		
	if(index != 0)
	{
		if(index < 1 || index > 4)
		{
			_ioctl_print("%s: invalid key index: %d\n", __FUNCTION__, index);
			return -EINVAL;
		} 
		else
		{
			index--;	//key index: 0~3
		}
	}
	
	if (erq ->flags & IW_ENCODE_DISABLED)
	{
	    Adap_Disalbe_Encode(pAdap,index);
    }
    else
	{
        Adap_Set_Encode(pAdap, index, extra, erq->length);	
    }
	
	return 0;
}

    
      
int _ioctl_iw_get_encode(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	struct iw_point  *erq = &data->encoding;	
	unsigned char  index;
	WEP_KEY wep;

	if(!Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	
	index = erq->flags & IW_ENCODE_INDEX;
	
	if(index != 0)
	{
		if(index < 1 || index > 4)
		{
			_ioctl_print("%s: invalid key index: %d\n", __FUNCTION__, index);
			return -EINVAL;
		} 
		else
		{
			index--;	//key index: 0~3
		}
	}

    Adap_Get_Encode(pAdap, index, &wep);

    if(wep.len == 0)
    {
        erq->length = 0;
		erq->flags |= IW_ENCODE_DISABLED;
		return 0;
    }
	else
	{
		erq->length = wep.len;
        tdsp_memcpy(extra, wep.key, erq->length);
	    erq->flags |= IW_ENCODE_ENABLED;
	}
	
	return 0;	
}

int _ioctl_iw_set_encode_ext(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	struct iw_point 				*erq = &data->encoding;	
	struct iw_encode_ext 			*ext = (struct iw_encode_ext *)extra;
	unsigned char						key_buffer[64];
	PADD_KEY		p_add_key = (PADD_KEY)key_buffer;
	unsigned int index;
	

	if(!Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	_ioctl_print("%s: => flags=0x%04x, ext_flags=0x%08x,alg=%u\n",
		__FUNCTION__, erq->flags,ext->ext_flags,ext->alg);
//		ext->addr.sa_data[0], ext->addr.sa_data[1], ext->addr.sa_data[2], 
//		ext->addr.sa_data[3], ext->addr.sa_data[4], ext->addr.sa_data[5] );
//	DBG_PRINT_BUFF("ext->rx_seq",  ext->rx_seq ,IW_ENCODE_SEQ_MAX_SIZE);
//	DBG_PRINT_BUFF("ext->tx_seq",  ext->tx_seq ,IW_ENCODE_SEQ_MAX_SIZE);
//	DBG_PRINT_BUFF("ext->key", ext->key, ext->key_len);
	
	//tdsp_memset(key_buffer,0x00,64);
		/* Determine and validate the key index */

    tdsp_memset(key_buffer,0x00,64);
	index = erq->flags & IW_ENCODE_INDEX;


	if(index!=0)
	{
		if(index < 1 || index > 4)
		{
			_ioctl_print("%s: invalid key index: %d\n", __FUNCTION__, index);
			return -EINVAL;
		} 
		else
		{
			index--;	//key index: 0~3
		}
	}

	if(erq->flags & IW_ENCODE_DISABLED)  // Remove Key
	{
		_ioctl_print("%s: IW_ENCODE_DISABLED\n", __FUNCTION__);
		return Adap_Remove_Key_Ext(pAdap,index,ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY);
    }
		
	p_add_key->is_group_key = ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY?1:0;
	p_add_key->is_tx_key = ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY?1:0;
	p_add_key->keyRSC_valid = ext->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID?1:0;
	tdsp_memcpy(&p_add_key->KeyRSC,ext->rx_seq,sizeof(unsigned long long));
	p_add_key->KeyIndex  = index;
	p_add_key->KeyLength = ext->key_len;
	tdsp_memcpy(p_add_key->BSS_ID, ext->addr.sa_data, 6);


	switch (ext->alg)
	{		
	case IW_ENCODE_ALG_WEP:
		_ioctl_print("%s: encode alg is WEP\n", __FUNCTION__);
		p_add_key->wep_mode = WEP_MODE_WEP;
		break;

	case IW_ENCODE_ALG_TKIP:
		_ioctl_print("%s: encode alg is TKIP\n", __FUNCTION__);
		p_add_key->wep_mode = WEP_MODE_TKIP;
		break;

	case IW_ENCODE_ALG_CCMP:
		_ioctl_print( "%s: encode alg is CCMP\n", __FUNCTION__);
		p_add_key->wep_mode = WEP_MODE_AES;
		break;
		
	default:
		_ioctl_print("%s: encode alg %d is invalied when add key\n", __FUNCTION__, ext->alg);
		return -EINVAL;
	}
	
		
	if (	!p_add_key->is_group_key
	&&	(	(	index != 0 
			&&	ext->alg != IW_ENCODE_ALG_WEP)
		||	Adap_Get_Mode(pAdap) == WLAN_MACMODE_IBSS_STA))
	{
		_ioctl_print("%s: invalid combonation.\n", __FUNCTION__);
		return -EINVAL;
	}


	if(	!p_add_key->is_group_key
	&&	(	!p_add_key->is_tx_key
		||	index != 0))
	{
		_ioctl_print( "%s: invalid combonation.\n", __FUNCTION__);
		return -EINVAL;
	}

	if(	MAC_ADDR_IS_BCAST(p_add_key->BSS_ID) 
	&&	!p_add_key->is_group_key)
	{
		_ioctl_print("%s: invalid combonation.\n", __FUNCTION__);
		return -EINVAL;
	}

	
	switch(ext->key_len)
	{
		case 5:
		case 13:
		case 16:
		case 32:
			break;
		default:
			_ioctl_print( "%s: invalid key length when add key: %d\n", __FUNCTION__, ext->key_len);
			return -EINVAL;
	}
	
	tdsp_memcpy(p_add_key->KeyMaterial, ext->key, ext->key_len);
    return  Adap_Add_Key_Ext(pAdap, index, p_add_key);

}




int _ioctl_iw_get_encode_ext(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
    int 						key_len;
	unsigned char			    index;
	unsigned char			    wep_mode;
	unsigned char			    key_buf[16];
    void * pAdap = lwlan_get_netdev_priv(net_dev);
	struct iw_point 				*erq = &data->encoding;	
	struct iw_encode_ext 			*ext = (struct iw_encode_ext *)extra;
	
	if(!Adap_Driver_isWork(pAdap))
	{
		return -1;
	}
		
	
	key_len = erq->length - sizeof(*ext);
	if (key_len < 0)
	{
		return -EINVAL;
	}
	
	index = erq->flags & IW_ENCODE_INDEX;

	if(	((!ext->ext_flags) & IW_ENCODE_EXT_GROUP_KEY) 
	&&    (ext->alg != IW_ENCODE_ALG_WEP)
	&&	(	(index != 0) 
		||	(Adap_Get_Mode(pAdap) == WLAN_MACMODE_IBSS_STA)))
	{
		return -EINVAL;
	}
		
	erq->flags = (IW_ENCODE_INDEX & (index + 1));
	
	tdsp_memset(ext, 0, sizeof(*ext));

	if (0 == Adap_Is_Encode_Enabled(pAdap))
	{
		ext->alg = IW_ENCODE_ALG_NONE;
		ext->key_len = 0;
		erq->flags |= IW_ENCODE_DISABLED;
	}
	else
	{
	    wep_mode = Adap_Get_Encode_Mode(pAdap, ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY);

		if (wep_mode == WEP_MODE_WEP)////HJJHHJJJ
			ext->alg = IW_ENCODE_ALG_WEP;
		else if (wep_mode == WEP_MODE_TKIP)
			ext->alg = IW_ENCODE_ALG_TKIP;
		else if (wep_mode == WEP_MODE_AES)
			ext->alg = IW_ENCODE_ALG_CCMP;
		else
			return -EINVAL;

        key_len =  Adap_Get_Encode_Key(pAdap,index,key_buf);

        if(key_len < 0)
        {
            
            return -EINVAL;
        }
		
		ext->key_len = key_len;
		
		tdsp_memcpy(ext->key, key_buf, ext->key_len);
		
		erq->flags |= IW_ENCODE_ENABLED;
	}

	return 0;
}



#if 0

int ioctl_iw_get_name(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_name(net_dev,info,data,extra);
}

int ioctl_iw_set_freq(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_freq(net_dev,info,data,extra);
}

int ioctl_iw_get_freq(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_freq(net_dev,info,data,extra);
}

int ioctl_iw_set_bss_type(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_bss_type(net_dev,info,data,extra);
}

int ioctl_iw_get_bss_type(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_bss_type(net_dev,info,data,extra);
}

int ioctl_iw_get_range(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_range(net_dev,info,data,extra);
}

int ioctl_iw_get_ap(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_ap(net_dev,info,data,extra);
}

int ioctl_iw_start_scan(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_start_scan(net_dev,info,data,extra);
}

int ioctl_iw_get_scan_res(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_scan_res(net_dev,info,data,extra);
}

int ioctl_iw_set_essid(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_essid(net_dev,info,data,extra);
}

int ioctl_iw_get_essid(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_essid(net_dev,info,data,extra);
}

int ioctl_iw_set_rate(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_rate(net_dev,info,data,extra);
}

int ioctl_iw_get_rate(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_rate(net_dev,info,data,extra);
}

int ioctl_iw_set_rts(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_rts(net_dev,info,data,extra);
}

int ioctl_iw_get_rts(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_rts(net_dev,info,data,extra);
}

int ioctl_iw_set_frag(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_frag(net_dev,info,data,extra);
}

int ioctl_iw_get_frag(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_frag(net_dev,info,data,extra);
}

int ioctl_iw_set_encode(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_encode(net_dev,info,data,extra);
}

int ioctl_iw_get_encode(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_encode(net_dev,info,data,extra);
}

int ioctl_iw_set_auth(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_auth(net_dev,info,data,extra);
}

int ioctl_iw_get_auth(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_auth(net_dev,info,data,extra);
}

int ioctl_iw_set_encode_ext(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_set_encode_ext(net_dev,info,data,extra);
}

int ioctl_iw_get_encode_ext(
	struct net_device *net_dev,
	struct iw_request_info *info,
	union iwreq_data *data,
	char *extra)
{
	DBG_ENTER();
	return _ioctl_iw_get_encode_ext(net_dev,info,data,extra);
}


static const iw_handler _wdev_iwconfig_handler[] = {
	(iw_handler) NULL,					/* SIOCSIWCOMMIT */
	(iw_handler) ioctl_iw_get_name,	/* SIOCGIWNAME */
	(iw_handler) NULL,	/* SIOCSIWNWID */
	(iw_handler) NULL,	/* SIOCGIWNWID */
	
	(iw_handler) ioctl_iw_set_freq,	/* SIOCSIWFREQ */
	(iw_handler) ioctl_iw_get_freq,	/* SIOCGIWFREQ */
	
	(iw_handler) ioctl_iw_set_bss_type,	/* SIOCSIWMODE */
	(iw_handler) ioctl_iw_get_bss_type,	/* SIOCGIWMODE */
	(iw_handler) NULL,	/* SIOCSIWSENS */
	(iw_handler) NULL,	/* SIOCGIWSENS */
	(iw_handler) NULL,	/* SIOCSIWRANGE */
	(iw_handler) ioctl_iw_get_range,	/* SIOCGIWRANGE */
	(iw_handler) NULL,					/* SIOCSIWPRIV */
	(iw_handler) NULL,					/* SIOCGIWPRIV */
	(iw_handler) NULL,					/* SIOCSIWSTATS */
	(iw_handler) NULL,					/* SIOCGIWSTATS */
	(iw_handler) NULL,					/* SIOCSIWSPY */
	(iw_handler) NULL,					/* SIOCGIWSPY */
	(iw_handler) NULL,					/* SIOCSIWTHRSPY */
	(iw_handler) NULL,					/* SIOCGIWTHRSPY */
	(iw_handler) NULL,		//_ioctl_iw_set_ap,		/* SIOCSIWAP */
	(iw_handler) ioctl_iw_get_ap,		/* SIOCGIWAP */
	(iw_handler) NULL,					/* -- hole -- */
	(iw_handler) NULL,					/* SIOCGIWAPLIST depreciated */
	(iw_handler) ioctl_iw_start_scan,	/* SIOCSIWSCAN */
	(iw_handler) ioctl_iw_get_scan_res,/* SIOCGIWSCAN */
	(iw_handler) ioctl_iw_set_essid,	/* SIOCSIWESSID */
	(iw_handler) ioctl_iw_get_essid,	/* SIOCGIWESSID */
	(iw_handler) NULL,	/* SIOCSIWNICKN */
	(iw_handler) NULL,	/* SIOCGIWNICKN */
	(iw_handler) NULL,					/* -- hole -- */
	(iw_handler) NULL,					/* -- hole -- */
	(iw_handler) ioctl_iw_set_rate,	/* SIOCSIWRATE */
	(iw_handler) ioctl_iw_get_rate,	/* SIOCGIWRATE */
	(iw_handler) ioctl_iw_set_rts,		/* SIOCSIWRTS */
	(iw_handler) ioctl_iw_get_rts,		/* SIOCGIWRTS */
	(iw_handler) ioctl_iw_set_frag,	/* SIOCSIWFRAG */
	(iw_handler) ioctl_iw_get_frag,	/* SIOCGIWFRAG */
	(iw_handler) NULL,	/* SIOCSIWTXPOW */
	(iw_handler) NULL,	/* SIOCGIWTXPOW */
	(iw_handler) NULL,		//_ioctl_iw_set_retry,	/* SIOCSIWRETRY */
	(iw_handler) NULL,		//_ioctl_iw_get_retry,	/* SIOCGIWRETRY */
	(iw_handler) ioctl_iw_set_encode,	/* SIOCSIWENCODE */
	(iw_handler) ioctl_iw_get_encode,	/* SIOCGIWENCODE */

	(iw_handler) NULL,	/* SIOCSIWPOWER */
	(iw_handler) NULL,	/* SIOCGIWPOWER */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) NULL,			/* SIOCSIWGENIE */
	(iw_handler) NULL,			/* SIOCGIWGENIE */
	(iw_handler) ioctl_iw_set_auth,		/* SIOCSIWAUTH */
	(iw_handler) ioctl_iw_get_auth,		/* SIOCGIWAUTH */
	(iw_handler) ioctl_iw_set_encode_ext,	/* SIOCSIWENCODEEXT */
	(iw_handler) ioctl_iw_get_encode_ext,	/* SIOCGIWENCODEEXT */
	(iw_handler) NULL,			/* SIOCSIWPMKSA */
};

#else

static const iw_handler _wdev_iwconfig_handler[] = {
	(iw_handler) NULL,					/* SIOCSIWCOMMIT */
	(iw_handler) _ioctl_iw_get_name,	/* SIOCGIWNAME */
	(iw_handler) NULL,	/* SIOCSIWNWID */
	(iw_handler) NULL,	/* SIOCGIWNWID */
	
	(iw_handler) _ioctl_iw_set_freq,	/* SIOCSIWFREQ */
	(iw_handler) _ioctl_iw_get_freq,	/* SIOCGIWFREQ */
	
	(iw_handler) _ioctl_iw_set_bss_type,	/* SIOCSIWMODE */
	(iw_handler) _ioctl_iw_get_bss_type,	/* SIOCGIWMODE */
	(iw_handler) _ioctl_iw_set_sens,		/* SIOCSIWSENS */
	(iw_handler) _ioctl_iw_get_sens,		/* SIOCGIWSENS */
	(iw_handler) NULL,	/* SIOCSIWRANGE */
	(iw_handler) _ioctl_iw_get_range,	/* SIOCGIWRANGE */
	(iw_handler) NULL,					/* SIOCSIWPRIV */
	(iw_handler) NULL,					/* SIOCGIWPRIV */
	(iw_handler) NULL,					/* SIOCSIWSTATS */
	(iw_handler) NULL,					/* SIOCGIWSTATS */
	(iw_handler) NULL,					/* SIOCSIWSPY */
	(iw_handler) NULL,					/* SIOCGIWSPY */
	(iw_handler) NULL,					/* SIOCSIWTHRSPY */
	(iw_handler) NULL,					/* SIOCGIWTHRSPY */
	(iw_handler) NULL,		//_ioctl_iw_set_ap,		/* SIOCSIWAP */
	(iw_handler) _ioctl_iw_get_ap,		/* SIOCGIWAP */
	(iw_handler) NULL,					/* -- hole -- */
	(iw_handler) NULL,					/* SIOCGIWAPLIST depreciated */
	(iw_handler) _ioctl_iw_start_scan,	/* SIOCSIWSCAN */
	(iw_handler) _ioctl_iw_get_scan_res,/* SIOCGIWSCAN */
	(iw_handler) _ioctl_iw_set_essid,	/* SIOCSIWESSID */
	(iw_handler) _ioctl_iw_get_essid,	/* SIOCGIWESSID */
	(iw_handler) NULL,	/* SIOCSIWNICKN */
	(iw_handler) NULL,	/* SIOCGIWNICKN */
	(iw_handler) NULL,					/* -- hole -- */
	(iw_handler) NULL,					/* -- hole -- */
	(iw_handler) _ioctl_iw_set_rate,	/* SIOCSIWRATE */
	(iw_handler) _ioctl_iw_get_rate,	/* SIOCGIWRATE */
	(iw_handler) _ioctl_iw_set_rts,		/* SIOCSIWRTS */
	(iw_handler) _ioctl_iw_get_rts,		/* SIOCGIWRTS */
	(iw_handler) _ioctl_iw_set_frag,	/* SIOCSIWFRAG */
	(iw_handler) _ioctl_iw_get_frag,	/* SIOCGIWFRAG */
	(iw_handler) _ioctl_iw_set_tx_pow,	/* SIOCSIWTXPOW */
	(iw_handler) _ioctl_iw_get_tx_pow,	/* SIOCGIWTXPOW */
	(iw_handler) NULL,		//_ioctl_iw_set_retry,	/* SIOCSIWRETRY */
	(iw_handler) NULL,		//_ioctl_iw_get_retry,	/* SIOCGIWRETRY */
	(iw_handler) _ioctl_iw_set_encode,	/* SIOCSIWENCODE */
	(iw_handler) _ioctl_iw_get_encode,	/* SIOCGIWENCODE */

	(iw_handler) NULL,	/* SIOCSIWPOWER */
	(iw_handler) NULL,	/* SIOCGIWPOWER */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) NULL,			/* -- hole -- */
	(iw_handler) NULL,			/* SIOCSIWGENIE */
	(iw_handler) NULL,			/* SIOCGIWGENIE */
	(iw_handler) _ioctl_iw_set_auth,		/* SIOCSIWAUTH */
	(iw_handler) _ioctl_iw_get_auth,		/* SIOCGIWAUTH */
	(iw_handler) _ioctl_iw_set_encode_ext,	/* SIOCSIWENCODEEXT */
	(iw_handler) _ioctl_iw_get_encode_ext,	/* SIOCGIWENCODEEXT */
	(iw_handler) NULL,			/* SIOCSIWPMKSA */
};

#endif

inline int _ioctl_priv__get_bytes_driver_ver(
	struct net_device *ndev, 
	union iwreq_data *u,
	char *buff)
{
	int i;
	
	_ioctl_print("%s =>\n", __FUNCTION__);
	
	sprintf(buff, "%s", WLAN_DRV_VERSION);
	u->data.length = tdsp_strlen(buff);

	// wumin: replace space to underline
	// wumin: change time format
	// wumin: because wifi-radar can't slove space and some of symbols
	for(i=0;i<u->data.length;i++)
	{
		if(	buff[i] == ' '
		||	buff[i] == ':')
		{
			buff[i] = '_';			
		}
	}

	return 0;
}


inline int _ioctl_priv__get_bytes_dsp_ver(
	struct net_device *ndev, 
	union iwreq_data *u,
	char *buff)
{
	void*	pAdap = lwlan_get_netdev_priv(ndev);

	_ioctl_print("%s =>\n", __FUNCTION__);

	sprintf(buff, "0x%08x", Adap_Get_Dsp_Version(pAdap));
	u->data.length = strlen(buff);
	
	return 0;
}


int _ioctl_priv_set_string(
	struct net_device *ndev, 
	struct iw_request_info *info, 
	void *uwrq, 
	char *extra)
{
	int retv = -EOPNOTSUPP;
	return retv;
}



int _ioctl_priv_get_string(
	struct net_device *ndev, 
	struct iw_request_info *info, 
	void *uwrq, 
	char *extra)
{
	struct iw_point *iwpoint = uwrq;
	union iwreq_data *u = uwrq;
	int retv = -EOPNOTSUPP;

	switch (iwpoint->flags)
	{
		case DSP_PARAM__DRIVER_VER:
			retv = _ioctl_priv__get_bytes_driver_ver(ndev, u, extra);
			break;
		case DSP_PARAM__DSP_VER:
			retv = _ioctl_priv__get_bytes_dsp_ver(ndev, u, extra);
			break;
		default:
			retv = EOPNOTSUPP;
			break;
	}

	return retv;	
}


int _ioctl_priv__set_domain(
	struct net_device *ndev, 
	unsigned int *domain_val)
{
	void * pAdap = lwlan_get_netdev_priv(ndev);

	_ioctl_print("%s =>\n", __FUNCTION__);
	switch (*domain_val)
	{
		case 0://DRIVERREGION_AMERICAS:
			_ioctl_print("%s: set domian to USA\n", __FUNCTION__);
			break;
		case 1://DRIVERREGION_EUROPE:
			_ioctl_print("%s: set domian to Europe\n", __FUNCTION__);
			break;
		case 2://DRIVERREGION_JAPAN:
			_ioctl_print("%s: set domian to Japan\n", __FUNCTION__);
			break;
		case 3://DRIVERREGION_RESTOFWORLD:
			_ioctl_print("%s: set domian to ResetofWorld\n", __FUNCTION__);
			break;
		default:
			_ioctl_print( "[%s]    Unknown country region[%x] !!! ",
							__FUNCTION__, *domain_val);
			return -EINVAL;
	}

    Adap_Set_Domain(pAdap, (unsigned char)(*domain_val));

	return 0;
}


inline int _ioctl_priv__get_domain(
	struct net_device *ndev, 
	unsigned int *domain_val)
{
	void * pAdap = lwlan_get_netdev_priv(ndev);
	*domain_val = Adap_Get_Domain(pAdap);
	
	return 0;
}

static inline int _ioctl_priv__set_dbg_cmd(
	struct net_device *ndev, 
	unsigned int *domain_val)
{

	void * pAdap = lwlan_get_netdev_priv(ndev);


	switch (*domain_val) {
		
	case 0:		// reserved
		//tdsp_memory_report(0, 0, 0);
		break;
		
	case 1:		// dump usb register 

		printk("\n\n************** USB REGISTERS***************\n");	
		Adap_Print_USBReg(pAdap);		
		break;
		
	case 2:		// dump tx status
		printk("\n\n************** TX STATUS ***************\n");
	    Tx_Print_Status(pAdap);
		break;

	case 3:		//dump rx status
		printk("\n\n************** RX STATUS ***************\n");
		break;
		
	case 4:		//dump sme status
		printk("\n\n************** MNGEMENT STATUS ***************\n");
		break;


#ifdef TEST_SYSTEM_DATA_STRUCT		//fdeng@20091214	 test
	case 96:		
		printk( "\n\n\n[%s]    sizeof(uint8) = %ld,     "
					"sizeof(uint16) = %ld,     "
					"sizeof(uint32) = %ld,     \n"
					"sizeof(uint64) = %ld,      "
					"sizeof(w_ulong) = %ld\n\n\n\n\n", __FUNCTION__,
					(w_ulong)sizeof(uint8), (w_ulong)sizeof(uint16),
					(w_ulong)sizeof(uint32),(w_ulong)sizeof(uint64),
					(w_ulong)sizeof(w_ulong));
		break;
#endif

	default:
		printk("[%s]    Unknown debug command[%x] !!! \n", 
					__FUNCTION__, *domain_val);
		printk("    command number 96 : test internal RESET !!! \n");
		printk("    command number  4 : dump management status !!! \n");
		printk("    command number  3 : dump rx status !!! \n");
		printk("    command number  2 : dump tx status !!! \n");
		printk("    command number  1 : dump usb register !!! \n");
		printk("    command number  0 : dump detail of apapter struct !!! \n");
		return -EINVAL;
	}

	return 0;
}


int _ioctl_priv_set_int(
	struct net_device *ndev, 
	struct iw_request_info *info, 
	void *uwrq, 
	char *extra)
{
	unsigned int *i = (unsigned int *) extra;
	unsigned int param = i[0];		/* parameter id is 1st */
	unsigned int value = i[1];		/* NB: most values are TYPE_INT */
	int retv = -EOPNOTSUPP;

	switch (param) {
	case DSP_PARAM__REG_DOMAIN:
		retv = _ioctl_priv__set_domain(ndev, &value);
		break;
	case DSP_PARAM__TX_PWR: 
//		retv = _ioctl_priv__set_txpower(ndev, &value);
		break;
	case DSP_PARAM__DBG_CMD:
	    retv = _ioctl_priv__set_dbg_cmd(ndev, &value);
		break;
	default:
		retv = EOPNOTSUPP;
		break;
	}
	
	return retv;	

}



int _ioctl_priv_get_int(
	struct net_device *ndev, 
	struct iw_request_info *info, 
	void *uwrq, 
	char *extra)
{
	unsigned int *param = (unsigned int *)extra;
	int retv = -EOPNOTSUPP;

	switch (param[0]) {
	case DSP_PARAM__REG_DOMAIN:
		retv = _ioctl_priv__get_domain(ndev, param);
		break;
	case DSP_PARAM__TX_PWR:
//		retv = _ioctl_priv__get_txpower(ndev, param);
		break;
		
	default:
		retv = EOPNOTSUPP;
		break;
	}

	return retv;	
}


static const iw_handler _wdev_iwpriv_handler[] = {
													/* SIOCIWFIRSTPRIV + */
	(iw_handler) NULL,								/* 0 */
	(iw_handler) NULL,					/* 1 */
	(iw_handler) NULL,					/* 2 */
	(iw_handler) NULL,//_ioctl_priv_get_phy_mac,			/* 3 */
	(iw_handler) NULL,//_ioctl_priv_set_phy_mac,			/* 4 */
	(iw_handler) NULL,					/* 5 */
	(iw_handler) NULL,					/* 6 */
	(iw_handler) NULL,					/* 7 */
	(iw_handler) NULL,					/* 8 */
	(iw_handler) NULL,								/* 9 */
	(iw_handler) NULL,								/* 10 */
	(iw_handler) NULL,								/* 11 */
	(iw_handler) NULL,								/* 12 */
	(iw_handler) _ioctl_priv_get_string,			/* 13 */
	(iw_handler) _ioctl_priv_set_string,			/* 14 */
	(iw_handler) _ioctl_priv_get_int,				/* 15 */
	(iw_handler) _ioctl_priv_set_int,				/* 16 */
	(iw_handler) NULL,//_ioctl_priv_ctldev_help,			/* 17 */
	(iw_handler) NULL,			                    /* 18 */
	(iw_handler) NULL,			                    /* 19 */
	(iw_handler) NULL,								/* 20 */
	(iw_handler) NULL,								/* 21 */
	(iw_handler) NULL,								/* 22 */
};	





static const struct iw_priv_args _wdev_iwpriv_args[] = {
	/*{ cmd, set_args, get_args, name } */
	{DSP_RESET, 0, 0, "reset" },
	{DSP_RESTART, 0, 0, "restart" },
	{DSP_GET_PHY_MAC, 
	 0, IW_PRIV_TYPE_CHAR | 18, "getMac"},
	{DSP_SET_PHY_MAC, 
	 IW_PRIV_TYPE_CHAR | 18, 0, "setMac"},

	
	{DSP_CTLDEV_GET_BYTES,
	IW_PRIV_TYPE_CHAR |512, IW_PRIV_TYPE_CHAR |512, "get_param_bytes" },
	{DSP_CTLDEV_SET_BYTES,  IW_PRIV_TYPE_CHAR |512, 0, "set_param_bytes" },
	/* sub-ioctl handlers */
	{DSP_CTLDEV_GET_BYTES, 0, IW_PRIV_TYPE_CHAR |512, "" },
	{DSP_CTLDEV_SET_BYTES, IW_PRIV_TYPE_CHAR |512, 0, "" },
	/* sub-ioctl definitions */
	{DSP_PARAM__DRIVER_VER,
	 0, IW_PRIV_TYPE_CHAR |512, "driver_version" },
	{DSP_PARAM__DSP_VER,
	 0, IW_PRIV_TYPE_CHAR |512, "dsp_version" },
	/* end of sub-ioctl definitions */

	{DSP_CTLDEV_GET_INT, 
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_param_int" },
	{DSP_CTLDEV_SET_INT, 
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "set_param_int" },
	 
	/* sub-ioctl handlers */
	{ DSP_CTLDEV_GET_INT,
	  0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "" },
	{DSP_CTLDEV_SET_INT,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "" },
	/* sub-ioctl definitions */
	{DSP_PARAM__REG_DOMAIN,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "domain" },
	{DSP_PARAM__REG_DOMAIN,
	 0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_domain" },
	{DSP_PARAM__TX_PWR,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "tx_power" },
	{DSP_PARAM__TX_PWR,
	 0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_tx_power" }, 
	{DSP_PARAM__DBG_CMD,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "dbg_cmd" },

	/* end of sub-ioctl definitions */
	{DSP_CTLDEV_HELP, 
	 0, IW_PRIV_TYPE_CHAR | HELP_BUF_LEN, "help"},
};




const struct iw_handler_def _wlan_dev_iw_handler_def =
{
	.num_standard	= sizeof(_wdev_iwconfig_handler) / sizeof(iw_handler),
	.standard	= (iw_handler *) _wdev_iwconfig_handler,
	.num_private	= sizeof(_wdev_iwpriv_handler) / sizeof(iw_handler),
	.private	= (iw_handler *) _wdev_iwpriv_handler,
	.num_private_args = sizeof(_wdev_iwpriv_args) / sizeof(struct iw_priv_args),
	.private_args	= (struct iw_priv_args *) _wdev_iwpriv_args,
	.get_wireless_stats = _ioctl_iw_get_wireless_stats
};


void  iw_send_bssid_to_up(void *dev, unsigned char *bssid)
{
	union iwreq_data wrqu;	
	wrqu.data.length = 0;		
	wrqu.data.flags = 0;		
	if (bssid == NULL)
		tdsp_memset(wrqu.ap_addr.sa_data, 0, ETH_ALEN);
	else
		tdsp_memcpy(wrqu.ap_addr.sa_data, bssid, ETH_ALEN);	
	wrqu.ap_addr.sa_family = ARPHRD_ETHER;	
	wireless_send_event(dev, SIOCGIWAP, &wrqu, NULL);
}


