#include "bpf_sockops.h"

#define SERVER_PORT 12345
   
SEC("sk_skb/stream_verdict")
int bpf_redir(struct __sk_buff * skb)
{
    if (skb->family != AF_INET)
        return SK_PASS;
        
    if (skb->local_port != SERVER_PORT)
        return SK_PASS;
    
    if (skb->len == 0)
        return SK_PASS;

    struct sockmap_key skm_key = {
        .family = skb->family,
        .remote_ip4 = skb->remote_ip4,
        .local_ip4  = skb->local_ip4,
        .remote_port  = bpf_ntohl(skb->remote_port),
        .local_port = skb->local_port,
    };
    
    int ret;
    ret = bpf_sk_redirect_hash(skb, &sockmap_ops, &skm_key, 0); 
    
    if (ret != SK_PASS)
        bpf_printk("bpf_sk_redirect_hash() failed %d, error \n", -ret);
    
    bpf_printk("bpf_msg_redirect_hash() failed %d, error %d --> %d\n", -ret, bpf_ntohl(skb->remote_port), skb->local_port);
        
    return ret;   
}

char _license[] SEC("license") = "GPL";
