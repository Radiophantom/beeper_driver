#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <asm/uaccess.h>

//#include <mem.h>

//----------------------------- Header -----------------------------------------
#define AUTHOR      "KriserX"
#define LICENSE     "GPL"
#define DESC        "Malloc memory and write/read its content"
#define DEVICES     "Simple memory driver"

#define SUCCESS     0
#define DEVICE_NAME "smem"
#define MAX_SIZE    256

//static int __init init(void);
//static void __exit deinit(void);

static int     device_open    (struct inode *, struct file *);
static int     device_release (struct inode *, struct file *);

static ssize_t device_read    (struct file *, char *,       size_t, loff_t *);
static ssize_t device_write   (struct file *, const char *, size_t, loff_t *);
//------------------------------------------------------------------------------

static int Major;
static int Device_is_open = 0;

//struct file_operations fops = {
//  .read    = device_read,
//  .write   = device_write,
//  .open    = device_open,
//  .release = device_release
//};

static int __init init(void) {
  printk(KERN_INFO "Custom memory driver initialized!\n");
  //Major = register_chrdev(0, DEVICE_NAME, &fops);
  //if(Major < 0) {
  //  printk(KERN_ALERT "Custom memory driver registering failed!\n");
  //  return Major;
  //printk(KERN_INFO "Custom memory driver registered!\n");
  return SUCCESS;
}

static void __exit deinit(void) {
  //int ret = unregister_chrdev(Major, DEVICE_NAME);
  //if(ret < 0 )
  //  printk(KERN_ALERT "Error in unregister_chrdev\n");
  printk(KERN_INFO "Custom memory driver deinitialized!\n");
  return;
}

static int device_open(struct inode *inode, struct file *file) {
  if(Device_is_open)
    return -EBUSY;
  Device_is_open ++;
  //printf("Device file opened!\n");
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
  //bytes_amount = 0;
  //bytes_amount++;
  //return bytes_amount;
  return length;
}

static ssize_t device_write(struct file *filp, const char *buf, size_t length, loff_t *offset ) {
  //bytes_amount = 0;
  //bytes_amount++;
  //return bytes_amount;
  return length;
}

module_init(init);
module_exit(deinit);

MODULE_AUTHOR(AUTHOR);
MODULE_LICENSE(LICENSE);
MODULE_DESCRIPTION(DESC);
//MODULE_SUPPORTED_DEVICE(DEVICES);
