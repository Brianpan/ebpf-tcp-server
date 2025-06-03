#include "bpf_sockops.h"

static inline void update_sockmap_ops(struct bpf_sock_ops *skops)
{
     struct sockmap_key skm_key = {
        .remote_port  = bpf_ntohl(skops->remote_port),
        .local_port = skops->local_port,
    };

    int ret;

    if (skops->remote_ip4 != skops->local_ip4) {
        skm_key.family = skops->family;
        skm_key.local_ip4 = skops->local_ip4;
        skm_key.remote_ip4 = skops->remote_ip4;
    }

    ret = bpf_sock_hash_update(skops, &sockmap_ops, &skm_key, BPF_NOEXIST);
    
    if (ret) {
        // bpf_printk("Update map failed. %d\n", -ret);
        return;
    }
    // bpf_printk("Update map success.\n");
}

SEC("sockops")
int bpf_sockmap(struct bpf_sock_ops *skops)
{
    if (skops->family != AF_INET)
        return 0;

    __u32 rport = bpf_ntohl(skops->remote_port);
    __u32 lport = skops->local_port;

    switch (skops->op) {
        // incoming connection established ACK
        case BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
        // outgoing connection established SYNACK
        case BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB:
            // bpf_printk("Add entry to SOCKHASH map op: %d => %u->%u : %lx (local_port : sk)\n", skops->op, lport, rport, bpf_get_socket_cookie(skops));
            update_sockmap_ops(skops);
            break;
        default:
            break;
    }

    return 0;
}

SEC("license") const char __license[] = "GPL";

