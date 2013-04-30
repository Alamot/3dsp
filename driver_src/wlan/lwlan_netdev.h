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
 * [lwlan_netdev.h] description:
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
#ifndef _LWLAN_NETDEV_H_
#define _LWLAN_NETDEV_H_

#include <linux/netdevice.h>
#include <net/iw_handler.h>
#include <linux/wireless.h>
#include <linux/etherdevice.h>
#include <linux/if_arp.h>

#include "usbwlan_lib.h"
typedef struct _WLAN_NET_PRIV
{
	void *priv;
}TWLAN_NET_PRIV;

static inline void tdsp_netdev_skb_sbmt(void* net_dev, void* pkt_buff, unsigned int buff_len)
{
	struct sk_buff *	skb = (struct sk_buff *)pkt_buff;

    skb_put(skb, buff_len);
	skb->len = buff_len;
    
	if (net_dev == NULL)
	{
		dev_kfree_skb_any(skb);
		return;
	}
	
	skb->dev = (struct net_device *)net_dev;
	skb->ip_summed = CHECKSUM_NONE;
	skb->protocol = eth_type_trans(skb, (struct net_device *)net_dev);

#if 0
{
	PUINT8			p_dest_address = (PUINT8)(skb->data);
	PUINT16			p_eth_proto_type = (PUINT16)(skb->data+12);
	BOOLEAN			b_not_submit = FALSE;
	
	if(	!MAC_ADDR_IS_GROUP(p_dest_address)
	&&	!tdsp_memequ(pAdap->current_address,p_dest_address,6))
	{
//		DBG_WLAN__NETDEV(LEVEL_TRACE,"RNS,bad addr: %02x:%02x:%02x:%02x:%02x:%02x.\n",
//			p_dest_address[0], p_dest_address[1], p_dest_address[2], p_dest_address[3], p_dest_address[4], p_dest_address[5]);
		b_not_submit = TRUE;
	}
	else if(pAdap->wlan_attr.hasjoined != JOIN_HASJOINED)
	{
//		DBG_WLAN__NETDEV(LEVEL_TRACE,"RNS, Not Joined.\n");
		b_not_submit = TRUE;
	}	
	else if(	(	pAdap->wlan_attr.auth_mode == AUTH_MODE_WPA
			||	pAdap->wlan_attr.auth_mode == AUTH_MODE_WPA_PSK
			||	pAdap->wlan_attr.auth_mode == AUTH_MODE_WPA2
			||	pAdap->wlan_attr.auth_mode == AUTH_MODE_WPA2_PSK) 
		&&	!pAdap->wlan_attr.wpa_group_key_valid
		&&	*p_eth_proto_type != 0x8e88)
	{
//		DBG_WLAN__NETDEV(LEVEL_TRACE,"RNS, WPA not complete, proto type:0x%04x.\n",*p_eth_proto_type);
		b_not_submit = TRUE;
	}

	//DBG_WLAN__NETDEV(LEVEL_TRACE,"[%s] skb=%p, skb->data=%p, skb->tail=%p, skb->len=%u\n", __FUNCTION__,skb,skb->data,skb->tail,skb->len);
	
	if(b_not_submit)
	{
		dev_kfree_skb_any(skb);
	}
	else
	{
		if(!MAC_ADDR_IS_GROUP(p_dest_address))
		{		
			//DBG_PRINT_BUFF("^^ Driver Send Uni Packet to OS ^^:", skb->data-14, skb->len+14);
		}
		netif_rx(skb);
	}
}
#else
	netif_rx(skb);
#endif

	return;
}

static inline void* tdsp_netdev_skb_alloc(unsigned int buf_len, void** para)
{
	struct sk_buff *skb = NULL;

	skb = dev_alloc_skb(buf_len+2);

	*para = (void*)skb;
	
	if(skb==NULL)
	{
		return NULL;
	}

	skb_reserve(skb,2);        /*Force 16 byte alignment*/
	
	//DBG_WLAN__NETDEV(LEVEL_TRACE,"[%s] skb=%p, skb->data=%p, para=%p\n", __FUNCTION__,skb,skb->data,*para);

	return (void*)skb->data;
}


static inline void tdsp_netdev_skb_free(void* pkt_buff)
{
	struct sk_buff *skb = (struct sk_buff *)pkt_buff;

	//DBG_WLAN__NETDEV(LEVEL_TRACE,"[%s] skb=%p, skb->data=%p, pkt_buff=%p\n", __FUNCTION__,skb,skb->data,pkt_buff);
	
	dev_kfree_skb_any(skb);

	return;
}

//#define lnetdev_tx_netq_stopped(pAdap)		_lnetdev_tx_netq_stopped(pAdap,(INT8*)__FUNCTION__,__LINE__)

static inline int tdsp_netdev_netq_ifstop(void* net_dev)//,INT8* func_name, UINT32 line_number)
{	
	//DBG_WLAN__NETDEV(LEVEL_TRACE, "[%s] TX is %s (Called by %s[%u])\n", __FUNCTION__, netif_queue_stopped(pAdap->net_dev)?"STOPED":"STARTED",func_name,line_number);
	
	if (net_dev == NULL)
    {
        printk("[%s]: error:net dev is NULL\n", __FUNCTION__);
		return 1;
    }
	return (netif_queue_stopped((struct net_device *)net_dev) ? 1 :0);
}

//#define lnetdev_tx_start(pAdap)		_lnetdev_tx_start(pAdap,(INT8*)__FUNCTION__,__LINE__)
	
static inline void tdsp_netdev_netq_start(void* net_dev)//,INT8* func_name, UINT32 line_number)
{	
	//DBG_WLAN__NETDEV(LEVEL_TRACE, "[%s] START TX (Called by %s[%u])!\n", __FUNCTION__,func_name,line_number);
	if (net_dev == NULL)
		return;
	
	netif_wake_queue((struct net_device *)net_dev);
	return;
}

static inline void lwlan_set_netdev_priv(struct net_device *net_dev, void *data)
{
	TWLAN_NET_PRIV *net_priv;
	
	net_priv = netdev_priv(net_dev);
	net_priv->priv = data;
//	net_dev->ml_priv = data;
}
static inline void *lwlan_get_netdev_priv(struct net_device *net_dev)
{
	TWLAN_NET_PRIV *net_priv;
	
	net_priv = netdev_priv(net_dev);
	return net_priv->priv;
//	return net_dev->ml_priv;
}

static inline void tdsp_netdev_netq_stop(void* net_dev)
{
	//DBG_WLAN__NETDEV(LEVEL_TRACE, "[%s] STOP TX!\n", __FUNCTION__);
	netif_stop_queue((struct net_device *)net_dev);
	return;
}

int wlan_netdev_setup(void* adapt);

void wlan_netdev_destroy(void* net_dev);

#endif

