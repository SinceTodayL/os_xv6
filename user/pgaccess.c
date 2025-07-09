#include "user/user.h"
#include "kernel/types.h"

uint64
pgaccess(void *addr, int npages, uint64 *mask)
{
  return syscall(SYS_pgaccess, addr, npages, mask);
}
