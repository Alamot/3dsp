#ifndef __TDSP_SKB_H__
#define __TDSP_SKB_H__
#include <linux/skbuff.h>
#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include "tdsp_memory.h"
#include "usbbt_lib.h"

void tdsp_skb_queue_init(PTDSP_SK_BUFF_HEAD skb_head);
void tdsp_skb_queue_release(PTDSP_SK_BUFF_HEAD skb_head);
unsigned int tdsp_skb_get_queue_len(PTDSP_SK_BUFF_HEAD skb_head);
void tdsp_skb_put_queue_tail(PTDSP_SK_BUFF_HEAD skb_head, PTDSP_SK_BUFF newsk);
int tdsp_skb_is_queue_empty(PTDSP_SK_BUFF_HEAD skb_head);
void* tdsp_skb_dequeue_skb(PTDSP_SK_BUFF_HEAD skb_head);
void* tdsp_skb_alloc_atomic(unsigned long size);
void* tdsp_bt_skb_alloc_atomic(unsigned long size);
void tdsp_skb_kfree(PTDSP_SK_BUFF newsk);
void* tdsp_put_to_skb(PTDSP_SK_BUFF newsk, unsigned long size);
unsigned int tdsp_skb_get_len(PTDSP_SK_BUFF newsk);
void* tdsp_skb_get_dataptr(PTDSP_SK_BUFF newsk);
void tdsp_skb_set_dev(PTDSP_SK_BUFF newsk, void* dev);
void tdsp_skb_set_btpkt_type(PTDSP_SK_BUFF newsk, unsigned long type);

#endif
