#include<linux/syscalls.h>
#include<linux/kernel.h>
#include<linux/sched.h>

SYSCALL_DEFINE0(aadeshgetpid)
{
return task_tgid_vnr(current);
}
