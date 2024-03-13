#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include "cala.h"

static int __init calculation_init(void){
    printk(KERN_EMERG "calculation inti\n");
    printk(KERN_EMERG "itype + 1 %d type -1 %d\n", my_add(itype, 1), my_sub(itype, 1));
    return 0;
}

static void __exit calculation_exit(void){
    printk(KERN_EMERG "calculation exit\n");
}

module_init(calculation_init);
module_exit(calculation_exit);

MODULE_LICENSE("GPL2");
MODULE_AUTHOR("SCHEMBEDSTUDY");
MODULE_DESCRIPTION("calculation module");
MODULE_ALIAS("calculation module");