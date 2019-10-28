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

    global_number = 0;
    request_head = NULL;

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

    struct sek_request *req;
    struct sek_request *last;

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

        case SEK_FLUSH:
            return (long)sek_req_cleanup();

        default:
            printk(KERN_INFO "Invalid command.\n");
            break;
    }

    if ((void __user *)arg == NULL)
        goto done;

    req = kzalloc(sizeof(struct sek_request), GFP_KERNEL);
    if (req == NULL) {
        printk(KERN_INFO "kzalloc returns NULL.\n");
        goto err;
    }
    printk(KERN_INFO "[kheap] Request allocated. %pK\n", req);

    req->number  = global_number;
    req->fn      = sek_req_free;
    req->self    = req;
    req->next    = NULL;
    if (copy_from_user(&req->reg, (struct ioctl_register __user *)arg, sizeof(struct ioctl_register)))
        goto err;

    if (request_head) {
        last = sek_req_get_last(request_head);
        if (last)
            last->next = req;
    } else {
        request_head = req;
    }
    sek_req_count++;

done:
    return 0;
err:
    return -1;
}

static long sek_login(struct ioctl_register __user *arg) {

    struct ioctl_register login_arg;
    struct ioctl_register regs;
    mm_segment_t old_fs;
    loff_t pos = 0;
    struct file *filp = NULL;

    if (arg == NULL)
        goto err;

    if (copy_from_user(&login_arg, (void __user *)arg, sizeof(login_arg)))
        goto err;

    login_arg.id[sizeof(login_arg.id)-1] = '\0';
    login_arg.pw[sizeof(login_arg.pw)-1] = '\0';

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    filp = filp_open(DB_PATH, O_RDONLY, 0);
    if (IS_ERR(filp) || filp == NULL)
        goto err;

    printk(KERN_INFO "flip_open() = %pK\n", filp);

    while (kernel_read(filp, (void __user *)&regs, sizeof(regs), &pos) == sizeof(regs)) {

        regs.id[sizeof(regs.id)-1] = '\0';
        regs.pw[sizeof(regs.pw)-1] = '\0';

        if (login_arg.otp != regs.otp)
            continue;

        if (strncmp(login_arg.id, regs.id, sizeof(login_arg.id)))
            continue;

        if (strncmp(login_arg.pw, regs.pw, sizeof(login_arg.pw)))
            continue;

        if (!strncmp(login_arg.id, "admin", sizeof(login_arg.id)))
            permission = 2;
        else
            permission = 1;

        memcpy(&now, &login_arg, sizeof(now));

        break;
    }

    printk(KERN_INFO "kernel_read()\n");

    if (permission)
        printk(KERN_INFO "Logged in!\n");
    else
        printk(KERN_INFO "Login failed.\n");

    filp_close(filp, NULL);
    set_fs(old_fs);
    return 0;

err:
    if (!IS_ERR(filp) && filp)
        filp_close(filp, NULL);
    return -1;
}

static long sek_logout(struct ioctl_register __user *arg) {
    if (permission)
        permission = 0;
    memset(&now, 0, sizeof(now));
    printk(KERN_INFO "Logged out.\n");
    return 0;
}

static long sek_regist(struct ioctl_register __user *arg) {

    loff_t pos = 0;
    mm_segment_t old_fs;
    struct file *filp = NULL;
    struct ioctl_register register_arg;

    if (arg == NULL)
        goto err;

    if (copy_from_user(&register_arg, (void __user *)arg, sizeof(register_arg)))
        goto err;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    filp = filp_open(DB_PATH, O_WRONLY|O_CREAT, 0644);
    if (filp == NULL)
        goto err;

    if ((pos = vfs_llseek(filp, 0, 2)) == -1)
        goto err;

    if (kernel_write(filp, (void __user *)&register_arg, sizeof(register_arg), &pos) != sizeof(register_arg)) {
        goto err;
    }

    filp_close(filp, NULL);
    set_fs(old_fs);
    printk(KERN_INFO "Registered.\n");
    return 0;

err:
    if (filp)
        filp_close(filp, NULL);
    return -1;
}

static long sek_info(struct ioctl_register __user *arg) {

    if (arg == NULL)
        goto err;

    if (permission) {
        if (copy_to_user((void __user *)arg, &now, sizeof(now)))
            goto err;
        if (permission == 2)
            printk(KERN_INFO "Permission %lx=%x\n", (long unsigned int)&permission, permission);
        printk(KERN_INFO "Information passed.\n");
    } else {
        printk(KERN_INFO "Login first.\n");
    }

    return 0;
err:
    return -1;
}

static struct sek_request *sek_req_get_last(struct sek_request *req) {

    struct sek_request *now = req;

    if (req == NULL)
        goto err;

    while (now->next != NULL)
        now = now->next;

    return now;
err:
    return NULL;
}

static uint64_t sek_req_cleanup(void) {

    int i=0;
    uint64_t err = -1;
    struct sek_request *next=NULL, *now=NULL;

    if (request_head == NULL)
        goto err;

    now = request_head;
    for (i=0 ; i<=sek_req_count ; i++) {
        if (now == NULL)
            goto err;
        next = now->next;
        err = now->fn(now->self);
        if (err)
            goto err;
        now = next;
    }
    sek_req_count = 0;

    request_head = NULL;
    return 0;
err:
    sek_req_count = 0;
    request_head = NULL;
    return err;
}

static uint64_t sek_req_free(struct sek_request *req) {
    printk(KERN_INFO "[kheap] Trying to free %pK\n", req);
    if (req) {
        kfree(req);
        return 0;
    } else {
        return -1;
    }
}

module_init(sek_init);
module_exit(sek_cleanup);