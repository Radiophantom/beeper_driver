#define AUTHOR      "KriserX"
#define LICENSE     "GPL"
#define DESC        "Malloc memory and write/read its content"
#define DEVICES     "Simple memory driver"

#define SUCCESS     0
#define DEVICE_NAME "smem"
#define MAX_SIZE    256

static int     device_open    (struct inode *, struct file *);
static int     device_release (struct inode *, struct file *);

static ssize_t device_read    (struct file *, char *,       size_t, loff_t *);
static ssize_t device_write   (struct file *, const char *, size_t, loff_t *);

