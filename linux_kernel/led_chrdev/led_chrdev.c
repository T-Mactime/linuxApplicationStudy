#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/init.h>

// 设备名称
#define DEV_NAME "LedChrDEV"
// 这里因为要控制三种不同的RGB灯，所以这里应该有三个不同的设备
#define DEV_CNT (3)

// 这里为什么是 500000000
#define AHB4_RERIPH_BASH (0x50000000)

// RCC是个什么东西，这里不确定
#define RCC_BASE (AHB4_RERIPH_BASH + 0x0000)
#define RCC_MP_GPIOENA (RCC_BASE + 0xA28)

#define GPIOA_BASE (AHB4_RERIPH_BASH + 0x2000)
#define GPIOA_MODER (GPIOA_BASE + 0x0000)
#define GPIOA_OTYPER (GPIOA_BASE + 0x0004)
#define GPIOA_DSPEEDR (GPIOA_BASE + 0x0008)
#define GPIOA_PUPDR (GPIOA_BASE + 0x000c)
#define GPIOA_BSRR (GPIOA_BASE + 0x018)

#define GPIOG_BASE (AHB4_RERIPH_BASH + 0x8000)
#define GPIOG_MODER (GPIOG_BASE + 0x0000)
#define GPIOG_OTYPER (GPIOG_BASE + 0x0004)
#define GPIOG_DSPEEDR (GPIOG_BASE + 0x0008)
#define GPIOG_PUPDR (GPIOG_BASE + 0x000c)
#define GPIOG_BSRR (GPIOG_BASE + 0x018)

#define GPIOB_BASE (AHB4_RERIPH_BASH + 0x3000)
#define GPIOB_MODER (GPIOB_BASE + 0x0000)
#define GPIOB_OTYPER (GPIOB_BASE + 0x0004)
#define GPIOB_DSPEEDR (GPIOB_BASE + 0x0008)
#define GPIOB_PUPDR (GPIOB_BASE + 0x000c)
#define GPIOB_BSRR (GPIOB_BASE + 0x018)

static dev_t devno;
struct class *led_chrdev_class;

struct led_chrdev
{
    struct cdev dev;
    unsigned int __iomem *va_moder;
    unsigned int __iomem *va_otyper;
    unsigned int __iomem *va_ospeeder;
    unsigned int __iomem *va_pupdr;
    unsigned int __iomem *va_bsrr;

    unsigned int led_pin;
};

unsigned int __iomem *va_clkaddr;

static int led_chrdev_open(struct inode *inode, struct file *filp)
{
    unsigned int val = 0;

    struct led_chrdev *led_cdev =
        (struct led_chrdev *)container_of(inode->i_cdev, struct led_chrdev, dev);
    filp->private_data = container_of(inode->i_cdev, struct led_chrdev, dev);

    printk("open\n");

    val |= (0x43); // 开启 a b g 时钟
    iowrite32(val, va_clkaddr);

    // 设置寄存器模式
    val = ioread32(led_cdev->va_moder);
    val &= ~((unsigned int)0x3 << (2 * led_cdev->led_pin));
    val += ((unsigned int)0x1 << (2 * led_cdev->led_pin));
    iowrite32(val, led_cdev->va_moder);
    // 设置输出类型寄存器： 推挽模式
    // 什么是推挽模式
    val = ioread32(led_cdev->va_otyper);
    val &= ~((unsigned int)0x1 << led_cdev->led_pin);
    iowrite32(val, led_cdev->va_otyper);
    // 设置输出速度寄存器 ： 高速
    val = ioread32(led_cdev->va_ospeeder);
    val &= ((unsigned int)0x3 << (2 * led_cdev->led_pin));
    val |= ((unsigned int)0x2 << (2 * led_cdev->led_pin));
    iowrite32(val, led_cdev->va_ospeeder);
    // 设置上下拉寄存器： 上拉
    val = ioread32(led_cdev->va_pupdr);
    val &= ~((unsigned int)0x3 << (2 * led_cdev->led_pin));
    val |= ((unsigned int)0x1 << (2 * led_cdev->led_pin));
    iowrite32(val, led_cdev->va_pupdr);

    val = ioread32(led_cdev->va_bsrr);
    val &= ~((unsigned int)0x1 << (led_cdev->led_pin + 16));
    iowrite32(val, led_cdev->va_bsrr);

    return 0;
}

static int led_chrdev_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t led_chrdev_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    unsigned long val = 0;
    unsigned long ret = 0;

    int tmp = count;

    struct led_chrdev *led_cdev = (struct led_chrdev *)filp->private_data;

    kstrtoul_from_user(buf, tmp, 10, &ret);
    iowrite32(0x43, va_clkaddr); // 开启GPIO时钟

    val = ioread32(led_cdev->va_bsrr);
    if (ret == 0)
    {
        val |= (0x01 << (led_cdev->led_pin + 16));
    }
    else
    {
        val |= (0x01 << led_cdev->led_pin);
    }

    iowrite32(val, led_cdev->va_bsrr);
    *ppos += tmp;
    return tmp;
}

static struct file_operations led_chrdev_fops = {
    .owner = THIS_MODULE,
    .open = led_chrdev_open,
    .release = led_chrdev_release,
    .write = led_chrdev_write,
};

static struct led_chrdev led_cdev[DEV_CNT] = {
    {.led_pin = 13}, // 定义GPIP引脚号
    {.led_pin = 2},
    {.led_pin = 5},
};

static __init int led_chrdev_init(void)
{
    int i = 0;
    dev_t cur_dev;

    printk("led chrdev init \n");

    va_clkaddr = ioremap(RCC_MP_GPIOENA, 4); // 映射物理地址到虚拟地址： gpio 时钟 rcc寄存器

    led_cdev[0].va_moder = ioremap(GPIOA_MODER, 4);
    led_cdev[0].va_otyper = ioremap(GPIOA_OTYPER, 4);
    led_cdev[0].va_ospeeder = ioremap(GPIOA_DSPEEDR, 4);
    led_cdev[0].va_pupdr = ioremap(GPIOA_PUPDR, 4);
    led_cdev[0].va_bsrr = ioremap(GPIOA_BSRR, 4);

    led_cdev[1].va_moder = ioremap(GPIOG_MODER, 4);
    led_cdev[1].va_otyper = ioremap(GPIOG_OTYPER, 4);
    led_cdev[1].va_ospeeder = ioremap(GPIOG_DSPEEDR, 4);
    led_cdev[1].va_pupdr = ioremap(GPIOG_PUPDR, 4);
    led_cdev[1].va_bsrr = ioremap(GPIOG_BSRR, 4);

    led_cdev[2].va_moder = ioremap(GPIOB_MODER, 4);
    led_cdev[2].va_otyper = ioremap(GPIOB_OTYPER, 4);
    led_cdev[2].va_ospeeder = ioremap(GPIOB_DSPEEDR, 4);
    led_cdev[2].va_pupdr = ioremap(GPIOB_PUPDR, 4);
    led_cdev[2].va_bsrr = ioremap(GPIOB_BSRR, 4);

    alloc_chrdev_region(&devno, 0, DEV_CNT, DEV_NAME);

    led_chrdev_class = class_create(THIS_MODULE, "led_chrdev");

    for (; i < DEV_CNT; i++)
    {
        cdev_init(&led_cdev[i].dev, &led_chrdev_fops);
        led_cdev[i].dev.owner = THIS_MODULE;

        cur_dev = MKDEV(MAJOR(devno), MINOR(devno) + i);

        cdev_add(&led_cdev[i].dev, cur_dev, 1);

        device_create(led_chrdev_class, NULL, cur_dev, NULL, DEV_NAME "%d", i);
    }

    return 0;
}

module_init(led_chrdev_init);

static __exit void led_chrdev_exit(void)
{
    int i;
    dev_t cur_dev;
    printk("led chrdev exit\n");

    iounmap(va_clkaddr);

    for (i = 0; i < DEV_CNT; i++)
    {
        iounmap(led_cdev[i].va_moder);
        iounmap(led_cdev[i].va_otyper);
        iounmap(led_cdev[i].va_ospeeder);
        iounmap(led_cdev[i].va_pupdr);
        iounmap(led_cdev[i].va_bsrr);
    }

    for (i = 0; i < DEV_CNT; i++)
    {
        cur_dev = MKDEV(MAJOR(devno), MINOR(devno) + 1);

        device_destroy(led_chrdev_class, cur_dev);

        cdev_del(&led_cdev[i].dev);
    }

    unregister_chrdev_region(devno, DEV_CNT);

    class_destroy(led_chrdev_class);
}
module_exit(led_chrdev_exit);

MODULE_AUTHOR("embedfire");
MODULE_LICENSE("GPL");