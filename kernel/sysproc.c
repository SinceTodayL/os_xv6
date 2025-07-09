#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#define MAX_PGACCESS 64 

uint64
sys_pgaccess(void)
{
  uint64 va_start, user_mask_ptr;
  int npages;

  if (argaddr(0, &va_start) < 0 ||
      argint(1, &npages) < 0 ||
      argaddr(2, &user_mask_ptr) < 0)
    return -1;

  if (npages <= 0 || npages > MAX_PGACCESS)
    return -1;

  struct proc *p = myproc();
  uint64 mask = 0;

  for (int i = 0; i < npages; i++) {
    uint64 va = va_start + i * PGSIZE;
    pte_t *pte = walk(p->pagetable, va, 0);
    if (pte && (*pte & PTE_A)) {
      mask |= (1UL << i);  
      *pte &= ~PTE_A;        
    }
  }

  if (copyout(p->pagetable, user_mask_ptr, (char *)&mask, sizeof(mask)) < 0)
    return -1;

  return 0;
}
