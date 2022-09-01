#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/kernel.h>
//#include <linux/moduleparam.h>
#include <linux/init.h>
//#include <linux/stat.h>

#include <mem.h>

#define MM_SIZE 4096

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
static int     beeper_mmap  (struct file *file, struct vm_area_struct *vma                 );

static int platform_probe ( struct platform_device *pdev );
static int platform_remove( struct platform_device *pdev );

static const struct file_operations fops = {
	.owner    = THIS_MODULE,
        .open     = beeper_open,
        .release  = beeper_release,
	.read     = beeper_read,
	.write    = beeper_write,
        .llseek   = beeper_llseek,
        .mmap     = beeper_mmap
};

static int Major;
static int Device_is_open = 0;

static char *mem;
static char *mem_ptr;

static struct device *mychardev_dev;

static struct class *mychardev_class = NULL;

uint8_t sysfs_val = 41;

static int mychardev_uevent(struct device *dev, struct kobj_uevent_env *env) {
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return 0;
}

static ssize_t show_value(struct device *dev, struct device_attribute *attr, char *buf) {
  *buf = sysfs_val;
  return 1;
}

static ssize_t set_value(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
  sysfs_val = *buf;
  return count;
}

static ssize_t reset_value(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
  sysfs_val = 0;
  return count;
}

static DEVICE_ATTR(show,  S_IRUSR, show_value, NULL );
static DEVICE_ATTR(set,   S_IWUSR, NULL, set_value  );
static DEVICE_ATTR(reset, S_IWUSR, NULL, reset_value);

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
	int reg;
};

static struct my_device_platform_data my_device_pdata = {
	.reset_gpio = 101,
	.reg        = 0x00CC0000
};

static struct platform_device *my_device;

static int platform_probe( struct platform_device *pdev ) {
  struct my_device_platform_data *my_device_pdata;
  pr_info("platform_probe enter\n");
  Major = register_chrdev(0, DEVICE_NAME, &fops);
  if(Major < 0) {
    printk(KERN_ALERT "Custom memory driver registering failed!\n");
    return Major;
  }
  mem = kzalloc(MM_SIZE, GFP_KERNEL);
  if(mem == NULL) {
    printk(KERN_ALERT "No mem can be allocated by driver!\n");
    return -ENOMEM;
  }
  cdev_init(&my_dev.cdev, &fops);
  cdev_add(&my_dev.cdev, MKDEV(Major,0), 1);
  mychardev_class = class_create(THIS_MODULE,"mychardev");
  mychardev_class->dev_uevent = mychardev_uevent;
  mychardev_dev = device_create(mychardev_class, NULL, MKDEV(Major,0), NULL, "mychardev-0");
  device_create_file(mychardev_dev, &dev_attr_show);
  device_create_file(mychardev_dev, &dev_attr_set);
  device_create_file(mychardev_dev, &dev_attr_reset);

  my_device_pdata = pdev -> dev.platform_data;
  pr_info("reset_gpio: %d. reg: %d\n", my_device_pdata->reset_gpio, my_device_pdata->reg);
  pr_info("platform_probe exit\n");
  return 0;
}

static int platform_remove( struct platform_device *pdev ) {
  pr_info("platform_remove enter\n");
  device_remove_file(mychardev_dev, &dev_attr_show);
  device_remove_file(mychardev_dev, &dev_attr_set);
  device_remove_file(mychardev_dev, &dev_attr_reset);
  device_destroy(mychardev_class, MKDEV(Major,0));
  class_unregister(mychardev_class);
  class_destroy(mychardev_class);
  cdev_del(&my_dev.cdev);
  unregister_chrdev(Major, DEVICE_NAME);
  kfree(mem);
  pr_info("platform_remove exit\n");
  return 0;
}

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
  *offset += length;
  return length;
}

static ssize_t beeper_write(struct file *filp, const char *buf, size_t length, loff_t *offset ) {
  if(length + *offset > 1000)
    length = 1000 - *offset;
  if(copy_from_user(mem+*offset,buf,length))
    return -EFAULT;
  *offset += length;
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

static int beeper_mmap(struct file *file, struct vm_area_struct *vma) {
  vma->vm_flags |= ( VM_IO | VM_DONTDUMP | VM_DONTEXPAND );
  vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
  if(remap_pfn_range(vma,vma->vm_start,virt_to_phys(mem)>>PAGE_SHIFT,vma->vm_end - vma->vm_start, vma->vm_page_prot))
    return -EAGAIN;
  return 0;
}

static int beeper_init(void) {
  int ret_val;
  pr_info("beeper_init enter\n");
  my_device = platform_device_alloc("Custom BEEPER Driver",0);
  platform_device_add_data(my_device, &my_device_pdata, sizeof(my_device_pdata));
  platform_device_add(my_device);
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
  kfree(my_device->dev.platform_data);
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

