/* This lkm is modified from:
 * http://www.tldp.org/LDP/lkmpg/2.6/html/x121.html
 */
 
/* linux/module.h - for the module
 * linux/kernel.h - macros for printk
 */
#include <linux/module.h>
#include <linux/kernel.h>

/* Entry point of the lkm.
 * This is executed as soon as the lkm is loaded.
 * Priority of printk - from linux/kernel.h
 * http://web.cecs.pdx.edu/~jrb/ui/linux/driver4.txt
 */
int init_module(void)
{
	printk(KERN_DEBUG "Hello World!\n");
}
/* Exit point of the lkm.
 * This is executed as soon as the lkm is unloaded and undo
 * everthing did in init_module.
 */
void cleanup_module(void)
{}
