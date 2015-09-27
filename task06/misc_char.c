#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/init.h>

#define BUF_LEN 13
static const char device_name[]       = "eudyptula";
static const char euid[]              = "4f92b3aee379";

ssize_t device_read(struct file *filp,
		    char *bufStoreData, size_t bufCount, loff_t *curOffset)
{
	return simple_read_from_buffer(bufStoreData, bufCount, curOffset,
				       euid, strlen(euid));
}

ssize_t device_write(struct file *filp,
		     const char *bufSourceData, size_t bufCount,
		     loff_t *curOffset)
{
	int ret;
	char data[BUF_LEN];

	if (bufCount > strlen(euid))
		return -EINVAL;

	ret = simple_write_to_buffer(data, bufCount, curOffset, bufSourceData,
				     BUF_LEN);

	data[BUF_LEN-1] = '\0';

	if (!strcmp(data, euid) == 0)
		return -EINVAL;

	return ret;
}

const struct file_operations fops = {
	.write		= device_write,
	.read		= device_read
};

struct miscdevice miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= device_name,
	.fops		= &fops
};

static int driver_entry(void)
{
	return misc_register(&miscdev);
}

static int driver_exit(void)
{
	return misc_deregister(&miscdev);
}

module_init(driver_entry);
module_exit(driver_exit);
MODULE_LICENSE("GPL");
