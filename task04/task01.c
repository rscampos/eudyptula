#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

/*
 * Entry point of the lkm.
 */
int init_module(void)
{
	pr_devel("Hello World!\n");
	return 0;
}
/*
 * Exit point of the lkm.
 */
void cleanup_module(void)
{
}
