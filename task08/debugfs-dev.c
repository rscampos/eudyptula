#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/slab.h>     /* kernel allocation        */
#include <linux/gfp.h>      /* kernel allocation flags  */
#include <linux/rwsem.h> /* read_lock/unlock         */
#include <linux/delay.h>

#define BUF_LEN 13
static const char dir_name[]		= "eudyptula";
static const char file_name_id[]	= "id";
static const char file_name_foo[]	= "foo";
static const char file_name_jiffies[]	= "jiffies";
static const char euid[]		= "4f92b3aee379";
static struct dentry *debugfs_dir;
static unsigned char *block;
struct rw_semaphore rw_sem;

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
                    char *bufStoreData, size_t bufCount, loff_t *curOffset)
{
    int ret;
    unsigned long temp = jiffies;

    pr_info("----------------------------------------\n");
    pr_info("[Jiffies :0x%x %d]\n", temp, sizeof(temp));

    /* Buffer before */
    pr_info("[Buffer user (before): &0x%x[0x%x]]\n", &bufStoreData, bufStoreData);
    pr_info("    &0x%x[0x%x%x%x%x]\n", bufStoreData, *(bufStoreData+3),
                    *(bufStoreData+2),*(bufStoreData+1), *(bufStoreData));

    pr_info("[Writing: (0x%x) [size:%d] to 0x%x\n", temp, sizeof(temp), bufStoreData);

    pr_info("[Simple_read_from_buffer(0x%x, %d, 0x%x, 0x%x, %d)]\n", bufStoreData,
                    bufCount, curOffset, &temp, sizeof(temp));

    ret = simple_read_from_buffer(bufStoreData, bufCount, curOffset, &temp, sizeof(temp));

    /* Buffer after */
    pr_info("[Buffer user (after): &0x%x[0x%x]]\n", &bufStoreData, bufStoreData);
    pr_info("    &0x%x[0x%x%x%x%x]\n", bufStoreData, *(bufStoreData+3),
                    *(bufStoreData+2),*(bufStoreData+1), *(bufStoreData));

    pr_info("----------------------------------------\n");

    return ret;
}

/* Handles for the file 'foo' */
ssize_t device_read_foo(struct file *filp,
                    char *bufStoreData, size_t bufCount, loff_t *curOffset)
{
        int ret;
        int i;

        pr_info("[READ][BEFORE  ][Alloc:&0x%x[0x%x]=0x%x]\n", &block, block, *block);
        pr_info("[READ][BEFORE  ][LOCK: &0x%x[0x%x]]\n", &rw_sem, rw_sem);

        down_read(&rw_sem);
        msleep(20000);
        pr_info("      [READ][INSIDE  ][LOCK: &0x%x[0x%x]]\n", &rw_sem, rw_sem);
        ret = simple_read_from_buffer(bufStoreData, bufCount, curOffset, block, PAGE_SIZE);
        up_read(&rw_sem);

        pr_info("[READ][AFTER   ][LOCK: &0x%x[0x%x]]\n", &rw_sem, rw_sem);
        pr_info("[READ][AFTER   ]:");

        for(i=0; i < 10; i++)
            pr_info("       block - [0x%x -  0x%x] - bufStoreData\n",block[i],bufStoreData[i]);

        return ret;
}

ssize_t device_write_foo(struct file *filp,
                     const char *bufSourceData, size_t bufCount,
                     loff_t *curOffset)
{
        int ret;
        int i;

        if (bufCount > PAGE_SIZE - 1)
                return -EINVAL;

        if(!block){
            block = kmalloc(PAGE_SIZE, GFP_KERNEL);

            if (!block)
                return -ENOMEM;
        }

        pr_info("[WRITE][BEFORE][Alloc: &0x%x[0x%x]=0x%x]\n", &block, block, *block);
        pr_info("[WRITE][BEFORE][LOCK:  &0x%x[0x%x]]\n", &rw_sem, rw_sem);

        down_write(&rw_sem);
        msleep(20000);
        pr_info("      [WRITE][INSIDE  ][LOCK: &0x%x[0x%x]]\n", &rw_sem, rw_sem);
        ret = simple_write_to_buffer(block, bufCount, curOffset, bufSourceData,
                                     PAGE_SIZE);
        up_write(&rw_sem);

        block[bufCount] = '\0';

        pr_info("[WRITE][AFTER][LOCK:   &0x%x[0x%x]]\n", &rw_sem, rw_sem);
        pr_info("[WRITE][AFTER][Alloc:  &0x%x[0x%x]=0x%x]\n", &block, block, *block);
        pr_info("[WRITE][STRIN][Alloc:  [%s]\n", block);

        return ret;
}


const struct file_operations fops_id = {
        .write          = device_write_id,
        .read           = device_read_id
};

const struct file_operations fops_jiffies = {
        .read           = device_read_jiffies
};

const struct file_operations fops_foo = {
        .write          = device_write_foo,
        .read           = device_read_foo
};

static int driver_entry(void)
{
        struct dentry *debugfs_file;
        umode_t mode_id = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
        umode_t mode_jiffies = S_IRUSR|S_IRGRP|S_IROTH;
        umode_t mode_foo = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
        unsigned int data;

        init_rwsem(&rw_sem);

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

        /* foo file */
        debugfs_file = debugfs_create_file(file_name_foo, mode_foo,
                                           debugfs_dir, &data, &fops_foo);
        if (!debugfs_file)
                return ERR_PTR(-ENOMEM);


        pr_info("driver_entry!\n");
        return 0;
}

static int driver_exit(void)
{
        debugfs_remove_recursive(debugfs_dir);

        if(!block)
            kfree(block);

        pr_info("driver_exit!\n");

        return 0;
}

module_init(driver_entry);
module_exit(driver_exit);
MODULE_LICENSE("GPL");
