#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>


#define FIRST_MINOR 0
#define MINOR_CNT 1

#define DEVICE_NAME "driver"
#define IOCTL_GET_PHYSICAL_ADDR _IOR('k', 1, unsigned long)
#define IOCTL_WRITE_PHYSICAL_ADDR _IOW('k', 2, unsigned long)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");

struct WritePhysicalParams {
    unsigned long virt_addr;
    char value;
};

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static int device_open(struct inode *inode, struct file *file) {
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;
}

static unsigned long vaddr_to_paddr(struct mm_struct* mm, unsigned long vaddr){
    // page table walk; explained in lkm3.c
    pgd_t *pgd; 
    p4d_t *p4d; 
    pud_t *pud; 
    pmd_t *pmd; 
    pte_t *pte;

    unsigned long paddr = 0;
    int page_addr = 0;
    int page_offset = 0;

    pgd = pgd_offset(mm, vaddr);
    if (pgd_none(*pgd)) {
        printk("not mapped in pgd\n");
        return -1;
    }

    p4d = p4d_offset(pgd, vaddr);
    if (p4d_none(*p4d)) {
        printk("not mapped in p4d\n");
        return -1;
    }

    pud = pud_offset(p4d, vaddr);
    if (pud_none(*pud)) {
        printk("not mapped in pud\n");
        return -1;
    }

    pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd)) {
        printk("not mapped in pmd\n");
        return -1;
    }

    pte = pte_offset_kernel(pmd, vaddr);
    if (pte_none(*pte)) {
        printk("not mapped in ptes\n");
        return -1;
    }

    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = vaddr & ~PAGE_MASK;
    paddr = page_addr | page_offset;
    
    return paddr;
}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
    unsigned long virt_addr, phys_addr;
    void *kaddr;

    switch (ioctl_num) {
        case IOCTL_GET_PHYSICAL_ADDR:
            // case 1: getting p addr; using page table walk
            if (copy_from_user(&virt_addr, (unsigned long *)ioctl_param, sizeof(unsigned long)))
                return -EFAULT;

            struct pid *pid_struct = find_get_pid(current->pid);
            if (!pid_struct) {
                printk(KERN_ALERT "Process with PID %d not found!\n", current->pid);
                return -1;
            }

            struct task_struct *task = pid_task(pid_struct, PIDTYPE_PID);

            struct mm_struct *mm = get_task_mm(task);

            unsigned long r = vaddr_to_paddr(mm, virt_addr);

            kaddr = (void *)r;
            
            return copy_to_user((unsigned long *)ioctl_param, &kaddr, sizeof(unsigned long)) ? -EFAULT : 0;

        case IOCTL_WRITE_PHYSICAL_ADDR:
            // case 2: modifying value present at provided p addr
            if (copy_from_user(&phys_addr, (unsigned long *)ioctl_param, sizeof(unsigned long)))
                return -EFAULT;

            struct WritePhysicalParams *params;
            params = (struct WritePhysicalParams *)ioctl_param;
            unsigned long phys_addr = virt_to_phys((void *)params->virt_addr);

            // converting it to virtual addr; then changing the value
            kaddr = (void *)phys_to_virt((unsigned long)phys_addr);
            int * temp = (int *)kaddr;

            *temp = params->value;
            return 0;

        default:
            return -ENOTTY;
    }
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl,
};

static int __init mem_ioctl_init(void) {
    int ret;
    struct device *dev_ret;

    printk(KERN_ALERT "Module Loaded.\n");

    //  dynamically allocates a range of character device numbers
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, DEVICE_NAME)) < 0)
    {
        return ret;
    }
 
    cdev_init(&c_dev, &fops);
 
    // registers the device driver
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
    
    
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, DEVICE_NAME)))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
 
    return 0;
}

static void __exit mem_ioctl_exit(void) {
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    
    printk(KERN_ALERT "Module Unloaded.\n");
}

module_init(mem_ioctl_init);
module_exit(mem_ioctl_exit);
