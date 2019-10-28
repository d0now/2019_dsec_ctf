/*
 * super_ez_kern.c
 * d0now@stealien
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/current.h>
#include <linux/uaccess.h>

/* Include header. */
#include "super_ez_kern.h"

/* Initialize module */
static int __init sek_init(void) {
    
    return 0;
}

/* Cleanup module */
static void __exit sek_cleanup(void) {
    
}
 
module_init(sek_init);
module_exit(sek_cleanup);