#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
//#include <linux/moduleparam.h>
//#include <linux/init.h>
//#include <linux/stat.h>

#include <mem.h>

static int Major;
static int Device_is_open = 0;

static char msg [30];
static char *msg_ptr;

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
  //sprintf(mem, "I'm custom mem driver\n");
  //msg_ptr = msg;
  try_module_get(THIS_MODULE);
  return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file) {
  Device_is_open--;
  //printf("Device file closed!\n");
  module_put(THIS_MODULE);
  return SUCCESS;
}

static ssize_t device_read(struct file *filp, char *buf, size_t length, loff_t *offset ) {
  uint8_t tmp_buf [ 1000 ];
  if(length + *offset > 1000)
    length = 1000 - *offset;
  if(copy_to_user(buf,mem+*offset,length))
    return -EFAULT;
  return length;
}

static ssize_t device_write(struct file *filp, const char *buf, size_t length, loff_t *offset ) {
  if(length + *offset > 1000)
    length = 1000 - *offset;
  if(copy_from_user(mem+*offset,buf,length))
    return -EFAULT;
  return length;
}

static int __init init(void) {
  printk(KERN_INFO "Custom memory driver initialized!\n");
  Major = register_chrdev(0, DEVICE_NAME, &fops);
  if(Major < 0) {
    printk(KERN_ALERT "Custom memory driver registering failed!\n");
    return Major;
  }
  mem = kzalloc(1000, GFP_KERNEL);
  if(mem == NULL) {
    printk(KERN_ALERT "No mem can be allocated by driver!\n");
    return -ENOMEM;
  }
  printk(KERN_INFO "Custom memory driver registered!\n");
  printk(KERN_INFO "Major number: %d\n", Major);
  return SUCCESS;
}

static void __exit deinit(void) {
  unregister_chrdev(Major, DEVICE_NAME);
  kfree(mem);
  printk(KERN_INFO "Custom memory driver deinitialized!\n");
  return;
}

module_init(init);
module_exit(deinit);

MODULE_AUTHOR(AUTHOR);
MODULE_LICENSE(LICENSE);
MODULE_DESCRIPTION(DESC);

