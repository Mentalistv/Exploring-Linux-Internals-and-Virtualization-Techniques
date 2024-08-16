#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_AUTHOR("varn");
MODULE_DESCRIPTION("1.1     1. Lists running/runnable processes");

MODULE_LICENSE("GPL");

// init_module
static int __init lkm1_init(void) {
    struct task_struct *task;

    printk(KERN_ALERT "Module Loaded\n");

    // for_each_process: a way to iterate through all active processes
    for_each_process(task) { 
        if (task->__state == TASK_RUNNING) {
            printk(KERN_INFO "PID: %d\n", task->pid);
        }
    }

    return 0;
}

//cleanup_module
static void __exit lkm1_exit(void) {
    printk(KERN_ALERT "Module Unloaded\n");
}

module_init(lkm1_init);
module_exit(lkm1_exit);

