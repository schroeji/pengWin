#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>  // seq_read, ..
#include <linux/ptrace.h>
#include <linux/sched.h>


// Module metadata
MODULE_AUTHOR("");
MODULE_DESCRIPTION("Hello world driver");
MODULE_LICENSE("GPL");

static int custom_show(struct seq_file *m, void *v) {
  seq_printf(m, "Hello proc!\n");
  return 0;
}

static int custom_open(struct inode *inode, struct  file *file) {
  return single_open(file, custom_show, NULL);
}

static pid_t cs_pid(void) {
  return 28630;
}

static ssize_t custom_read(struct file* file, char __user* user_buffer, size_t count, loff_t* offset)
{
 printk(KERN_INFO "calling our very own custom read method.");
 if (offset == NULL || user_buffer == NULL) {
    printk(KERN_INFO "Offset is nullptr");
    return 0;
 }


 struct task_struct *task;
 struct pid *pid_struct;
 pid_struct = find_get_pid(cs_pid());
 task = get_pid_task(pid_struct, PIDTYPE_PID);
 task_lock(task);
 if (!task) {
   printk(KERN_INFO "Task not found\n");
   task_unlock(task);
   return -ESRCH;
 }
 printk(KERN_INFO "Reading %lu bytes from offset %lu. PID: %lu ; Name: %s", (unsigned long)count, (unsigned long)*offset, task->tgid, task->comm);
 void* buffer = kmalloc(count, GFP_KERNEL);
 int ret = access_process_vm(task, *offset, buffer, count, 0);
 if (ret < 0) {
   printk(KERN_INFO "Failed to read memory\n");
   task_unlock(task);
   return ret;
 }
 task_unlock(task);

 *offset = *offset+count;
 return count;
}


static struct proc_ops fops =
{
  .proc_open = custom_open,
  .proc_read = custom_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};
// Custom init and exit methods
static int __init custom_init(void) {
 proc_create("helloworlddriver", 0666, NULL, &fops);
 printk(KERN_INFO "Hello world driver loaded.");
 return 0;
}
static void __exit custom_exit(void) {
 remove_proc_entry("helloworlddriver", NULL);
 printk(KERN_INFO "Goodbye my friend, I shall miss you dearly...");
}

module_init(custom_init);
module_exit(custom_exit);
