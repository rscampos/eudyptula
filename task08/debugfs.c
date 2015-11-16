#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>

#define BUF_LEN 13
static const char dir_name[] = "eudyptula";
static const char file_name_id[] = "id";
static const char euid[] = "4f92b3aee379";
static const char file_name_jiffies[] = "jiffies";
static struct dentry *debugfs_dir;

/* Handles for the file 'id' */
ssize_t device_read_id(struct file *filp,
		       char *bufStoreData, size_t bufCount, loff_t *curOffset)
{
	return simple_read_from_buffer(bufStoreData, bufCount, curOffset,
				       euid, strlen(euid));
}

ssize_t device_write_id(struct file *filp,
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

/* Handle for the file 'jiffies' */
ssize_t device_read_jiffies(struct file *filp,
			    char *bufStoreData, size_t bufCount,
			    loff_t *curOffset)
{
	int ret;
	unsigned long temp = jiffies;

	return simple_read_from_buffer(bufStoreData, bufCount, curOffset,
				       &temp, sizeof(temp));
}

const struct file_operations fops_id = {
	.write	= device_write_id,
	.read	= device_read_id
};

const struct file_operations fops_jiffies = {
	.read	= device_read_jiffies
};

static int driver_entry(void)
{
	struct dentry *debugfs_file;
	umode_t mode_id = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
	umode_t mode_jiffies = S_IRUSR|S_IRGRP|S_IROTH;
	unsigned int data;

	debugfs_dir = debugfs_create_dir(dir_name, NULL);

	if (!debugfs_dir)
		return ERR_PTR(-ENOMEM);

	/* id file */
	debugfs_file = debugfs_create_file(file_name_id, mode_id,
					   debugfs_dir, &data, &fops_id);
	if (!debugfs_file)
		return ERR_PTR(-ENOMEM);

	/* jiffies file */
	debugfs_file = debugfs_create_file(file_name_jiffies, mode_jiffies,
					   debugfs_dir, &data, &fops_jiffies);
	if (!debugfs_file)
		return ERR_PTR(-ENOMEM);

	return 0;
}

static void driver_exit(void)
{
	debugfs_remove_recursive(debugfs_dir);
}

module_init(driver_entry);
module_exit(driver_exit);
MODULE_LICENSE("GPL");
