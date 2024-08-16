#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/memory.h>
#include <linux/list.h>

#include "ioctl_driver.h"  // Include the header file

MODULE_AUTHOR("varn");
MODULE_DESCRIPTION("ioctl driver for Dr. Doom's soldier control");

MODULE_LICENSE("GPL");

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static int ioctl_driver_open(struct inode *inode, struct file *file) {
    return 0;
}

static int ioctl_driver_release(struct inode *inode, struct file *file) {
    return 0;
}

static long ioctl_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int arg_copy;
    struct task_struct *curr;
    struct task_struct *new_parent;

    struct task_struct *child;
    struct list_head *it, *temp;

    // since there's only one cmd; no need for switch

    if (cmd == IOCTL_CHANGE_PARENT) {
        rcu_read_lock();
        if (copy_from_user(&arg_copy, (int *)arg,  sizeof(int))) {
            return -EACCES;
        }

        // process whose parent has to be changed
        curr = current;

        // new parent for the 'curr'
        new_parent = pid_task(find_vpid(arg_copy), PIDTYPE_PID);

        // Delete curr from it's parent's children list
        list_for_each_safe(it, temp,  &current->parent->children) {
            child = list_entry(it, struct task_struct, sibling);
            if (child == curr) {
                list_del(it);
                break;
            }
        }

        // Add curr to parent's children list
        list_add_tail(&curr->sibling, &new_parent->children);

        // Change the parent of curr
        curr->real_parent = new_parent;
        curr->parent = new_parent;
        
        rcu_read_unlock();

        return 0;
    }

    return -ENOTTY;  // Invalid ioctl command
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = ioctl_driver_open,
    .release = ioctl_driver_release,
    .unlocked_ioctl = ioctl_driver_ioctl,
};

static int __init ioctl_driver_init(void) {
    int ret;
    struct device *dev_ret;
 
    printk(KERN_ALERT "Module Loaded.\n");
    
    if ((ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME)) < 0)
    {
        return ret;
    }
 
    cdev_init(&c_dev, &fops);
 
    if ((ret = cdev_add(&c_dev, dev, 1)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, DEVICE_NAME)))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(dev_ret);
    }

    return 0;
}

static void __exit ioctl_driver_exit(void) {
    // Remove device from the system
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, 1);
    
    printk(KERN_ALERT "Module Unloaded.\n");
}

module_init(ioctl_driver_init);
module_exit(ioctl_driver_exit);

