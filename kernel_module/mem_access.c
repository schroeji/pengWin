#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mmap_lock.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <linux/seq_file.h> // seq_read, ..
#include <linux/string.h>
#include <linux/uaccess.h>

#define PROC_FILE_NAME "read_access"
#define GET_MAPS_MAGIC_NUMBER 0xFFFF
#define MAPS_BUFFER_SIZE 0x80000
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
    /* printk(KERN_INFO "%8d %8d %16s %8d", process->pid, process->tgid, */
    /*        process->comm, process->prio); */
    if (strncmp(process->comm, "cs2", 3) == 0) {
      result = process->pid;
    }
  }
  rcu_read_unlock();
  return result;
}

static ssize_t read_maps(struct file *file, char __user *user_buffer,
                         size_t count, loff_t *offset,
                         struct task_struct *task) {
  struct mm_struct *mm = task->mm;
  struct vm_area_struct *vma;
  if (mm == NULL) {
    printk(KERN_INFO "No memory descriptor found.");
    return -ENOMEM;
  }
  char *maps_buffer = kmalloc(MAPS_BUFFER_SIZE, GFP_KERNEL);
  // Lock the mm->mmap_lock mutex to safely access the memory mappings
  mmap_read_lock(mm);
  VMA_ITERATOR(iter, mm, 0);
  int cursor = 0;
  for_each_vma(iter, vma) {
    char filename[256];
    if (vma->vm_file) {
      strcpy(filename, vma->vm_file->f_path.dentry->d_name.name);
    } else { // implies an anonymous mapping i.e. not file backup'ed
      strcpy(filename, "[anon]");
    }
    printk(KERN_INFO "0x%lx-0x%lx %s", vma->vm_start, vma->vm_end, filename);
    if (cursor >= MAPS_BUFFER_SIZE) {
      break;
    }
    cursor = cursor + snprintf(&maps_buffer[cursor], MAPS_BUFFER_SIZE - cursor,
                               "0x%lx-0x%lx %s\n", vma->vm_start, vma->vm_end,
                               filename);
  }
  printk(KERN_INFO "%lx bytes were requested.", count);
  printk(KERN_INFO "Wrote total %x bytes to maps buffer.", cursor);
  mmap_read_unlock(mm);
  if (cursor < count) {
    count = cursor;
  }
  int ret = copy_to_user(user_buffer, maps_buffer, count);
  if (ret < 0) {
    printk(KERN_INFO "Copy to user failed for %lx bytes.", count);
  }
  printk(KERN_INFO "Copied %lx bytes to user buffer.", count);
  kfree(maps_buffer);

  return count;
}

static ssize_t read_cs2_mem(struct file *file, char __user *user_buffer,
                            size_t count, loff_t *offset) {
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
  if (!task) {
    printk(KERN_INFO "Task not found\n");
    return -ESRCH;
  }

  if (*offset ==
      GET_MAPS_MAGIC_NUMBER) { // special value for reading the maps file
    return read_maps(file, user_buffer, count, offset, task);
  }

  void *buffer = kmalloc(count, GFP_KERNEL);
  int ret = access_process_vm(task, *offset, buffer, count, 0);
  if (ret < 0) {
    printk(KERN_INFO "Failed to read memory\n");
    kfree(buffer);
    return ret;
  }
  ret = copy_to_user(user_buffer, buffer, count);
  if (ret < 0) {
    printk(KERN_INFO "Copy to user failed for %lx bytes.", count);
  }
  kfree(buffer);

  *offset = *offset + count;
  return count;
}

static struct proc_ops fops = {
    .proc_open = custom_open,
    .proc_read = read_cs2_mem,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};
// Custom init and exit methods
static int __init custom_init(void) {
  proc_create(PROC_FILE_NAME, S_IRUSR, NULL, &fops);
  printk(KERN_INFO "Hello world driver loaded.");
  return 0;
}
static void __exit custom_exit(void) {
  remove_proc_entry(PROC_FILE_NAME, NULL);
  printk(KERN_INFO "Goodbye my friend, I shall miss you dearly...");
}

module_init(custom_init);
module_exit(custom_exit);
