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

// Helper: Convert an integer to a string
int itoa(int n, char *s) {
    int i = 0, sign = n;

    if (sign < 0) n = -n; // Handle negative numbers

    // Generate digits in reverse order
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) s[i++] = '-'; // Add negative sign

    s[i] = '\0';

    // Reverse the string
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = s[j];
        s[j] = s[k];
        s[k] = temp;
    }
    return i; // Return the length of the string
}

int sys_strace_dump(void) {
    struct proc *p = myproc(); // Get the current process
    struct file *f = p->ofile[1]; // Get the file associated with stdout

    if (!f) {
        return -1; // No valid stdout file descriptor
    }

    // Determine how many events to display
    int num_events = strace_count;

    // Calculate the starting index for the oldest event
    int start_idx = (strace_index - num_events + STRACE_BUFFER_SIZE) % STRACE_BUFFER_SIZE;

    for (int i = 0; i < num_events; i++) {
        int idx = (start_idx + i) % STRACE_BUFFER_SIZE; // Circular buffer logic
        struct trace_event *event = &strace_buffer[idx];

        // Ensure the event is valid before printing
        if (event->pid == 0 && event->name[0] == '\0' && event->syscall[0] == '\0') {
            continue; // Skip uninitialized entries
        }

        // Build the trace event string manually
        char buf[128];
        int len = 0;

        safestrcpy(buf + len, "TRACE EVENT: pid = ", sizeof(buf) - len);
        len += strlen(buf + len); // Update len after copying

        len += itoa(event->pid, buf + len);

        safestrcpy(buf + len, " | command_name = ", sizeof(buf) - len);
        len += strlen(buf + len);

        safestrcpy(buf + len, event->name, sizeof(buf) - len);
        len += strlen(buf + len);

        safestrcpy(buf + len, " | syscall = ", sizeof(buf) - len);
        len += strlen(buf + len);

        safestrcpy(buf + len, event->syscall, sizeof(buf) - len);
        len += strlen(buf + len);

        safestrcpy(buf + len, " | return_value = ", sizeof(buf) - len);
        len += strlen(buf + len);

        len += itoa(event->return_value, buf + len);
        buf[len++] = '\n';

        // Write to stdout (which may be redirected)
        if (filewrite(f, buf, len) != len) {
            return -1; // Error writing to file
        }
    }
    return 0;
}


