#include <linux/module.h>
//#include <linux/moduleparam.h>
#include <linux/kernel.h>
//#include <linux/init.h>
#include <linux/fs.h>
//#include <linux/stat.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#include <mem.h>

static int Major;
static int Device_is_open = 0;

struct file_operations fops = {
  .read    = device_read,
  .write   = device_write,
  .open    = device_open,
  .release = device_release
};

static char *mem;

static int device_open(struct inode *inode, struct file *file) {
  if(Device_is_open)
    return -EBUSY;
  Device_is_open ++;
  if((mem = kmalloc(1000, GFP_KERNEL)) == NULL){
    printk(KERN_ALERT "No mem can be allocated by driver!\n");
    return -ENOMEM;
  }
  //printf("Device file opened!\n");
  try_module_get(THIS_MODULE);
  return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file) {
  Device_is_open--;
  //printf("Device file closed!\n");
  kfree(mem);
  module_put(THIS_MODULE);
  return SUCCESS;
}

static ssize_t device_read(struct file *filp, char *buf, size_t length, loff_t *offset ) {
  char *mem_ptr = mem;
  int  bytes_read = 0;
  while(length) {
    put_user(*(mem_ptr++),buf++);
    length--;
    bytes_read++;
  }
  return bytes_read;
}

static ssize_t device_write(struct file *filp, const char *buf, size_t length, loff_t *offset ) {
  char *mem_ptr = mem;
  char *buf_ptr = buf;
  int  bytes_written = 0;
  while(length) {
    get_user(*buf_ptr++,mem_ptr++);
    length--;
    bytes_written++;
  }
  return bytes_written;
}

static int __init init(void) {
  printk(KERN_INFO "Custom memory driver initialized!\n");
  Major = register_chrdev(0, DEVICE_NAME, &fops);
  if(Major < 0) {
    printk(KERN_ALERT "Custom memory driver registering failed!\n");
    return Major;
  }
  printk(KERN_INFO "Custom memory driver registered!\n");
  return SUCCESS;
}

static void __exit deinit(void) {
  unregister_chrdev(Major, DEVICE_NAME);
  printk(KERN_INFO "Custom memory driver deinitialized!\n");
  return;
}

module_init(init);
module_exit(deinit);

MODULE_AUTHOR(AUTHOR);
MODULE_LICENSE(LICENSE);
MODULE_DESCRIPTION(DESC);

