#ifndef __BPF_COMPILER_H_
#define __BPF_COMPILER_H_

static inline int bpf(enum bpf_cmd cmd, union bpf_attr *attr,
                      unsigned int size)
{
  return syscall(__NR_bpf, cmd, attr, size);
}

#endif /* __BPF_COMPILER_H_ */
