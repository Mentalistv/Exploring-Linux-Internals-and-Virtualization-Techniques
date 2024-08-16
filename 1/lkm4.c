#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/pid.h>
#include <linux/proc_fs.h>

static struct proc_dir_entry *proc_entry;

static long target_pid = 0;
module_param(target_pid, long, 0);

MODULE_AUTHOR("varn");
MODULE_DESCRIPTION("1.1     4. Size of the allocated virtual address space and the mapped physical address space");

MODULE_LICENSE("GPL");

static void get_memory_info(struct mm_struct* mm)
{
    unsigned long total_virtual_size = 0;
    unsigned long total_physical_size = 0;

    // get vas
    total_virtual_size =  mm->total_vm;

    //get pas
    total_physical_size += get_mm_counter(mm, MM_FILEPAGES) + get_mm_counter(mm, MM_SHMEMPAGES) + get_mm_counter(mm, MM_ANONPAGES);
    // + get_mm_counter(mm, NR_MM_COUNTERS) + get_mm_counter(mm, MM_SWAPENTS);

    printk("Virtual address space: %lu KB\n", total_virtual_size*4);
    printk("Physical address space: %lu KB\n", total_physical_size*4);
}

static int __init lkm4_init(void)
{
    printk(KERN_ALERT "Module Loaded\n");

    if (target_pid == -1) {
        return -1;
    }

    struct task_struct *task = get_pid_task(find_get_pid(target_pid), PIDTYPE_PID);
    if (!task) {
        printk("Process not found\n");
        return -1;
    }
    struct mm_struct *mm = get_task_mm(task);

    // calling the function
    get_memory_info(mm);

    return 0;
}

static void __exit lkm4_exit(void)
{
    printk(KERN_ALERT "Module Unloaded\n");
}

module_init(lkm4_init);
module_exit(lkm4_exit);
