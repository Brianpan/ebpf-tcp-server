sudo bpftool prog detach pinned /sys/fs/bpf/bpf_redir stream_verdict pinned bpffs/sockmap_ops
ret=$?
if [ ret -ne 0 ];then
echo "failed to detach prog"
fi

sudo bpftool cgroup detach /sys/fs/cgroup/ sock_ops pinned /sys/fs/bpf/bpf_sockops
ret=$?
if [ ret -ne 0 ];then
echo "failed to detach cgroup"
fi

sudo rm /sys/fs/bpf/bpf_redir
sudo rm /sys/fs/bpf/bpf_sockops
sudo rm bpffs/sockmap_ops
sudo umount bpffs
