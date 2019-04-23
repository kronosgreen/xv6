#include "types.h"
#include "pstat.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

unsigned long numsys = 0;
extern struct pstat *pstattable;

int shmem_count(int page_number);
void* shmem_access(int page_number, struct proc *proc);

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_getreadcount(void)
{
  return numsys;
}

int
sys_shmem_access(void) {
  int page_num;
  if(argint(0, &page_num) < 0)
    return -1;
  return (int)shmem_access(page_num, proc);
}

int sys_shmem_count(void) {
  int page_num;
  // illegal page number
  if(argint(0, &page_num) < 0)
    return -1;
  // call shmem_count in proc.c
  return shmem_count(page_num);
}

int sys_clone(void)
{
  int func_add;
  int arg;
  int stack_add;

  if (argint(0, &func_add) < 0)
     return -1;
  if (argint(1, &arg) < 0)
     return -1;
  if (argint(2, &stack_add) < 0)
     return -1;

  return clone((void *)func_add, (void *)arg, (void *)stack_add);

}

int sys_join(void)
{
  int stack_add;

  if (argint(0, &stack_add) < 0)
     return -1;

  return join((void **)stack_add);
}
