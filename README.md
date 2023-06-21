# ebpf-echo-server

`ebpf-echo-server` uses eBPF to implement a simple echo server to receive and send packets in kernel space, which saves two memory copies (i.e. zero-copy) compared to building a server in user space. We don't need to write the kernel module or modify the kernel code at all.

## Build
```
$ make
```

## Load `bpf_sockops` eBPF program 
```
$ sudo bpftool prog load bpf_sockops.o /sys/fs/bpf/bpf_sockops
$ sudo bpftool prog show pinned /sys/fs/bpf/bpf_sockops
51: sock_ops  name bpf_sockmap  tag c289be3d02881e81  gpl
	loaded_at 2023-06-21T16:43:51+0800  uid 0
	xlated 440B  jited 253B  memlock 4096B  map_ids 5,7
	btf_id 110
```

## Pin eBPF maps
Mount a dedicated bpf file-system.
``` 
$ mkdir bpffs
$ sudo mount -t bpf none bpffs
```

Pin `sockmap_ops` map, a sockmap_ops file will be created in the bpffs.
```
$ sudo bpftool map show name sockmap_ops
5: sockhash  name sockmap_ops  flags 0x0
	key 16B  value 4B  max_entries 65535  memlock 1572864B
$ sudo bpftool map pin name sockmap_ops bpffs/sockmap_ops
```

## Attach `bpf_sockops` to cgroup
``` 
$ sudo bpftool cgroup attach /sys/fs/cgroup/ sock_ops pinned /sys/fs/bpf/bpf_sockops
```

## Load `bpf_redir` eBPF program 
Reuse `bpffs/sockmap_ops`.
```
$ sudo bpftool prog load bpf_redir.o /sys/fs/bpf/bpf_redir map name sockmap_ops pinned bpffs/sockmap_ops
$ sudo bpftool prog show pinned /sys/fs/bpf/bpf_redir
75: sk_skb  name bpf_redir  tag b32dde5a16e2cdf3  gpl
	loaded_at 2023-06-21T16:50:40+0800  uid 0
	xlated 752B  jited 435B  memlock 4096B  map_ids 5,19
	btf_id 137
```

## Attach `bpf_redir` to `sockmap_ops`
``` 
$ sudo bpftool prog attach pinned /sys/fs/bpf/bpf_redir stream_verdict pinned bpffs/sockmap_ops
```

## Run and test
Run the echo server.
```
$ ./ebpf-echo-server
``` 
If the output is "correct", it means that the string sent by the client to the server is the same as the echo message. It records time elapsed by sending and receiving the message.
``` 
$ ./bench
correct
```
