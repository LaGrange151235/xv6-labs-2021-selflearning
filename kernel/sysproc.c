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

// TODO: sys_pgaccess here.
#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 startingva;
  int pages;
  uint64 buffer;
  if(argaddr(0, &startingva) < 0)
	  return -1;
	if(argint(1, &pages) < 0)	
	  return -1;
  if(argaddr(2, &buffer) < 0)
  	return -1;

  // walk through the table
  struct proc *p = myproc();
  if (p == 0)
	  return -1;
  int bitmask = 0;
  uint64 va = startingva;
  for(int i = 0; i < pages; ++i) {
  	pte_t * pte;
  	pagetable_t pagetable = p->pagetable;
	  for (int level = 2; level >= 0; --level) {
		  pte = &pagetable[PX(level, va)];
		  if(*pte & PTE_V)
			  pagetable = (pagetable_t)PTE2PA(*pte);
	  }
	  if (pte != 0 && ((*pte) & PTE_A)) {
		  bitmask |= 1<<i;		// write in bitmask
		  *pte ^= PTE_A;			// clear PTE_A
	  }
	  va += PGSIZE;
  }
  copyout(p->pagetable, buffer, (char*)&bitmask, sizeof(int));
  return 0;
}
#endif

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
