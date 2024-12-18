#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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
  return myproc()->pid;
}

int
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
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_strace(void) {
    int enable;
    if (argint(0, &enable) < 0) {
        return -1;
    }

    struct proc *p = myproc();

    // Immediately disable strace if enable == 0
    if (enable == 0) {
        p->strace_enabled = 0;
    } else {
        p->strace_enabled = 1;
    }
    return 0;
}

int sys_strace_dump(void) {
    for (int i = 0; i < strace_count; i++) {
        int idx = (strace_index + i) % STRACE_BUFFER_SIZE;
        struct trace_event *event = &strace_buffer[idx];
        cprintf("TRACE EVENT: pid = %d | command_name = %s | syscall = %s | return_value = %d\n",
                event->pid, event->name, event->syscall, event->return_value);
    }
    return 0;
}
