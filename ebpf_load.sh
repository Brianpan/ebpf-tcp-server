sudo bpftool prog load bpf_sockops.o /sys/fs/bpf/bpf_sockops
sudo mount -t bpf none bpffs
sudo bpftool map pin name sockmap_ops bpffs/sockmap_ops
sudo bpftool cgroup attach /sys/fs/cgroup/ sock_ops pinned /sys/fs/bpf/bpf_sockops
sudo bpftool prog load bpf_redir.o /sys/fs/bpf/bpf_redir map name sockmap_ops pinned bpffs/sockmap_ops
sudo bpftool prog attach pinned /sys/fs/bpf/bpf_redir stream_verdict pinned bpffs/sockmap_ops
