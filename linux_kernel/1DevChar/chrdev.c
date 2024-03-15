#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEV_NAME "EmbedChardev"
#define DEV_CNT (2)
#define BUFF_SIZE 128

// 定义字符设备的设备号
static dev_t devno;

// 定义字符设备结构图
static struct cdev chr_dev;

// 数据缓冲区
char buf1[BUFF_SIZE];
char buf2[BUFF_SIZE];

// 定义了四种行为，开启，释放，写， 读
static int chr_dev_open(struct inode *inode, struct file *filp);
static int chr_dev_release(struct inode *inode, struct file *filp);
static int chr_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos);
static int chr_dev_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos);

// 文件操作符与回调函数
static struct file_operations chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = chr_dev_open,
    .release = chr_dev_release,
    .read = chr_dev_read,
    .write = chr_dev_write,
};

/**
 * 设备驱动打开需要完成两个动作
 * 1. 设备号获取
 * 2. 配置缓存空间
 */
static int chr_dev_open(struct inode *inode, struct file *filp)
{
    printk("open\n");

    // 从给定设备中取出设备号
    switch (MINOR(inode->i_rdev))
    {
    case 0:
        // 私有buffer
        filp->private_data = buf1;
        break;
    case 1:
        // 私有buffer
        filp->private_data = buf2;
    default:
        break;
    }
    return 0;
}

/**
 * 设备释放
 */
static int chr_dev_release(struct inode *inode, struct file *filp)
{
    printk("release \n");
    return 0;
}

// 写数据， 
static int chr_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    // printk("write sone info");

    unsigned long p = *ppos;
    int ret;
    char *vbuf = filp->private_data;
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

static ssize_t chr_dev_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
    unsigned long p = *ppos;
    int ret;
    int tmp = count;
    char *vbuf = filp->private_data;
    if (p > BUFF_SIZE)
    {
        return 0;
    }

    if (tmp > BUFF_SIZE - p)
    {
        tmp = BUFF_SIZE - p;
    }

    ret = copy_to_user(buf, vbuf, tmp);
    *ppos += tmp;
    return tmp;
}

static int __init chrdev_init(void)
{
    int ret = 0;
    printk("chrdev init\n");

    // 获取设备号
    ret = alloc_chrdev_region(&devno, 0, DEV_CNT, DEV_NAME);
    if (ret < 0)
    {
        printk("fail to alloc devno \n");
        goto alloc_err;
    }

    // 设备初始化
    cdev_init(&chr_dev, &chr_dev_fops);

    // 添加设备
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

module_init(chrdev_init);

static void chrdev_exit(void)
{
    printk("exit into \n");
    unregister_chrdev_region(devno, DEV_CNT);

    cdev_del(&chr_dev);
}

module_exit(chrdev_exit);

MODULE_LICENSE("GPL");
