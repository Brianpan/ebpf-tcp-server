#include "bpf_sockops.h"

static inline void update_sockmap_ops(struct bpf_sock_ops *skops)
{
    struct sockmap_key skm_key = {
        .family = skops->family,
        .remote_ip4 = skops->remote_ip4,
        .local_ip4  = skops->local_ip4,
        .remote_port  = bpf_ntohl(skops->remote_port),
        .local_port = skops->local_port,
    };

    int ret;
    ret = bpf_sock_hash_update(skops, &sockmap_ops, &skm_key, BPF_NOEXIST);
    
    if (ret) {
        bpf_printk("Update map failed. %d\n", -ret);
        return;
    }
    bpf_printk("Update map success.\n");
}

SEC("sockops")
int bpf_sockmap(struct bpf_sock_ops *skops)
{
    if (skops->family != AF_INET)
        return 0;

    switch (skops->op) {
        case BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
        case BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB:
            update_sockmap_ops(skops);
            break;
        default:
            break;
    }

    return 0;
}

SEC("license") const char __license[] = "GPL";

