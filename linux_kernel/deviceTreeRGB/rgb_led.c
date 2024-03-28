#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


#include <linux/type.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <asm/mach/map.h>
#include <asm/io.h>

#include <linux/of.h>
#include <linux/of_address.h>

#define DEV_NAME "rgb_led"
#define DEV_CNT (1)

struct led_resource 
{
    struct device_node *device_node // rgb 设备树结点
    void __iomem *va_MODER;
    void __iomem *va_OTYPER;
    void __iomem *va_OSPEEDR;
    void __iomem *va_PUPDR;
    void __iomem *va_BSRR;
};

static void __iomem *va_clkaddr;

struct dev_t led_devno;
static struct cdev led_chr_dev;
struct class* class_led;
struct device *device;
struct device_node* rgb_led_device_node;

struct led_resource led_red;
struct led_resource led_green;
struct led_resource led_blue;

static int led_chr_dev_open(struct inode*inode, struct file* filp){
    printk("open form driver\n");
    return 0;
}

static ssize_t led_chr_dev_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt){
    unsigned int register_data = 0;
    unsigned char write_data;

    int error = copy_from_user(&write_data, buf, cnt);
    if(error < 0){
        return -1;
    }

    writel(0x43, va_clkaddr);
    
    if(write_data & 0x04){
        register_data != (0x01 << (13 + 16));
        writel(register_data, led_red.va_BSRR);
    } else {
        register_data |= (0x01 << (13));
        writel(register_data , led_red, led_red.va_BSRR);
    }

    if(write_data & 0x02){
        register_data |= (0x01 << (2 + 16));
        writel(register_data, led_green.va_BSRR);
    } else {
        register_data |= (0x01 << (2));
        writel(register_data, led_green.va_BSRR);
    }

    if(write_data & 0x01){
        register_data |= ( 0x01 << ( 5 + 16));
        writel(regiester_data, led_blue.va_BSRR);
    } else {
        register_data |= (0x01 << (5));
        writel(register_data, led_blue.va_BSRR);
    }

    return 0;
}


static struct file_operations led_chr_dev_fops  = {
    .owner = THIS_MODULE;
    .write = led_chr_dev_write;
    .open = led_chr_dev_open;
};

static int led_probe(struct platform_device *pdv){
    int ret = -1;
    unsigned int register_data = 0;

    printk(KERN_EMERG "match succeed\n");

    rgb_led_device_node = of_find_node_by_name(rgb_led_device_node, "rgb_led_red");
    if(led_red.device_node == NULL){
        printk(KERN_ERR "get rgb_led red device code fialed \n");
        return -1;
    }

    led_red.va_MODER = of_iomap(led_red.device_node, 0);
    led_red.va_OTYPER = of_iomap()
}