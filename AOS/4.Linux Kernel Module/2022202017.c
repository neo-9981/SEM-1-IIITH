#include<linux/module.h>

// The following includes are not initially required, but have been included for Assignment #3 - Sp 2020
#include<linux/sched/signal.h>
#include<linux/pid_namespace.h>
#include<linux/cdev.h>
#include<linux/proc_fs.h>
#include<linux/slab.h>

void PrintProces(void)
{
   struct task_struct *itr;
  int long rn,itrt,untrt;
   rn=0;
   itrt=0;
   untrt=0;
for_each_process(itr)
{
   if(itr->state==0)
      {rn+=1;}
   if(itr->state==1)
     {itrt+=1;}
   if(itr->state==2)
    {untrt+=1;}
}
   printk(KERN_INFO "Count of Running Process is %ld\n",rn);
   printk(KERN_INFO "Count of Interrupted Process is %ld\n",itrt);
   printk(KERN_INFO "Count of UnInterrupted Process is %ld\n",untrt);
 
  return;


}
int proc_init (void) {
  printk(KERN_INFO "helloModule: kernel module initialized\n");
// Calling Function to Print Count of Various Process
PrintProces();
  return 0;
}

void proc_cleanup(void) {
  printk(KERN_INFO "helloModule: performing cleanup of module\n");
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);

