#define AUTHOR      "KriserX"
#define LICENSE     "GPL"
#define DESCRIPTION "Bercut-ET 'beeper' feature control driver"

#define DEVICE_NAME "beeper"

// User-defined structures
struct my_device_data {
  struct cdev cdev;
  //char   my_device_name[30];
  //char   *my_ptr;
};

struct my_device_platform_data {
  int cr_base_addr;
  int cr_cnt;
  int sr_base_addr;
  int sr_cnt;
};

// Char device file operation function prototype
static int     beeper_open   (struct inode *inode, struct file *file);
static int     beeper_release(struct inode *inode, struct file *file);

static ssize_t beeper_read   (struct file *file, char *buf,       size_t len, loff_t *offset);
static ssize_t beeper_write  (struct file *file, const char *buf, size_t len, loff_t *offset);
static loff_t  beeper_llseek (struct file *file, loff_t offset,   int whence                );
static int     beeper_mmap   (struct file *file, struct vm_area_struct *vma                 );

// Platform device driver function prototype
static int platform_probe ( struct platform_device *pdev );
static int platform_remove( struct platform_device *pdev );

// UDEV event function prototype
static int  mychardev_uevent(struct device *dev, struct kobj_uevent_env *env);

// 'Sysfs' attributes' file operations prototype
static ssize_t  show_value (struct device *dev, struct device_attribute *attr,       char *buf              );
static ssize_t  set_value  (struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static ssize_t  reset_value(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
