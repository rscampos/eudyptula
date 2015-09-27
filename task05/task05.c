/* This code was modified from:
 * http://stackoverflow.com/questions/23307579/loading-module\
 * -on-keyboard-hotplug
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static struct usb_device_id usb_table[] = {
	{USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
	USB_INTERFACE_SUBCLASS_BOOT,
	USB_INTERFACE_PROTOCOL_KEYBOARD)},
	{} /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, usb_table);

static int __init usb_init(void)
{
	pr_debug("Hello World!\n");
}

static void __exit usb_exit(void)
{
	pr_debug("Bye World!\n");
}

module_init(usb_init);
module_exit(usb_exit);

MODULE_LICENSE("GPL");



