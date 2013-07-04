#include <linux/module.h>
#include <linux/slab.h>
#include "../include/asm/uaccess.h"
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>

static ssize_t ledstrip_write(struct file *, const char *, size_t, loff_t *);

#define DEVICE_NAME "ledstrip"
#define LEDSTRIP_CLOCK 15
#define LEDSTRIP_DATA 7

static unsigned int major;
static dev_t char_d_mm;
static struct cdev char_d_cdev;
static struct class* cl;
static struct device* de;

/* Functions for character device */
struct file_operations ledstrip_fops = {
	.read = NULL,
	.write = ledstrip_write,
	.open = NULL,
	.release = NULL
};


int init_module()
{
	int result;

	printk("Loading the LED Strip LKM...\n");

	// Allocate a major number for this device
	result = alloc_chrdev_region(&char_d_mm, 0, 1, DEVICE_NAME);
	if (result < 0) {
		printk(KERN_ALERT "Could not allocate a major number %d\n", result);
		return result;
	}
	major = MAJOR(char_d_mm);

	// Register the character device
	cdev_init(&char_d_cdev, &ledstrip_fops);
	char_d_cdev.owner = THIS_MODULE;
	result = cdev_add(&char_d_cdev, char_d_mm, 1);
	if (result < 0) {
		printk(KERN_ALERT "Unable to register char dev %d\n", result);
		return result;
	}
	printk(KERN_INFO "Char interface registered on %d\n", major);

	cl = class_create(THIS_MODULE, DEVICE_NAME);
	if (cl == NULL) {
		printk(KERN_ALERT "Could not create device class\n");
		return 1;
	}

	// Create the device in /dev/ledstrip
	de = device_create(cl, NULL, char_d_mm, NULL, DEVICE_NAME);
	if (de == NULL) {
		printk(KERN_ALERT "Could not create device\n");
		return 1;
	}

	// Initialize GPIO
	result = gpio_request(LEDSTRIP_CLOCK, DEVICE_NAME);
	if (result) {
		printk(KERN_ALERT "gpio_request for GPIO %d failed with %d\n",
				LEDSTRIP_CLOCK, result);
	}

	result = gpio_request(LEDSTRIP_DATA, DEVICE_NAME);
	if (result) {
		printk(KERN_ALERT "gpio_request for GPIO %d failed with %d\n",
				LEDSTRIP_DATA, result);
	}

	gpio_direction_output(LEDSTRIP_CLOCK, 0);
	gpio_direction_output(LEDSTRIP_DATA, 0);

	return 0;
}


void cleanup_module()
{
	gpio_free(LEDSTRIP_CLOCK);
	gpio_free(LEDSTRIP_DATA);

	cdev_del(&char_d_cdev);
	unregister_chrdev(char_d_mm, DEVICE_NAME);
	device_destroy(cl, char_d_mm);
	class_destroy(cl);

	printk("Unloaded led strip kernel module\n");
}

static ssize_t
ledstrip_write(struct file *filp, const char *in_buf, size_t len, loff_t * off)
{
	const unsigned char *buf = in_buf;

	unsigned char cur_buf[3];
	int out_len = 0;

	uint32_t color;
	int color_bit;

	unsigned long result;

	while (len >= 3) {

		result = copy_from_user(cur_buf, buf, 3);
		if (result != 0) {
			// Could not copy from userspace
			continue;
		}

		//unsigned int gpio_pin = XMAS_OUT_0;
		//if (cur_buf[0] & 0x40) {
		//	gpio_pin = XMAS_OUT_1;
		//}

		color = ((cur_buf[0] & 0xFF) << 16) |
		        ((cur_buf[1] & 0xFF) << 8) |
		        (cur_buf[2] & 0xFF);

		local_irq_disable();

		// Toggle the colors down the pin
		for (color_bit=0; color_bit<24; color_bit++) {
			gpio_set_value(LEDSTRIP_CLOCK, 0); //Only change data when clock is low

			if (color & (1 << color_bit)) {
				gpio_set_value(LEDSTRIP_DATA, 1);
			} else {
				gpio_set_value(LEDSTRIP_DATA, 0);
			}

			gpio_set_value(LEDSTRIP_CLOCK, 1);
		}

		local_irq_enable();

		buf += 3;
		len -= 3;
		out_len += 3;
	}

	// Pull clock low for 500us to put strip into reset/post mode
	gpio_set_value(LEDSTRIP_CLOCK, 0);
	udelay(500);

	// This might result in some weird (read: broken) behavior
	// if/when data is fragmented across multiple write calls
	// (page boundary?)
	if (len != 0) {
		return -EINVAL;
	}
	return out_len;
}


MODULE_AUTHOR("Brad Campbell");
MODULE_DESCRIPTION("Kernel module to drive the Sparkfun LED Strip.");
MODULE_LICENSE("GPL");
