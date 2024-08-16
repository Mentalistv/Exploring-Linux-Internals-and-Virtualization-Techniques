#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>  // For all_vm_events

MODULE_LICENSE("GPL");
MODULE_AUTHOR("varn");

static struct proc_dir_entry *proc_entry;

// shows page fault since boot
static int get_pgfaults_show(struct seq_file *m, void *v) {
    unsigned long s[NR_VM_EVENT_ITEMS];
    all_vm_events(s);
    seq_printf(m, "Page Faults - %ld\n", s[PGFAULT]);
    return 0;
}

static int custom_faults_open(struct inode *inode, struct file *file) {
    return single_open(file, get_pgfaults_show, NULL);
}

static const struct proc_ops get_pgfaults_fops = {
    .proc_open = custom_faults_open,
    .proc_read = seq_read,
    .proc_release = single_release,
};

static int __init get_pgfaults_init(void) {
    proc_entry = proc_create("get_pgfaults", 0, NULL, &get_pgfaults_fops);
    if (!proc_entry) {
        return -1;
    }
    return 0;
}

static void __exit get_pgfaults_exit(void) {
    remove_proc_entry("get_pgfaults", NULL);
}

module_init(get_pgfaults_init);
module_exit(get_pgfaults_exit);

