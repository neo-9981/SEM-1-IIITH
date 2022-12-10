#include<linux/syscalls.h>
#include<linux/kernel.h>
#include<linux/linkage.h>
#include<linux/uaccess.h>
#include<linux/sched.h>

SYSCALL_DEFINE0(aadeshprocess)
{
	printk("printing aadesh's system call\n");
	struct task_struct *parent = current->parent;
	printk("Process ID is -->> %ld\n", current->pid);
	printk("Parent ID is --->>%ld\n", parent->pid);
	return 0;

}
