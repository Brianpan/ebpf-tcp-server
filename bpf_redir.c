#include "bpf_sockops.h"
#include <stdint.h>
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
    
    int ret;
    if (skb->remote_ip4 == skb->local_ip4) {
        // to find out socket from local port -> ebpf server
        struct sockmap_key skm_key = {
            .remote_port  = skb->local_port,
            .local_port = bpf_ntohl(skb->remote_port),
        };
        bpf_printk("redirect %d->%d", skb->local_port, bpf_ntohl(skb->remote_port));
        ret = bpf_sk_redirect_hash(skb, &sockmap_ops, &skm_key,
                                   BPF_F_INGRESS);
        if (ret != SK_PASS)
            ret = bpf_sk_redirect_hash(skb, &sockmap_ops, &skm_key,
                                   BPF_F_INGRESS);
        if (ret != SK_PASS)
            ret = bpf_sk_redirect_hash(skb, &sockmap_ops, &skm_key,
                                   BPF_F_INGRESS);
        // store a timestamp key: sockmap_key, ts
    } else {
        struct sockmap_key skm_key = {
            .family = skb->family,
            .remote_ip4 = skb->remote_ip4,
            .local_ip4 = skb->local_ip4,
            .remote_port = bpf_ntohl(skb->remote_port),
            .local_port = skb->local_port,
        };
        ret = bpf_sk_redirect_hash(skb, &sockmap_ops, &skm_key, 0);
    } 
    
    if (ret != SK_PASS) {
        bpf_printk("bpf_sk_redirect_hash() failed %d, error \n", -ret);
    } else {
        bpf_printk("bpf_sk_redirect_hash() success, redirected\n");
    }
        
    return ret;   
}

char _license[] SEC("license") = "GPL";
