/*
 * super_ez_kern.c
 * d0now@stealien
*/

/* Include header. */
#include "super_ez_kern.h"

MODULE_LICENSE("GPL");

struct file_operations s_sek_fops = {
    .open    = sek_open,
    .release = sek_release,
    .read    = sek_read,
    .write   = sek_write,
    .unlocked_ioctl = sek_ioctl,
};

/* Initialize module */
static int __init sek_init(void) {

    int minor = 0;
    dev_t dev;

    if (alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME) != 0)
        goto err;

    sekdev_major = MAJOR(dev);
    dev = MKDEV(sekdev_major, MINOR_BASE);

    cdev_init(&sekdev_cdev, &s_sek_fops);
    sekdev_cdev.owner = THIS_MODULE;

    if (cdev_add(&sekdev_cdev, dev, MINOR_NUM) != 0) {
        unregister_chrdev_region(dev, MINOR_NUM);
        goto err;
    }

    sekdev_class = class_create(THIS_MODULE, "sekdev");
    if (IS_ERR(sekdev_class)) {
        cdev_del(&sekdev_cdev);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    device_create(sekdev_class, NULL, MKDEV(sekdev_major, minor), NULL, "sekdev%d", minor);

    printk(KERN_INFO "super ez kernel loaded.\n");
    return 0;

err:
    printk(KERN_INFO "super ez kernel not loaded.\n");
    return 0;
}

/* Cleanup module */
static void __exit sek_cleanup(void) {

    int minor = 0;
    dev_t dev = MKDEV(sekdev_major, MINOR_BASE);

    device_destroy(sekdev_class, MKDEV(sekdev_major, minor));
    class_destroy(sekdev_class);
    cdev_del(&sekdev_cdev);
    unregister_chrdev_region(dev, MINOR_NUM);

    printk(KERN_INFO "super ez kernel removed.\n");
    return;
}
 
static int sek_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "sek_open() called.\n");
    return 0;
}

static int sek_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "sek_release() called.\n");
    return 0;
}

static ssize_t sek_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    printk(KERN_INFO "sek_read() called.\n");
    return 0;
}

static ssize_t sek_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    printk(KERN_INFO "sek_write() called.\n");
    return 0;
}

static long sek_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {

    printk(KERN_INFO "sek_ioctl() called.\n");

    switch (cmd) {

        case SEK_LOGIN:
            if (sek_login((struct ioctl_register __user *)arg) != 0) {
                printk(KERN_INFO "Login failed.\n");
                goto err;
            }
            break;

        case SEK_LOGOUT:
            if (sek_logout((struct ioctl_register __user *)arg) != 0) {
                printk(KERN_INFO "Logout failed.\n");
                goto err;
            }
            break;

        case SEK_REGIST:
            if (sek_regist((struct ioctl_register __user *)arg) != 0) {
                printk(KERN_INFO "Register failed.\n");
                goto err;
            }
            break;

        case SEK_INFO:
            if (sek_info((struct ioctl_register __user *)arg) != 0) {
                printk(KERN_INFO "Getting information failed.\n");
                goto err;
            }
            break;

        default:
            printk(KERN_INFO "No command.\n");
            break;
    }

    return 0;
err:
    return -1;
}

static long sek_login(struct ioctl_register __user *arg) {

    struct ioctl_register login_arg;
    struct ioctl_register regs;
    mm_segment_t old_fs;
    int fd = 0;
    struct file *filp;

    if (copy_from_user(&login_arg, (void __user *)arg, sizeof(login_arg)))
        goto err;

    login_arg.id[sizeof(login_arg.id)-1] = '\0';
    login_arg.pw[sizeof(login_arg.pw)-1] = '\0';

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    if ((fd = sys_open(DB_PATH, O_RDONLY, 0)) < 0)
        goto err;

    filp = fget(fd);
    if (filp == NULL)
        goto err;

    while (vfs_read(fd, &regs, sizeof(regs)) != sizeof(regs)) {

        regs.id[sizeof(regs.id)-1] = '\0';
        regs.pw[sizeof(regs.pw)-1] = '\0';

        if (login_arg.otp != regs.otp)
            continue;

        if (strncmp(login_arg.id, regs.id, sizeof(login_arg.id)))
            continue;

        if (strncmp(login_arg.pw, regs,pw, sizeof(login_arg.pw)))
            continue;

        if (!strncmp(login_arg.id, "root", sizeof(login_arg,id)))
            permission = 2;
        else
            permission = 1;

        memcpy(&now, &login_arg, sizeof(now));

        break;
    }

    if (permission)
        printk(KERN_INFO "Logged in!\n");
    else
        printk(KERN_INFO "Login failed.\n");

done:
    sys_close(fd);
    set_fs(old_fs);
    return 0;

err:
    if (fd)
        close(fd);

    printk("Error occured while loggin in.\n");
    return -1;
}

static long sek_logout(struct ioctl_register __user *arg) {
    if (permission)
        permission = 0;
    memset(&now, 0, sizeof(now));
    printk(KERN_INFO "Logged out!\n");
    return 0;
}

static long sek_regist(struct ioctl_register __user *arg) {
    return 0;
}

static long sek_info(struct ioctl_register __user *arg) {
    return 0;
}

module_init(sek_init);
module_exit(sek_cleanup);