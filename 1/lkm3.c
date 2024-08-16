#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/sched.h>

MODULE_AUTHOR("varn");
MODULE_DESCRIPTION("1.1     3. Given pid and vaddr, determine paddr");

MODULE_LICENSE("GPL");

static long target_pid = 0;
module_param(target_pid, long, 0);

static unsigned long target_vaddr = 0;
module_param(target_vaddr, long, 0);

static unsigned long vaddr_to_paddr(struct mm_struct* mm, unsigned long vaddr){
    // page table walk

    pgd_t *pgd; 
    p4d_t *p4d; 
    pud_t *pud; 
    pmd_t *pmd; 
    pte_t *pte;

    unsigned long paddr = 0;
    int page_addr = 0;
    int page_offset = 0;

    // get offset from page global directory
    pgd = pgd_offset(mm, vaddr);
    if (pgd_none(*pgd)) {
        printk("not mapped in pgd\n");
        return -1;
    }

    // get offset from page directory
    p4d = p4d_offset(pgd, vaddr);
    if (p4d_none(*p4d)) {
        printk("not mapped in p4d\n");
        return -1;
    }

    // get offset from page upper directory
    pud = pud_offset(p4d, vaddr);
    if (pud_none(*pud)) {
        printk("not mapped in pud\n");
        return -1;
    }

    // get offset from page middle directory
    pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd)) {
        printk("not mapped in pmd\n");
        return -1;
    }

    // get pte 
    pte = pte_offset_kernel(pmd, vaddr);
    if (pte_none(*pte)) {
        printk("not mapped in ptes\n");
        return -1;
    }

    // paddr = [frame] + [page offset]
    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = vaddr & ~PAGE_MASK;
    paddr = page_addr | page_offset;
    
    return paddr;
}

static int __init lkm3_init(void)
{
    printk(KERN_ALERT "Module Loaded\n");

    struct pid *pid_struct = find_get_pid(target_pid);
    if (!pid_struct) {
        printk(KERN_ALERT "Process with PID %d not found!\n", target_pid);
        return -1;
    }

    struct task_struct *task = pid_task(pid_struct, PIDTYPE_PID);
    struct mm_struct *mm = get_task_mm(task);

    // calling the function
    unsigned long r = vaddr_to_paddr(mm, target_vaddr);

    printk(KERN_INFO "pid = %d : vir addr = %ld : phy addr = %ld\n", target_pid, target_vaddr, r);

    return 0;
}

static void __exit lkm3_exit(void)
{
    printk(KERN_ALERT "Module Unloaded\n");
}

module_init(lkm3_init);
module_exit(lkm3_exit);

