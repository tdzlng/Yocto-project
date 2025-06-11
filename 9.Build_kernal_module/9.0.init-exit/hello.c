#include <linux/module.h>  /* define module_init end module_exit */
#include <linux/fs.h>      /* define allocate major & minor number */

#define DRIVER_AUTHOR "LongTD   longtd.hust@gmail.com"
#define DRIVER_DESC   "Hello world kernel module"
#define DRIVER_VERS   "1.0"

/* Constructor */
static int  __init hello_world_init(void)
{
    printk(KERN_INFO "Hello world kernel module\n");
    return 0;
}

/* Destructor */
static void  __exit hello_world_exit(void)
{
    printk(KERN_INFO "Goodbye\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);  
MODULE_VERSION(DRIVER_VERS);