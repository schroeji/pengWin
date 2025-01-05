#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <linux/seq_file.h> // seq_read, ..
#include <linux/string.h>
#include <linux/uaccess.h>

// Module metadata
MODULE_AUTHOR("");
MODULE_DESCRIPTION("Hello world driver");
MODULE_LICENSE("GPL");

static int custom_show(struct seq_file *m, void *v) {
  seq_printf(m, "Hello proc!\n");
  return 0;
}

static int custom_open(struct inode *inode, struct file *file) {
  return single_open(file, custom_show, NULL);
}

static int get_cs_pid(void) {
  struct task_struct *process;
  // use Read-copy-update (RCU) to lock the list while we
  // traverse
  rcu_read_lock();
  int result = 0;
  for_each_process(process) {
    // lock the process task_struct while we access fields
    printk(KERN_INFO "%8d %8d %16s %8d", process->pid, process->tgid,
           process->comm, process->prio);
    if (strncmp(process->comm, "cs2", 3) == 0) {
      result = process->pid;
    }
  }
  rcu_read_unlock();
  return result;
}

static ssize_t custom_read(struct file *file, char __user *user_buffer,
                           size_t count, loff_t *offset) {
  printk(KERN_INFO "calling our very own custom read method.");
  if (offset == NULL || user_buffer == NULL) {
    printk(KERN_INFO "Offset is nullptr");
    return 0;
  }

  struct task_struct *task;
  struct pid *pid_struct;
  int pid = get_cs_pid();
  if (pid == 0) {
    printk(KERN_INFO "Could not find CS PID.");
    return 0;
  }
  pid_struct = find_get_pid(pid);
  task = get_pid_task(pid_struct, PIDTYPE_PID);
  task_lock(task);
  if (!task) {
    printk(KERN_INFO "Task not found\n");
    task_unlock(task);
    return -ESRCH;
  }
  printk(KERN_INFO "Reading %lu bytes from offset %lu. PID: %lu ; Name: %s",
         (unsigned long)count, (unsigned long)*offset, task->tgid, task->comm);
  /* void *buffer = kmalloc(count, GFP_KERNEL); */
  /* int ret = access_process_vm(task, *offset, buffer, count, 0); */
  /* if (ret < 0) { */
  /*   printk(KERN_INFO "Failed to read memory\n"); */
  /*   task_unlock(task); */
  /*   return ret; */
  /* } */
  task_unlock(task);

  *offset = *offset + count;
  return count;
}

static struct proc_ops fops = {
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
