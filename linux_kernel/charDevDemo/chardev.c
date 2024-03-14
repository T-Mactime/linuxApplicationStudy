#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

// 字符驱动名称
#define DEV_NAME "embedchardev"
// 设备数量？？？
// TODO 这里需要后面理解一下
#define DEV_CNT (1)
// 字符设备缓冲区大小？？？
#define BUFF_SIZE 128

// 定义字符设备的设备号，（设备ID）？？
struct dev_t devno;
// 定义字符设备结构体
static struct cdev chr_dev;

// 数据缓冲区
static char vbuf[BUFF_SIZE];
static int chr_dev_open(struct inode *inode, struct file *filp);
static int chr_dev_release(struct inode *inode, struct file *filp);
static ssize_t chr_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos);
static ssize_t chr_dev_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos);
static struct file_operations chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = chr_dev_open,
    .write = chr_dev_write,
    .release = chr_dev_release,
    .read = chr_dev_read,
};

static int chr_dev_open(struct inode *inode, struct file *filp)
{
    printk("open\n");
    return 0;
};

static int chr_dev_release(struct inode *inode, struct file *filp)
{
    printk("release\n");
    return 0;
}

static ssize_t chr_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    unsigned long p = *ppos;
    int ret;
    int tmp = count;
    if (p > BUFF_SIZE)
    {
        return 0;
    }
    if (tmp > BUFF_SIZE - p)
    {
        tmp = BUFF_SIZE - p;
    }
    ret = copy_from_user(vbuf, buf, tmp);
    *ppos += tmp;

    return tmp;
}

static ssize_t chr_dev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    unsigned long p = *ppos;
    int ret;
    int tmp = count;
    static int i = 0;
    i++;
    if (p >= BUFF_SIZE)
    {
        return 0;
    }

    if (tmp > BUFF_SIZE - p)
    {
        tmp = BUFF_SIZE - p;
    }

    ret = copy_to_user(buf, vbuf + p, tmp);
    *ppos += tmp;
    return tmp;
}

static int __init chrdev_init(void)
{
    int ret = 0;
    printk("chrdev init \n");

    ret = alloc_chrdev_region(&devno, 0, DEV_CNT, DEV_NAME);
    if (ret < 0)
    {
        printk("failed to alloc devno \n");
        goto alloc_err;
    }

    cdev_init(&chr_dev, &chr_dev_fops);

    ret = cdev_add(&chr_dev, devno, DEV_CNT);
    if (ret < 0)
    {
        printk("fail to add cdev\n");
        goto add_err;
    }
    return 0;

add_err:
    unregister_chrdev_region(devno, DEV_CNT);

alloc_err:
    return ret;
}

moudle_init(chrdev_init);

static void __exit chrdev_exit(void){
    printk("chrdev_exit \n");

    unregister_chrdev_region(devno, DEV_CNT);

    chrdev_del(&chr_dev);
}

moudle_exit(chrdev_exit);

MODULE_LICENSE("GPL");