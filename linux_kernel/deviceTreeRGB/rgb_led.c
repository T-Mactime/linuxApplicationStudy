#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include <asm/io.h>
#include <asm/mach/map.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/ide.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/types.h>

#include <linux/platform_device.h>

#define DEV_NAME "reg_led"
#define DEV_CNT (1)

/* 定义led 资源结构体， 保存获取到的节点心爱以及转换后的虚拟寄存器地址 */
struct led_resouce
{
    struct device_node* device_node; // rgb_led_red 设备树节点

    void __iomem* va_MODER;
    void __iomem* va_OTYPER;
    void __iomem* va_OSPEEDR;
    void __iomem* va_PUPDR;
    void __iomem* va_BSRR;
};

// 这个是时钟
static void __iomem* va_clkaddr;

static dev_t led_devno;                  // 定义一个字符设备的设备号
static struct cdev led_chr_dev;          // 定义字符设备结构体 chr_dev
struct class* class_led;                 // 定义led 设备类
struct device* device;                   // 定义一个设备
struct device_node* rgb_led_device_node; // rgb_led 的设备树节点结构体

// 定义RGB灯的三个led_srouces 结构体， 保存获取到的节点信息
struct led_resouce led_red;
struct led_resouce led_green;
struct led_resouce led_blue;

/* 字符设备操作函数集 open函数*/
static int led_chr_dev_open(struct inode* inode, struct file* filp)
{
    printk("This open function\n");
    return 0;
}

// 字符设备操作函数集 write函数
static ssize_t led_chr_dev_write(struct file* filp, const char __user* buf,
                                 size_t cnt, loff_t* offt)
{

    unsigned int register_data = 0; // 暂存读取得到的寄存器数据
    unsigned char write_data;       // 用于保存接收到的数据

    int error = copy_from_user(&write_data, buf, cnt);
        if (error < 0) {
            return -1;
    }

    /**
     * @brief
     *      这里这些代码应该结合实际调用来看，这里的输出，，也就是write_data
     * 实际上是输入了一个字节，这里 是根据字节处理信号的
     */

    // 开启rgb 的时钟
    writel(0x43, va_clkaddr);
        /* 设置 GPIOA13 输出电平 */
        if (write_data & 0x04) {
            register_data |= (0x01 << (16 + 13));
            // GPIOA13 引脚输出低电平，红灯亮
            writel(register_data, led_red.va_BSRR);
        } else {
            register_data |= (0x01 << (13));
            writel(register_data, led_red.va_BSRR);
        }

        /* 设置 GPIO G2 输出电平*/
        if (write_data & 0x02) {
            register_data |= (0x01 << (2 + 16));
            writel(register_data, led_green.va_BSRR);
        } else {
            register_data |= (0x01 << (2));
            writel(register_data, led_green.va_BSRR);
        }

        /* 设置 GPIO G5 输出电平*/
        if (write_data & 0x01) {
            register_data |= (0x01 << (5 + 16));
            writel(register_data, led_blue.va_BSRR);
        } else {
            register_data |= (0x01 << (5));
            writel(register_data, led_blue.va_BSRR);
        }

    return 0;
}

static struct file_operations led_chr_dev_fops = {
    .owner = THIS_MODULE,
    .write = led_chr_dev_write,
    .open = led_chr_dev_open,
};

static int led_probe(struct platform_device* pdv)
{

    int ret = -1;
    unsigned int register_data = 0;

    printk(KERN_EMERG "match successed \n");

    /* 获取rgb_led 的设备树节点*/
    rgb_led_device_node = of_find_node_by_path("/rgb_led");
        if (rgb_led_device_node == NULL) {
            printk(KERN_ERR "get rgb led failed\n");
            return -1;
    }

    /* 获取设备树节点 */
    led_red.device_node =
        of_find_node_by_name(rgb_led_device_node, "rgb_led_red");
        if (led_red.device_node) {
            printk(KERN_ERR "get rgb led red device node failed\n");
            return -1;
    }

    /* 获取reg 属性并转化为虚拟地址 */
    led_red.va_MODER = of_iomap(led_red.device_node, 0);
    led_red.va_OTYPER = of_iomap(led_red.device_node, 1);
    led_red.va_OSPEEDR = of_iomap(led_red.device_node, 2);
    led_red.va_PUPDR = of_iomap(led_red.device_node, 3);
    led_red.va_BSRR = of_iomap(led_red.device_node, 4);
    va_clkaddr = of_iomap(led_red.device_node, 5);

    register_data = readl(va_clkaddr);
    // 开启时钟
    register_data |= (0x43); // 开启时钟
    writel(register_data, va_clkaddr);

    /**
     * @brief 这里设置寄存器输出模式，推挽模式，高速，上拉，高低电平等
     *
     */

    // 这个时设置输出模式
    register_data = readl(led_red.va_MODER);
    register_data &= ~((unsigned int)0x3 << (2 * 13));
    register_data |= ((unsigned int)0x1 << (2 * 13));
    writel(register_data, led_red.va_MODER);

    // 设置推挽模式
    register_data = readl(led_red.va_OTYPER);
    register_data &= ~((unsigned int)0x1 << 13);
    writel(register_data, led_red.va_OTYPER);

    // 设置输出速率寄存器
    register_data = readl(led_red.va_OSPEEDR);
    register_data &= ~((unsigned int)0x3 << (2 * 13));
    register_data |= ((unsigned int)0x2 << (2 * 13));
    writel(register_data, led_red.va_OSPEEDR);

    // 设置上下拉寄存器 上拉
    register_data = readl(led_red.va_PUPDR);
    register_data &= ~((unsigned int)0x3 << (2 * 13));
    register_data |= ((unsigned int)0x1 << (2 * 13));
    writel(register_data, led_red.va_PUPDR);

    // 设置寄存器 默认输出高电平
    register_data = readl(led_red.va_BSRR);
    register_data &= ~((unsigned int)0x1 << (13));
    writel(register_data, led_red.va_BSRR);

    /* 获取设备树节点 */
    led_red.device_node =
        of_find_node_by_name(rgb_led_device_node, "rgb_led_green");
        if (led_red.device_node) {
            printk(KERN_ERR "get rgb green red device node failed\n");
            return -1;
    }

    /* 获取reg 属性并转化为虚拟地址 */
    led_green.va_MODER = of_iomap(led_green.device_node, 0);
    led_green.va_OTYPER = of_iomap(led_green.device_node, 1);
    led_green.va_OSPEEDR = of_iomap(led_green.device_node, 2);
    led_green.va_PUPDR = of_iomap(led_green.device_node, 3);
    led_green.va_BSRR = of_iomap(led_green.device_node, 4);
    va_clkaddr = of_iomap(led_green.device_node, 5);

    // 这个时设置输出模式
    register_data = readl(led_green.va_MODER);
    register_data &= ~((unsigned int)0x3 << (2 * 2));
    register_data |= ((unsigned int)0x1 << (2 * 2));
    writel(register_data, led_green.va_MODER);

    // 设置推挽模式
    register_data = readl(led_green.va_OTYPER);
    register_data &= ~((unsigned int)0x1 << 2);
    writel(register_data, led_green.va_OTYPER);

    // 设置输出速率寄存器
    register_data = readl(led_green.va_OSPEEDR);
    register_data &= ~((unsigned int)0x3 << (2 * 2));
    register_data |= ((unsigned int)0x2 << (2 * 2));
    writel(register_data, led_green.va_OSPEEDR);

    // 设置上下拉寄存器 上拉
    register_data = readl(led_green.va_PUPDR);
    register_data &= ~((unsigned int)0x3 << (2 * 2));
    register_data |= ((unsigned int)0x1 << (2 * 2));
    writel(register_data, led_green.va_PUPDR);

    // 设置寄存器 默认输出高电平
    register_data = readl(led_green.va_BSRR);
    register_data &= ~((unsigned int)0x1 << (2));
    writel(register_data, led_green.va_BSRR);

    /* 获取设备树节点 */
    led_red.device_node =
        of_find_node_by_name(rgb_led_device_node, "rgb_led_blue");
        if (led_red.device_node) {
            printk(KERN_ERR "get rgb led blue device node failed\n");
            return -1;
    }

    /* 获取reg 属性并转化为虚拟地址 */
    led_blue.va_MODER = of_iomap(led_blue.device_node, 0);
    led_blue.va_OTYPER = of_iomap(led_blue.device_node, 1);
    led_blue.va_OSPEEDR = of_iomap(led_blue.device_node, 2);
    led_blue.va_PUPDR = of_iomap(led_blue.device_node, 3);
    led_blue.va_BSRR = of_iomap(led_blue.device_node, 4);
    va_clkaddr = of_iomap(led_blue.device_node, 5);

    register_data = readl(va_clkaddr);
    // 开启时钟
    register_data |= (0x43); // 开启时钟
    writel(register_data, va_clkaddr);

    /**
     * @brief 这里设置寄存器输出模式，推挽模式，高速，上拉，高低电平等
     *
     */

    // 这个时设置输出模式
    register_data = readl(led_blue.va_MODER);
    register_data &= ~((unsigned int)0x3 << (2 * 5));
    register_data |= ((unsigned int)0x1 << (2 * 5));
    writel(register_data, led_blue.va_MODER);

    // 设置推挽模式
    register_data = readl(led_blue.va_OTYPER);
    register_data &= ~((unsigned int)0x1 << 5);
    writel(register_data, led_blue.va_OTYPER);

    // 设置输出速率寄存器
    register_data = readl(led_blue.va_OSPEEDR);
    register_data &= ~((unsigned int)0x3 << (2 * 5));
    register_data |= ((unsigned int)0x2 << (2 * 5));
    writel(register_data, led_blue.va_OSPEEDR);

    // 设置上下拉寄存器 上拉
    register_data = readl(led_blue.va_PUPDR);
    register_data &= ~((unsigned int)0x3 << (2 * 5));
    register_data |= ((unsigned int)0x1 << (2 * 5));
    writel(register_data, led_blue.va_PUPDR);

    // 设置寄存器 默认输出高电平
    register_data = readl(led_red.va_BSRR);
    register_data &= ~((unsigned int)0x1 << (5));
    writel(register_data, led_red.va_BSRR);

    /**
     * @brief 前面是初始化一些设备的基本信息，后面是创建设备类，设备树
     *
     *
     */

    /**
     * 第一步
     * 采用动态分配的方式，获取设备编号，次设备号为0
     * 设备名称为rgb leds 可以通过cat/proc/devices 查看
     * dev_cnt为1 当前只申请一个设备编号
     *
     */
    ret = alloc_chrdev_region(&led_devno, 0, DEV_CNT, DEV_NAME);
        if (ret < 0) {
            printk("failed to alloc led devno\n");
            goto alloc_err;
    }

    /**
     * @brief
     *  第二步
     * 关联字符设备结构体 cdev与文件操作结构体 file_operations
     */
    led_chr_dev.owner = THIS_MODULE;
    cdev_init(&led_chr_dev, &led_chr_dev_fops);
    /**
     * @brief 第三步
     * 添加设备至 cdev_map 散列表中
     *
     */
    ret = cdev_add(&led_chr_dev, led_devno, DEV_CNT);
        if (ret < 0) {
            printk("failed add dev\n");
            goto add_err;
    }

    /**
     * @brief 第四步
     * 创建类
     *
     */
    class_led = class_create(THIS_MODULE, DEV_NAME);

    /**
     * 创建设备
     */
    device = device_create(class_led, NULL, led_devno, NULL, DEV_NAME);

    return 0;

add_err:
    // 添加设备失败时， 需要注销设备号
    unregister_chrdev_region(led_devno, DEV_CNT);
    printk("error\n");

alloc_err:
    return -1;
}

static const struct of_device_id rgb_led[] = {{.compatible = "file,rgb_led"},
                                              {}};

/** 定义设备结构体*/
struct platform_driver led_platform_driver = {.probe = led_probe,
                                              .driver = {
                                                  .name = "rgb-leds-platform",
                                                  .owner = THIS_MODULE,
                                                  .of_match_table = rgb_led,
                                              }};

/**
 * 驱动初始化
 */
static int __init led_platform_driver_init(void)
{
    int DriverState;
    DriverState = platform_driver_register(&led_platform_driver);
    printk(KERN_EMERG "derive state is %d\n", DriverState);
    return 0;
}

/**
 * 驱动注销函数
 */
static void __exit led_platform_driver_exit(void)
{
    /* 取消物理地址映射到虚拟地址 */
    iounmap(va_clkaddr);
    iounmap(led_green.va_MODER);
    iounmap(led_green.va_OTYPER);
    iounmap(led_green.va_OSPEEDR);
    iounmap(led_green.va_PUPDR);
    iounmap(led_green.va_BSRR);

    iounmap(led_red.va_MODER);
    iounmap(led_red.va_OTYPER);
    iounmap(led_red.va_OSPEEDR);
    iounmap(led_red.va_PUPDR);
    iounmap(led_red.va_BSRR);

    iounmap(led_blue.va_MODER);
    iounmap(led_blue.va_OTYPER);
    iounmap(led_blue.va_OSPEEDR);
    iounmap(led_blue.va_PUPDR);
    iounmap(led_blue.va_BSRR);

    /* 删除设备 */
    device_destroy(class_led, led_devno);
    class_destroy(class_led);
    cdev_del(&led_chr_dev);
    unregister_chrdev_region(led_devno, DEV_CNT);

    platform_driver_unregister(&led_platform_driver);

    printk("hello world\n");
}

module_init(led_platform_driver_init);
module_exit(led_platform_driver_exit);

MODULE_LICENSE("GPL");