#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <asm/mach/map.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/of_irq.h>

#include "interrupt.h"

// 首先是字符设备定义的内容
#define DEV_NAME "button"
#define DEV_CNT  (1)

static dev_t button_devno;          // 定义一个字符设备设备号
static struct cdev button_chr_dev;  // 定义字符设备结构体 chr_dev
struct class *class_button;         // 设备类
struct device *device_button;       // 创建的设备

struct device_node *button_device_node = NULL;  // 定义按键设备节点结构体
unsigned button_GPIO_number = 0;                // 保存button使用的GPIO引脚编号
u32 interrupt_number = 0;
atomic_t button_status = ATOMIC_INIT(0);  // 定义整形原子变量， 保存按键状态， 设置初始值为0

// 这是任务列表吗
struct tasklet_struct button_tasklet;

void button_tasklet_hander(unsigned long data)
{
    int counter = 1;
    mdelay(200);
    printk(KERN_ERR "button tasklet hander counter = %d\n", counter++);
    mdelay(200);
    printk(KERN_ERR "button tasklet hander counter = %d\n", counter++);
    mdelay(200);
    printk(KERN_ERR "button tasklet hander counter = %d\n", counter++);
    mdelay(200);
    printk(KERN_ERR "button tasklet hander counter = %d\n", counter++);
    mdelay(200);
    printk(KERN_ERR "button tasklet hander counter = %d\n", counter++);
}

static irqreturn_t button_irq_hander(int irq, void *dev_id)
{
    atomic_inc(&button_status);

    printk(KERN_ERR "button irq button inter\n");
    tasklet_schedule(&button_tasklet);

    return IRQ_HANDLED;
}

static int button_open(struct inode *inode, struct file *filp)
{
    int error = -1;

    // 获取按键，设备树节点
    button_device_node = of_find_node_by_path("/button_interrupt");
    if (NULL == button_device_node) {
        printk(KERN_ERR "get device node failed\n");
        return -1;
    }

    button_GPIO_number = of_get_named_gpio(button_device_node, "button_gpio", 0);
    if (0 == button_GPIO_number) {
        printk(KERN_ERR "of get named gpio failed\n");
        return -1;
    }

    error = gpio_request(button_GPIO_number, "button_gpio");
    if (error < 0) {
        printk(KERN_ERR "gpio_requese failed\n");
        gpio_free(button_GPIO_number);
        return -1;
    }

    // 设置GPIO引脚为输入模式
    error = gpio_direction_input(button_GPIO_number);

    // 获取中断号
    interrupt_number = irq_of_parse_and_map(button_device_node, 0);
    printk(KERN_INFO "irq of parse and map = %d\n", interrupt_number);

    error = request_irq(interrupt_number, button_irq_hander, IRQF_TRIGGER_RISING,
                        "button_interrupt", device_button);
    if (error != 0) {
        printk("request_irq_error");
        free_irq(interrupt_number, device_button);
        return -1;
    }

    tasklet_init(&button_tasklet, button_tasklet_hander, 0);
    return 0;
}

static int button_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    int error = -1;
    int button_countervc = 0;

    // 读取按键状态值
    button_countervc = atomic_read(&button_status);

    // 结果要拷贝到用户空间
    error = copy_to_user(buf, &button_countervc, sizeof(button_countervc));
    if (error < 0) {
        printk(KERN_ERR "copy to user failed\n");
        return -1;
    }

    // 清零按键状态
    atomic_set(&button_status, 0);
    return 0;
}

static int button_release(struct inode *inode, struct file *filp)
{
    gpio_free(button_GPIO_number);
    free_irq(interrupt_number, device_button);
    return 0;
}

// 字符设备操作函数集
static struct file_operations button_chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = button_open,
    .read = button_read,
    .release = button_release,

};

// 初始化函数
static int __init button_device_init(void)
{
    int error = -1;

    // 采用动态分布的方式，获取设备好，次设备号为0
    error = alloc_chrdev_region(&button_devno, 0, DEV_CNT, DEV_NAME);
    if (error < 0) {
        printk(KERN_ERR "fail to alloc button devno\n");
        goto alloc_err;
    }

    // 关键字符设备结构体cdev与文件操作结构体 file_operations
    button_chr_dev.owner = THIS_MODULE;
    cdev_init(&button_chr_dev, &button_chr_dev_fops);

    error = cdev_add(&button_chr_dev, button_devno, DEV_CNT);
    if (error < 0) {
        printk(KERN_ERR "fail to add cdev\n");
        goto add_err;
    }

    class_button = class_create(THIS_MODULE, DEV_NAME);
    device_button = device_create(class_button, NULL, button_devno, NULL, DEV_NAME);

    return 0;

add_err:
    unregister_chrdev_region(button_devno, DEV_CNT);
    printk(KERN_ERR "error \n");

alloc_err:
    return -1;
}

static void __exit button_driver_exit(void)
{
    pr_info("button_driver exit\n");

    // 删除设备
    device_destroy(class_button, button_devno);
    class_destroy(class_button);
    cdev_del(&button_chr_dev);
    unregister_chrdev_region(button_devno, DEV_CNT);
}

module_init(button_device_init);
module_exit(button_driver_exit);

MODULE_LICENSE("GPL");