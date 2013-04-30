#include <linux/kernel.h>
#include "tdsp_skb.h"

void tdsp_skb_queue_init(PTDSP_SK_BUFF_HEAD skb_head)
{
	struct sk_buff_head *head;
	head =  (struct sk_buff_head *)kmalloc(sizeof(struct sk_buff_head), GFP_KERNEL);
	if(NULL == head)
	{
		printk("%s:error, can't alloc memory for spinlock\n",__FUNCTION__);
		return;
	}
	skb_queue_head_init(head);
	skb_head->buffhead = head;
}
void tdsp_skb_queue_release(PTDSP_SK_BUFF_HEAD skb_head)
{
	if(NULL == skb_head->buffhead)
	{
		printk("%s:error, can't alloc memory for spinlock\n",__FUNCTION__);
		return;
	}
	kfree(skb_head->buffhead);
	skb_head->buffhead = NULL;
}
unsigned int tdsp_skb_get_queue_len(PTDSP_SK_BUFF_HEAD skb_head)
{
	struct sk_buff_head *head;
	head =  (struct sk_buff_head *)skb_head->buffhead;
	if(NULL == head)
	{
		printk("%s:error, skb head is null!\n",__FUNCTION__);
	}
	return (skb_queue_len(head));
}
void tdsp_skb_put_queue_tail(PTDSP_SK_BUFF_HEAD skb_head, PTDSP_SK_BUFF newsk)
{
	struct sk_buff_head *head;
	struct sk_buff * skb;
	head =  (struct sk_buff_head *)skb_head->buffhead;
	skb = (struct sk_buff *)newsk->skbuff;
	if(NULL == head || NULL == skb)
	{
		printk("%s:error, skb head or sk buff is null!\n",__FUNCTION__);
		return;
	}
	skb_queue_tail(head, skb);
}
int tdsp_skb_is_queue_empty(PTDSP_SK_BUFF_HEAD skb_head)
{
	struct sk_buff_head *head;
	head =  (struct sk_buff_head *)skb_head->buffhead;
	if(NULL == head)
	{
		printk("%s:error, skb head is null!\n",__FUNCTION__);
	}
	return (skb_queue_empty(head));
}
void* tdsp_skb_dequeue_skb(PTDSP_SK_BUFF_HEAD skb_head)
{
	struct sk_buff_head *head;
	head =  (struct sk_buff_head *)skb_head->buffhead;
	if(NULL == head)
	{
		printk("%s:error, skb head is null!\n",__FUNCTION__);
	}
	return (skb_dequeue(head));
}
void* tdsp_skb_alloc_atomic(unsigned long size)
{
	return (alloc_skb(size, GFP_ATOMIC));
}
void* tdsp_bt_skb_alloc_atomic(unsigned long size)
{
	return (void*)(bt_skb_alloc(size, GFP_ATOMIC));
}
void tdsp_skb_kfree(PTDSP_SK_BUFF newsk)
{
	struct sk_buff * skb;
	skb = (struct sk_buff *)newsk->skbuff;
	if(NULL == skb)
	{
		printk("%s:error, sk buff is null!\n",__FUNCTION__);
		return;
	}
	kfree_skb(skb);
	newsk->skbuff = NULL;
}
void* tdsp_put_to_skb(PTDSP_SK_BUFF newsk, unsigned long size)
{
	struct sk_buff * skb;
	skb = (struct sk_buff *)newsk->skbuff;
	if(NULL == skb)
	{
		printk("%s:error, sk buff is null!\n",__FUNCTION__);
	}
	return (skb_put(skb, size));
}
unsigned int tdsp_skb_get_len(PTDSP_SK_BUFF newsk)
{
	struct sk_buff * skb;
	skb = (struct sk_buff *)newsk->skbuff;
	if(NULL == skb)
	{
		printk("%s:error, sk buff is null!\n",__FUNCTION__);
	}
	return (skb->len);
}
void* tdsp_skb_get_dataptr(PTDSP_SK_BUFF newsk)
{
	struct sk_buff * skb;
	skb = (struct sk_buff *)newsk->skbuff;
	if(NULL == skb)
	{
		printk("%s:error, sk buff is null!\n",__FUNCTION__);
	}
	return (skb->data);
}
void tdsp_skb_set_dev(PTDSP_SK_BUFF newsk, void* dev)
{
	struct sk_buff * skb;
	skb = (struct sk_buff *)newsk->skbuff;
	if(NULL == skb)
	{
		printk("%s:error, sk buff is null!\n",__FUNCTION__);
		return;
	}
	skb->dev = dev;
}
void tdsp_skb_set_btpkt_type(PTDSP_SK_BUFF newsk, unsigned long type)
{
	struct sk_buff * skb;
	skb = (struct sk_buff *)newsk->skbuff;
	if(NULL == skb)
	{
		printk("%s:error, sk buff is null!\n",__FUNCTION__);
		return;
	}
	bt_cb(skb)->pkt_type = type;
}
