#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
//#include <linux/kernel.h>
//#include <linux/moduleparam.h>
//#include <linux/init.h>
//#include <linux/stat.h>

#include <mem.h>

struct my_device_data {
	struct cdev cdev;
        char my_device_name[30];
        char *my_ptr;
};

struct my_device_data my_dev;

static int     beeper_open   (struct inode *inode, struct file *file);
static int     beeper_release(struct inode *inode, struct file *file);

static ssize_t beeper_read  (struct file *file, char *buf,       size_t len, loff_t *offset);
static ssize_t beeper_write (struct file *file, const char *buf, size_t len, loff_t *offset);
static loff_t  beeper_llseek(struct file *file, loff_t offset,   int whence                );

static const struct file_operations fops = {
	.owner    = THIS_MODULE,
        .open     = beeper_open,
        .release  = beeper_release,
	.read     = beeper_read,
	.write    = beeper_write,
        .llseek   = beeper_llseek
};

static int Major;
static int Device_is_open = 0;

static char *mem;
static char *mem_ptr;

static struct class *mychardev_class = NULL;

static int platform_probe( struct platform_device *pdev ) {
  pr_info("platform_probe enter\n");
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
  cdev_init(&my_dev.cdev, &fops);
  cdev_add(&my_dev.cdev, MKDEV(Major,0), 1);
  mychardev_class = class_create(THIS_MODULE,"mychardev");
  device_create(mychardev_class, NULL, MKDEV(Major,0), NULL, "mychardev-0");
  pr_info("platform_probe exit\n");
  return 0;
}

static int platform_remove( struct platform_device *pdev ) {
  pr_info("platform_remove enter\n");
  device_destroy(mychardev_class, MKDEV(Major,0));
  class_destroy(mychardev_class);
  cdev_del(&my_dev.cdev);
  unregister_chrdev(Major, DEVICE_NAME);
  kfree(mem);
  pr_info("platform_remove exit\n");
  return 0;
}

static struct of_device_id beeper_driver_dt_ids[] = {
  {
    .compatible = "dev,beeper"
  },
  { /* end of table */ }
};

MODULE_DEVICE_TABLE(of, beeper_driver_dt_ids);

static struct platform_driver beeper_platform_driver = {
	.probe  = platform_probe,
	.remove = platform_remove,
	.driver = {
		  .name = "Custom BEEPER Driver",
		  .owner = THIS_MODULE,
		  .of_match_table = beeper_driver_dt_ids
	          },
};

struct my_device_platform_data {
	int reset_gpio;
};

static struct my_device_platform_data my_device_pdata = {
	.reset_gpio = 101
};

static struct platform_device *my_device;
//= {
//	.name = "Custom BEEPER Driver",
//	.id   = PLATFORM_DEVID_NONE,
//	.dev.platform_data = &my_device_pdata
//};

static int beeper_open(struct inode *inode, struct file *file) {
  struct my_device_data *my_data;
  pr_info("device_open enter\n");
  if(Device_is_open)
    return -EBUSY;
  Device_is_open ++;
  my_data = container_of(inode->i_cdev, struct my_device_data, cdev);
  file -> private_data = my_data;
  try_module_get(THIS_MODULE);
  pr_info("device_open exit\n");
  return 0;
}

static int beeper_release(struct inode *inode, struct file *file) {
  pr_info("device_release enter\n");
  Device_is_open--;
  module_put(THIS_MODULE);
  pr_info("device_release exit\n");
  return 0;
}

static ssize_t beeper_read(struct file *filp, char *buf, size_t length, loff_t *offset ) {
  if(length + *offset > 1000)
    length = 1000 - *offset;
  if(copy_to_user(buf,mem+*offset,length))
    return -EFAULT;
  return length;
}

static ssize_t beeper_write(struct file *filp, const char *buf, size_t length, loff_t *offset ) {
  if(length + *offset > 1000)
    length = 1000 - *offset;
  if(copy_from_user(mem+*offset,buf,length))
    return -EFAULT;
  return length;
}

static loff_t  beeper_llseek(struct file *file, loff_t offset, int whence) {
  loff_t new_offset;
  switch(whence) {
    case SEEK_SET:
      new_offset = offset;
      break;
    case SEEK_CUR:
      new_offset = file->f_pos + offset;
      break;
    //case SEEK_END:
      //new_offset = FILE_SIZE + offset;
      //break;
    default:
      return -EINVAL;
  }
  file->f_pos = new_offset;
  return new_offset;
}

static int beeper_init(void) {
  int ret_val;
  pr_info("beeper_init enter\n");
  my_device = platform_device_alloc("Custom BEEPER Driver",0);
  platform_device_add(my_device);
  //platform_device_register(&my_device);
  pr_info("platform device registered!\n");
  ret_val = platform_driver_register(&beeper_platform_driver);
  if(ret_val != 0) {
    pr_err("platfrom_driver_register returned %d\n", ret_val);
    return ret_val;
  }
  pr_info("platfrom driver registered\n");
  pr_info("beeper_init exit\n");
  return 0;
}

static void beeper_cleanup(void) {
  pr_info("beeper_cleanup enter\n");
  platform_device_unregister(my_device);
  pr_info("platform device unregistered\n");
  platform_driver_unregister(&beeper_platform_driver);
  pr_info("platform driver unregistered\n");
  pr_info("beeper_cleanup exit\n");
}

module_init(beeper_init);
module_exit(beeper_cleanup);

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESC);
MODULE_VERSION("1.0");

