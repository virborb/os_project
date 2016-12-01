#include <linux/module.h>  /* Needed by all kernel modules */
#include <linux/kernel.h>  /* Needed for loglevels (KERN_WARNING, KERN_EMERG, KERN_INFO, etc.) */
#include <linux/init.h>    /* Needed for __init and __exit macros. */

static int __init onload(void) {
  printk(KERN_EMERG "Loadable module initialized\n");
  return 0;
}

static void __exit onunload(void) {
  printk(KERN_EMERG "Loadable module removed\n");
}
module_init(onload);
module_exit(onunload);