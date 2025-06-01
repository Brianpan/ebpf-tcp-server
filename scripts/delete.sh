sudo bpftool prog detach pinned /sys/fs/bpf/bpf_redir stream_verdict pinned bpffs/sockmap_ops
sudo bpftool cgroup detach /sys/fs/cgroup/ sock_ops pinned /sys/fs/bpf/bpf_sockops
sleep 1
sudo rm /sys/fs/bpf/bpf_sockops
sudo rm /sys/fs/bpf/bpf_redir
sudo rm bpffs/sockmap_ops
sudo umount bpffs
