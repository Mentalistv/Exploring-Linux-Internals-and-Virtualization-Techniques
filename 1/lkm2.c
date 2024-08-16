#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pid.h>

MODULE_AUTHOR("varn");
MODULE_DESCRIPTION("1.1     2. Lists child processes for given pid");

MODULE_LICENSE("GPL");

static int pid = -1; // Global variable to store input PID
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "Process ID to examine");

static int __init lkm2_init(void) {
    printk(KERN_ALERT "Module Loaded\n");

    struct pid *pid_struct;
    struct task_struct *task;
    struct list_head *list;

    if (pid == -1) {
        printk(KERN_ALERT "No process ID provided!\n");
        return -1;
    }

    // gets process structure
    pid_struct = find_get_pid(pid);
    if (!pid_struct) {
        printk(KERN_ALERT "Process with PID %d not found!\n", pid);
        return -1;
    }

    //  locate the task_struct associated with pid
    task = pid_task(pid_struct, PIDTYPE_PID);
    if (!task) {
        printk(KERN_ALERT "No child process for PID %d\n", pid);
        return -1;
    }

    printk(KERN_INFO "Process with PID %d:\n", task->pid);

    // iterate over children
    list_for_each(list, &task->children) {
        struct task_struct *child = list_entry(list, struct task_struct, sibling);
        printk(KERN_INFO "  Child PID: %d, State: %ld\n", child->pid, child->__state);
    }

    put_pid(pid_struct); 

    return 0;
}

static void __exit lkm2_exit(void) {
    printk(KERN_ALERT "Module Unloaded\n");
}

module_init(lkm2_init);
module_exit(lkm2_exit);

