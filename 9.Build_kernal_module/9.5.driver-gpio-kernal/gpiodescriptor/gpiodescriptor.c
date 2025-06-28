#include <linux/module.h>  /* define module_init end module_exit */
#include <linux/fs.h>      /* define allocate major & minor number */
#include <linux/device.h>  /* define class_create/device_create */
#include <linux/cdev.h>    /* define cdev_init/cdev_add */
#include <linux/slab.h>    /* define kmaloc, kfree */
#include <linux/uaccess.h> /* define copy_from_user, copy_to_user */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>      /* For platform devices */
#include <linux/gpio/consumer.h>        /* For GPIO Descriptor */
#include <linux/of.h>                   /* For DT*/


#define LOW 0
#define HIGH 1

#define NPAGES  1

static struct gpio_desc *gpio_27;

static const struct of_device_id gpiod_dt_ids[] = {
    { .compatible = "gpio-descriptor-sample", },
    { /* sentinel */ }
};

struct m_foo_dev {
    int size;
    char *kmalloc_ptr;
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
} mdev;

/* Function Prototypes */
static int      __init mod_init(void);
static void     __exit mod_exit(void);
static int      m_open(struct inode *inode, struct file *file);
static int      m_release(struct inode *inode, struct file *file);
static ssize_t  m_read(struct file *filp, char __user *user_buf, size_t size,loff_t * offset);
static ssize_t  m_write(struct file *filp, const char *user_buf, size_t size, loff_t * offset);
static void led_jdg(void);

/* Update pointer function */
static struct file_operations fops =
{
    .owner      = THIS_MODULE,
    .read       = m_read,
    .write      = m_write,
    .open       = m_open,
    .release    = m_release,
};

/* This function will be called when we open the Device file */
static int m_open(struct inode *inode, struct file *file)
{
    pr_info("System call open() called...!!!\n");
    return 0;
}

/* This function will be called when we close the Device file */
static int m_release(struct inode *inode, struct file *file)
{
    pr_info("System call close() called...!!!\n");
    return 0;
}

/* This function will be called when we read the Device file */
static ssize_t m_read(struct file *file, char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_read;

    pr_info("System call read() called...!!!\n");

    /* Check size doesn't exceed our mapped area size */
    to_read = (size > mdev.size - *offset) ? (mdev.size - *offset) : size;

	/* Copy from mapped area to user buffer */
	if (copy_to_user(user_buf, mdev.kmalloc_ptr + *offset, to_read))
		return -EFAULT;

    *offset += to_read;

	return to_read;
}

/* This function will be called when we write the Device file */
static ssize_t m_write(struct file *file, const char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_write;

    pr_info("System call write() called...!!!\n");

    /* Check size doesn't exceed our mapped area size */
	to_write = (size + *offset > NPAGES * PAGE_SIZE) ? (NPAGES * PAGE_SIZE - *offset) : size;

	/* Copy from user buffer to mapped area */
	memset(mdev.kmalloc_ptr, 0, NPAGES * PAGE_SIZE);
	if (copy_from_user(mdev.kmalloc_ptr + *offset, user_buf, to_write) != 0)
		return -EFAULT;

    mdev.kmalloc_ptr[to_write-1] = 0;
    pr_info("Data from usr: %s", mdev.kmalloc_ptr);

    led_jdg();

    *offset += to_write;
    mdev.size = *offset;

	return to_write;
}

/* Process judgment led status */
static void led_jdg(void){
    if(strcmp(mdev.kmalloc_ptr, "on") == 0) {
        pr_info("LED is ON");
    // Set GPIO 27 high
    gpiod_set_value(gpio_27, HIGH);
    } else if (strcmp(mdev.kmalloc_ptr, "off") == 0) {
        pr_info("LED is OFF");
    // Set GPIO 27 low
    gpiod_set_value(gpio_27, LOW);
    } else {
        pr_info("invalid command");
    }

    
}

/* Constructor */
static int  __init mod_init(void)
{
    /* 01. Allocating device number (cat /pro/devices)*/
    if (alloc_chrdev_region(&mdev.dev_num, 0, 1, "m-cdev") < 0) {
	    pr_err("Failed to alloc chrdev region\n");
	    return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));

    /* 02. Creating struct class */
    mdev.m_class = class_create("m_class");
    if ((mdev.m_class) == NULL) {
        pr_err("Cannot create the struct class for my device\n");
        goto rm_device_num;
    }

    /* 03 Creating device*/
    if ((device_create(mdev.m_class, NULL, mdev.dev_num, NULL, "m_device")) == NULL) {
        pr_err("Cannot create my device\n");
        goto rm_class;
    }

    /* 04.1 Creating cdev structure */
    cdev_init(&mdev.m_cdev, &fops);

    /* 04.2 Adding character device to the system */
    if ((cdev_add(&mdev.m_cdev, mdev.dev_num, 1)) < 0) {
        pr_err("Cannot add the device to the system\n");
        goto rm_device;
    }

    /* 05. Allocate kernal buffer */
    if((mdev.kmalloc_ptr = kmalloc(1024 , GFP_KERNEL)) == 0){
        pr_err("Cannot allocate memory in kernel\n");
        goto rm_device;
    }


    pr_info("gpio legacy kernel module\n");
    return 0;

rm_device:
    device_destroy(mdev.m_class, mdev.dev_num);
rm_class:
    class_destroy(mdev.m_class);
rm_device_num:
    unregister_chrdev_region(mdev.dev_num, 1);
    return -1;
}

/* Destructor */
static void  __exit mod_exit(void)
{
    kfree(mdev.kmalloc_ptr);                        /* 05 */
    cdev_del(&mdev.m_cdev);                         /* 04 */
    device_destroy(mdev.m_class, mdev.dev_num);     /* 03 */
    class_destroy(mdev.m_class);                    /* 02 */
    unregister_chrdev_region(mdev.dev_num, 1);      /* 01 */



    printk(KERN_INFO "Exit module\n");
}

static int gpio_descriptor_probe (struct platform_device *pdev)
{
    int retval;
    struct device *dev = &pdev->dev;

    // contructor charactor device
    mod_init();

    // Get the GPIO for gpio27 as output high
    gpio_27 = gpiod_get(dev, "mygpio27", GPIOD_OUT_HIGH);
    pr_info("gpiod_get.\n");
    if (IS_ERR(gpio_27)) {
        retval = PTR_ERR(gpio_27);
        pr_err("Failed to get my-gpio27-output GPIO: %d\n", retval);
        return retval;
    }
    pr_info("my_output_gpio (GPIO 27) initialized as output HIGH.\n");
    gpiod_set_value(gpio_27, HIGH);

    pr_info("Hello! device gpio27 descriptor probed!\n");
    return 0;
}

static void gpio_descriptor_remove(struct platform_device *pdev)
{


    // destroy gpio27
    gpiod_set_value(gpio_27, LOW);
    pr_info("gpio27 is off!\n");

    gpiod_put(gpio_27); 
    pr_info("exit gpio27 descriptor!\n");

    // destructor charactor device
    mod_exit();
}

static struct platform_driver mypdrv = {
    .probe      = gpio_descriptor_probe,
    .remove     = gpio_descriptor_remove,
    .driver     = {
        .name     = "gpio-descriptor-sample",
        .of_match_table = of_match_ptr(gpiod_dt_ids),
        .owner    = THIS_MODULE,
    },
};

module_platform_driver(mypdrv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LongTD");
MODULE_DESCRIPTION("Descriptor-Based GPIO Driver for Raspberry Pi Zero 2w");  
MODULE_VERSION("1.0");
