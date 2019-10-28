/*
 * super_ez_kern.h
 * d0now@stealien
*/

#ifndef _SUPER_EZ_KERN_H_
#define _SUPER_EZ_KERN_H_

/* Includes */
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
#include <linux/ioctl.h>
#include <linux/string.h>
#include <linux/fcntl.h>

/* driver name */
#define DRIVER_NAME "chardev"

/*
 * IOCTL Commands
*/

struct ioctl_register {
    char id[32];
    char pw[32];
    uint64_t otp;
};

#define IOCTL_MAGIC '*'
#define SEK_LOGIN  _IOW(IOCTL_MAGIC, 1, struct ioctl_register)
#define SEK_LOGOUT _IOW(IOCTL_MAGIC, 2, struct ioctl_register)
#define SEK_REGIST _IOW(IOCTL_MAGIC, 3, struct ioctl_register)
#define SEK_INFO   _IOW(IOCTL_MAGIC, 4, struct ioctl_register)  

/*
 * DB Settings
*/

#define DB_PATH "/tmp/database.txt"
int permission;
struct ioctl_register now;

/* global variables */
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM  = 1;
static unsigned int sekdev_major;
static struct cdev sekdev_cdev;
static struct class *sekdev_class = NULL;

/* Function pre-defines */
static int sek_open(struct inode *, struct file *);
static int sek_release(struct inode *, struct file *);
static ssize_t sek_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t sek_write(struct file *, const char *, size_t, loff_t *);
static long sek_ioctl(struct file *, unsigned int, unsigned long);
static long sek_login(struct ioctl_register __user *arg);
static long sek_logout(struct ioctl_register __user *arg);
static long sek_regist(struct ioctl_register __user *arg);
static long sek_info(struct ioctl_register __user *arg);

#endif