#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE1(aadeshprint, char __user *,buf)
{
    printk("In syscall aadeshprint\n");
    printk("syscall aadheshprint gives output: %s\n", buf);
    return 0;
}
