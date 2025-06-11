#include <linux/module.h>  /* define module_init end module_exit */
#include <linux/fs.h>      /* define allocate major & minor number */
#include <linux/device.h>  /* define class_create/device_create */

#define DRIVER_AUTHOR "LongTD   longtd.hust@gmail.com"
#define DRIVER_DESC   "Hello world kernel module"
#define DRIVER_VERS   "1.0"

struct m_foo_dev {
    dev_t dev_num;
    struct class *m_class;
} mdev;

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
        goto rm_class;
    }

    /* 03. Creating device*/
    if ((device_create(mdev.m_class, NULL, mdev.dev_num, NULL, "m_device")) == NULL) {
        pr_err("Cannot create my device\n");
        goto rm_device;
    }

    pr_info("This is device-file module\n");
    return 0;

rm_class:
    class_destroy(mdev.m_class);
rm_device:
    unregister_chrdev_region(mdev.dev_num, 1);
    return -1;
}

/* Destructor */
static void  __exit mod_exit(void)
{
    device_destroy(mdev.m_class, mdev.dev_num);     /* 03 */
    class_destroy(mdev.m_class);                    /* 02 */
    unregister_chrdev_region(mdev.dev_num, 1);      /* 01 */

    printk(KERN_INFO "Goodbye\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);  
MODULE_VERSION(DRIVER_VERS);