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
//#include <linux/kernel.h>
//#include <linux/init.h>

#include <beeper_drv.h>

static int Major;

static struct device  *beeper_dev;
static struct class   *beeper_class = NULL;

struct my_device_data my_dev;

static const struct file_operations fops = {
	.owner    = THIS_MODULE,
        .open     = beeper_open,
        .release  = beeper_release,
	.read     = beeper_read,
	.write    = beeper_write,
        .llseek   = beeper_llseek,
        .mmap     = beeper_mmap
};

static int beeper_uevent(struct device *dev, struct kobj_uevent_env *env) {
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return 0;
}

static ssize_t set_time(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
  if(count > 4)
    count = 4;
  if(copy_from_user(time_reg,buf,count))
    return -EFAULT;
  return count;
}

static ssize_t get_time(struct device *dev, struct device_attribute *attr, char *buf) {
  if(copy_to_user(buf,time_reg,4))
    return -EFAULT;
  return 1;
}

static ssize_t set_mode(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
  switch(buf) {
    case "ON":
      mode_reg = 0x00;
      break;
    case 1:
      mode_reg = 0x01;
      break;
    case 2:
      mode_reg = 0x02;
      break;
    default:
      mode_reg = 0x00;
  }
  return count;
}

static ssize_t get_time(struct device *dev, struct device_attribute *attr, char *buf) {
  switch(buf) {
    case 0:
      mode_reg = 0x00;
      break;
    case 1:
      mode_reg = 0x01;
      break;
    case 2:
      mode_reg = 0x02;
      break;
    default:
      mode_reg = 0x00;
  }
  return count;
  if(copy_to_user(buf,time_reg,4))
    return -EFAULT;
  return 1;
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

// ?????
/* get RESOURCE from platform device */
get_resources

MODULE_DEVICE_TABLE(of, beeper_driver_dt_ids);

static struct platform_driver beeper_platform_driver = {
	.probe  = platform_probe,
	.remove = platform_remove,
	.driver = {
		  .name           = "Custom BEEPER Driver",
		  .owner          = THIS_MODULE,
		  .of_match_table = beeper_driver_dt_ids
	          },
};

static int platform_probe( struct platform_device *pdev ) {
  struct my_device_platform_data *my_device_pdata;
  Major = register_chrdev(0, DEVICE_NAME, &fops);
  if(Major < 0) {
    printk(KERN_ALERT "Beeper platform driver char-device registering failed!\n");
    return Major;
  }
  cdev_init(&my_dev.cdev, &fops);
  cdev_add(&my_dev.cdev, MKDEV(Major,0), 1);
  beeper_class = class_create(THIS_MODULE,"beeper");
  beeper_class->dev_uevent = beeper_uevent;
  beeper_dev = device_create(beeper_class, NULL, MKDEV(Major,0), NULL, "beeper-0");
  device_create_file(beeper_dev, &dev_attr_show);
  device_create_file(beeper_dev, &dev_attr_set);
  device_create_file(beeper_dev, &dev_attr_reset);

  my_device_pdata = pdev -> dev.platform_data;
  pr_info("reset_gpio: %d. reg: %d\n", my_device_pdata->reset_gpio, my_device_pdata->reg);
  pr_info("platform_probe exit\n");
  return 0;
}

static int platform_remove( struct platform_device *pdev ) {
  device_remove_file(beeper_dev, &dev_attr_show);
  device_remove_file(beeper_dev, &dev_attr_set);
  device_remove_file(beeper_dev, &dev_attr_reset);
  device_destroy(beeper_class, MKDEV(Major,0));
  // ???
  // class_unregister(beeper_class);
  class_destroy(beeper_class);
  cdev_del(&my_dev.cdev);
  unregister_chrdev(Major, DEVICE_NAME);
  return 0;
}

static int beeper_open(struct inode *inode, struct file *file) {
  struct my_device_data *my_data;
  my_data = container_of(inode->i_cdev, struct my_device_data, cdev);
  //file -> private_data = my_data;
  try_module_get(THIS_MODULE);
  return 0;
}

static int beeper_release(struct inode *inode, struct file *file) {
  module_put(THIS_MODULE);
  return 0;
}

static ssize_t beeper_read(struct file *filp, char *buf, size_t length, loff_t *offset ) {
  //beeper_device = container_of(asdf);
  //if(copy_to_user(buf,mem+*offset,length))
    //return -EFAULT;
  *offset += length;
  return length;
}

static ssize_t beeper_write(struct file *filp, const char *buf, size_t length, loff_t *offset ) {
  //if(length + *offset > 1000)
    //length = 1000 - *offset;
  //if(copy_from_user(mem+*offset,buf,length))
    //return -EFAULT;
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
  ret_val = platform_driver_register(&beeper_platform_driver);
  if(ret_val != 0) {
    pr_err("beeper platfrom driver register function returned %d\n", ret_val);
    return ret_val;
  }
  pr_info("beeper platfrom driver registered\n");
  pr_info("beeper_init exit\n");
  return 0;
}

static void beeper_cleanup(void) {
  platform_driver_unregister(&beeper_platform_driver);
  pr_info("beeper platform driver unregistered\n");
}

module_init(beeper_init);
module_exit(beeper_cleanup);

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_VERSION("1.0");

