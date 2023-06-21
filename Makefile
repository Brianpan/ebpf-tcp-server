CC := clang
CFLAGS := -g -O2 -Wall -Wextra

PROGS := ebpf-echo-server bench bpf_sockops.o bpf_redir.o
.PHONY: all
all: $(PROGS)

ebpf-echo-server: ebpf-echo-server.c
	gcc $< -O2 -o $@
bench: bench.c
	gcc $< -O2 -o $@
bpf_sockops.o: bpf_sockops.c
	$(CC) $(CFLAGS) -target bpf -c -o $@ $<
bpf_redir.o: bpf_redir.c
	$(CC) $(CFLAGS) -target bpf -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(PROGS)
