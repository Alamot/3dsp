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
 * [lnet_dev.c] description:
 * 	This source file defines the net device of wlan driver in linux.
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

 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/usb.h>
#include <net/iw_handler.h>
#include <linux/wireless.h>
#include <linux/etherdevice.h>
#include <linux/if_arp.h>

#include "tdsp_string.h"
#include "lwlan_netdev.h"
#include "lwlan_ioctl.h"
#include "tdsp_memory.h"
#include "tdsp_bus.h"

static char* TDSP_FILE_INDICATOR="NETDV";

int _wlan_netdev_open(  struct net_device* netdev)
{

	netif_carrier_on(netdev);
	netif_wake_queue(netdev);

	return 0;
}



int _wlan_netdev_close( struct net_device* netdev)
{	
	netif_stop_queue(netdev);
	netif_carrier_off(netdev);
	return 0;
}



int _wlan_netdev_ioctl(
    struct net_device* netdev,
    struct ifreq* ifr,
    int    cmd)
{
	/*printk("[%s]            Log,  "
					 "someone invoked our IOCTL fn !!!\n", __FUNCTION__);*/
	return 0;
}


struct net_device_stats * _wlan_netdev_get_stats(struct net_device* netdev)
{   
	STAT_DATA data;
	void * pAdap = lwlan_get_netdev_priv(netdev);
	struct net_device_stats * net_stats = (struct net_device_stats *)Adap_Get_Netstat(pAdap);
    tdsp_memset(&data, 0, sizeof(STAT_DATA));
    Adap_Get_StatData(pAdap,&data);	
	net_stats->tx_packets = data.tx_good;
	net_stats->tx_errors = data.tx_err;
	net_stats->tx_bytes = data.tx_payload;
	net_stats->rx_packets = data.rx_good;
	net_stats->rx_errors = data.rx_err;
	net_stats->rx_bytes = data.rx_payload;
	net_stats->multicast = data.rx_mc;
	net_stats->rx_dropped = data.rx_dropped;

    return net_stats;
    
}


int _wlan_netdev_xmit(
    struct sk_buff		*xmit_skb,
    struct net_device	*netdev)
{
    //get 802.3 eth type
	int rvl = 0;
	void * pAdap = lwlan_get_netdev_priv(netdev);

    if(pAdap == NULL)
    {
        printk("_wlan_netdev_xmit:adap is null\n");
        return -1;
    }
	if(!Adap_Tx_Available(pAdap))
	{	
		return -1;
	}
	
	if(Adap_Use_WPA(pAdap))
	{			
		//get 802.3 eth type
		unsigned char * data_p = xmit_skb->data + 12;
		unsigned short eth_type = *(unsigned short *)data_p;
			
		//DBG_WLAN__NETDEV(LEVEL_TRACE,"JOINED but WPA havn't done.\n");
		
		if(eth_type != 0x8e88)
		{
			kfree_skb(xmit_skb);
			printk("filter packet for wpa mode\n");
			return 0;		// Success
		}
		else
		{
			printk("will transmit 802.1x packet\n");
		}
	}
	
	{
		//struct sk_buff *skb = NULL;
		unsigned int len;
		len = xmit_skb->len;
		if (len <= 0x600) /*MAX_ETHER_LEN*/
		{
			rvl = Adap_Send(pAdap, xmit_skb->data, len);
			kfree_skb(xmit_skb);
			return 0;		
		}	
		else
		{
			printk("%s len = %d > %d, error !!!\n", __FUNCTION__, xmit_skb->len, 0x600);
			return -1;
		}
	}
   
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
static const struct net_device_ops _wlan_netdev_op =
{
	.ndo_open		 = _wlan_netdev_open,
	.ndo_stop		 = _wlan_netdev_close,
	.ndo_start_xmit	 = _wlan_netdev_xmit,
	.ndo_get_stats	 = _wlan_netdev_get_stats,
	.ndo_do_ioctl    = _wlan_netdev_ioctl,
};
#endif

/*
 * Name			: wlan_dev_setup()
 *				:
 * Parameters	: 1. 
 *				: 2.
 *				: 3.
 *				:
 * Return value	: int(result number)
 *				:
 * Description	: This function register net_device in kernel
 *				:   
 * Notice 		:
 * See also		: 
 * TODO			: 
 */
int wlan_netdev_setup(void* adapt)
{	
	struct net_device* netdev;
	struct net_device_stats * netstat;
	if (Adap_Get_Netdev(adapt) != NULL)
		return -EEXIST;
	
	netdev = alloc_etherdev(4);

	if(netdev == NULL) 
	{
		return -ENOMEM;
	}
	ether_setup(netdev);
	// TODO:Jakcie mask it for 2.6.25 SET_MODULE_OWNER(netdev);

	tdsp_strcpy(netdev->name, "twifiu0");
	netstat = (struct net_device_stats *)tdsp_memory_alloc(sizeof(struct net_device_stats));
	tdsp_memset(netstat, 0, sizeof(struct net_device_stats));
	Adap_Set_Netstat(adapt,netstat);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
	netdev->netdev_ops = (struct net_device_ops*)&_wlan_netdev_op;
#else	
	netdev->open				= _wlan_netdev_open;
	netdev->stop				= _wlan_netdev_close;
	netdev->get_stats			= _wlan_netdev_get_stats;
	netdev->do_ioctl			= _wlan_netdev_ioctl;

	netdev->hard_start_xmit 	= _wlan_netdev_xmit; 
	netdev->set_mac_address	= NULL; 
#endif
	
	netdev->type                	= ARPHRD_ETHER;
	netdev->wireless_handlers 	= (struct iw_handler_def *) &_wlan_dev_iw_handler_def;


	lwlan_set_netdev_priv(netdev, (void *)adapt);		// save dsp_adapter sturct in netdev
	//set wlan device's mac addr
	tdsp_memcpy(netdev->dev_addr, Adap_Get_Mac_Addr(adapt), 6);

	Adap_Set_Netdev(adapt,netdev);    // save netdev in wlan sturct
	PTDSPBUS_INTERFACE pIntf = (PTDSPBUS_INTERFACE)Adap_Get_BusInf(adapt);
	struct usb_device *udev = (struct usb_device *)pIntf->pUsbDev;
	SET_NETDEV_DEV(netdev,&(udev->dev));
	if (register_netdev(netdev))
	{
		printk("[%s:%05d] register_netdev failed!!!\n ", TDSP_FILE_INDICATOR,__LINE__);
		free_netdev(netdev);
		return -ENODEV;
	}

    
	//pAdap->net_dev = netdev;			// save netdev in wlan sturct
    
	// tdsp_spin_lock_init(&(pAdap->netq_ctrl_lock));

     
	printk("[%s:%05d] register netdev successfully\n", TDSP_FILE_INDICATOR,__LINE__);
		
	return 0;	//0 indicates success

}





void wlan_netdev_destroy(void* adapt)
{
	struct net_device* 	net_dev ;
	struct net_device_stats * netstat;

	net_dev = (struct net_device *)Adap_Get_Netdev(adapt);
	
	if (net_dev == NULL)
		return ;
	Adap_Set_Netdev(adapt,NULL);

	// Unregister network device data	
	unregister_netdev(net_dev);
	free_netdev(net_dev);

	netstat = (struct net_device_stats *)Adap_Get_Netstat(adapt);
	tdsp_memory_free(netstat);
	Adap_Set_Netstat(adapt,NULL);

	printk("[%s:%05d] unregister netdev successfully\n", TDSP_FILE_INDICATOR,__LINE__);

	return;
}

