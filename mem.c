#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
//#include <linux/moduleparam.h>
//#include <linux/init.h>
//#include <linux/stat.h>

#include <mem.h>

static int beeper_probe (struct platform_device *pdev);
static int beeper_remove(struct platfrom_device *pdev);

static int ssize_t beeper_read (struct file *file, char *buf,       size_t len, loff_t *offset);
static int ssize_t beeper_write(struct file *file, const char *buf, size_t len, loff_t *offset);

struct beeper_dev {
	struct miscdevice miscdev;
	void __iomem *regs;
	u8 leds_value;
};

static struct of_device_id beeper_dt_ids[] = {
	{
		.compatible = "dev,beeper"
	},
	{ /* end of table */ }
};

MODULE_DEVICE_TABLE(of, beeper_dt_ids);

static struct platform_driver beeper_platform = {
	.probe  = beeper_probe,
	.remove = beeper_remove,
	.driver = {
		.name "Custom BEEPER Driver",
		.owner = THIS_MODULE,
		.of_match_table = beeper_dt_ids
	}
};

static int __init beeper_init(void) {
	ret = platform_driver_register(&beeper_platform);
}

static void __exit beeper_cleanup(void) {
	platform_driver_unregister(&beeper_platform);
}

module_init(beeper_init);
module_exit(beeper_cleanup);

static const struct file_operations beeper_fops = {
	.owner = THIS_MODULE,
	.read  = beeper_read,
	.write = beeper_write
};

static int Major;
static int Device_is_open = 0;

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

MODULE_AUTHOR(AUTHOR);
MODULE_LICENSE(LICENSE);
MODULE_DESCRIPTION(DESC);

