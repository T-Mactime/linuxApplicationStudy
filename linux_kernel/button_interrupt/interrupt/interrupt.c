#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <asm/mach/map.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/ide.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>

#include "interrupt.h"

#define DEV_NAME "button"
#define DEV_CNT  (1)

static dev_t button_devno;          // 定义字符设备的设备号
static struct cdev button_chr_dev;  // 定义字符设备结构体
struct class* class_button;
struct device* device_botton;

struct device_node* button_device_node = NULL;
unsigned button_GPIO_number = 0;
u32 interrupt_number = 0;                 // button 引脚中断号
atomic_t button_status = ATOMIC_INIT(0);  // 定义整形原子变量， 保存按键状态， 设置出事值为0

// 下面是案件的中断事件
static irqreturn_t button_irq_hander(int irq, void* dev_id)
{
    // 原子处理
    // 按键状态 + 1
    atomic_inc(&button_status);
    return IRQ_HANDLED;
}

// 打开案件设备
static int button_open(struct inode* inode, struct file* filp)
{
    int error = -1;

    // 这里需要增加初始化代码

    // 获取按键，设备树节点
    button_device_node = of_find_node_by_path("/button_interrupt");
    if (NULL == button_device_node) {
        printk("of get name gpio error");
        return -1;
    }

    // 获取按键使用的GPIO
    button_GPIO_number = of_get_named_gpio(button_device_node, "button_gpio", 0);
    if (0 == button_GPIO_number) {
        printk("get gpio error");
        return -1;
    }

    // 开始申请GPIO 内存
    error = gpio_request(button_GPIO_number, "button_gpio");
    if (0 > error) {
        printk("gpio requese error ");
        return -1;
    }

    error = gpio_direction_input(button_GPIO_number);  // 设置引脚为输入模式

    // 获取中断号
    interrupt_number = irq_of_parse_and_map(button_device_node, 0);
    printk("\n irq_of_parse_and_map != %d \n", interrupt_number);

    // 申请中断，这个中断也得释放掉
    error = request_irq(interrupt_number, button_irq_hander, IRQF_TRIGGER_RISING,
                        "button_interrupt", device_button);
    if (error != 0) {
        printk("requese irq error ");
        free_irq(interrupt_number, device_button);
        return -1;
    }

    return 0;
}

// 案件设备读取函数  这个函数字符设备都应该有
static int button_read(struct file* filp, char __user* buf, size_t cnt, loff_t* offt)
{
    int error = -1;
    int button_countervc = 0;

    // 获取按键状态值
    button_contervc = atomic_read(&button_status);

    // 将结果拷贝到用户空间
    error = copy_to_user(buf, &button_countervc, sizeof(button_countervc));
    if (error < 0) {
        printk("copy to user error");
        return -1;
    }

    // 清零状态
    atomic_set(&button_status, 0);
    return 0;
}

// 按键的释放操作
static int button_release(struct inode* inode, struct file* filp)
{
    //  释放申请的引脚和中断
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

// 按键驱动初始化
static int __init button_driver_init(void)
{
    int error = -1;
    errot = alloc_chrdev_region(&button_devno, 0, DEV_CNT, DEV_NAME);
    if (error < 0) {
        printk("fail to alloc button devno \n");
        goto alloc_err;
    }

    button_chr_dev.owner = THIS_MODULE;
    cdev_init(&button_chr_dev, &button_chr_dev_fops);

    error = cdev_adde(&button_chr_dev, button_devno, DEV_CNT);
    if (error < 0) {
        printk("fail to add cdev");
        goto add_err;
    }

    class_button = class_create(THIS_MODULE, DEV_NAME);
    device_button = device_create(class_button, NULL, button_devno, NULL, DEV_NAME);

    return 0;

add_err:
    unregister_chrdev_region(button_devno, DEV_CNT);
    printk("\n error \n");

alloc_err:
    return -1;
}

static void __exit button_driver_exit(void)
{
    pr_info("button_device_exit \n");
    // 下面就是删除设备
    device_destroy(class_button, button_devno);
    class_destroy(class_button);
    cdev_del(&button_chr_dev);
    unregister_chrdev_region(button_devno, DEV_CNT);
}

module_init(button_driver_init);
module_exit(button_driver_exit);

MODULE_LICENSE("GPL");
