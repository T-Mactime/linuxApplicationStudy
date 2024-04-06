#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
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

#include <linux/platform_device.h>

/** 字符设备内容 */
#define DEV_NAME "rgb_led"
#define DEV_CNT (1)

// 定义字符设备设备号
// 这里需要说明，一个设备应该有一个设备号和一个设备信息，这里dev_t指的是设备号，设备信息是在cdev中指定的，
// 表明这里其实是一个字符设备
static dev_t led_devno;
static struct cdev led_chr_dev;

// 这里是对设备信息的标注，包括设备类，设备，设备节点
struct class* class_led;              // 这是设备类
struct device* device;                   // 创建的设备
struct device_node* rgb_led_device_node; // rgb led 设备节点

// 这里用于保存获取到的各个功能引脚
int rgb_led_red;
int rgb_led_green;
int rgb_led_blue;

/** 字符设备操作函数集 open*/
/**
 * @brief 在样例程序中，其实open没有做任何动作，所以这里是没有做任何处理
 */
static int led_chr_dev_open(struct inode* inode, struct file* filp) {
    printk("open rgb led with pinctrl\n");
    return 0;
}

/* 这里实际上是对已经生成的设备文件进行修改 */
static ssize_t led_chr_dev_write(struct file* filp, const char __user* buf,
                                 size_t cnt, loff_t* loff) {
    unsigned char write_data; // 这里主要是为了读取聪buf中读取的字符
    // 定义一个错误信息返回值
    int error = 0;

    // 从buf中读取一个字符到write_data 中，这里是从用户态到内核态的空间拷贝
    error = copy_from_user(&write_data, buf, cnt);
    if (error < 0) {
        return -1;
    }

    /**
     * @brief 下面这三个循环其实就是调用gpio接口，实现对输出引脚高低电平的调整
     * 与前面实现相同的是，其实这里是通过一个字节空气三盏灯，分别通过一个字节的不同比特位，控制三盏灯的亮灭
     *
     */

    if (write_data & 0x04) {
        // 低第三位，是1  红灯亮
        gpio_direction_output(rgb_led_red, 0);
    } else {
        // 否则，红灯灭
        gpio_direction_output(rgb_led_red, 1);
    }

    if (write_data & 0x02) {
        // 低第二位，是1 绿灯亮
        gpio_direction_output(rgb_led_green, 0);
    } else {
        // 否则，绿灯灭
        gpio_direction_output(rgb_led_green, 1);
    }

    if (write_data & 0x01) {
        // 低第二位，是1 蓝亮
        gpio_direction_output(rgb_led_blue, 0);
    } else {
        // 否则，绿灯灭
        gpio_direction_output(rgb_led_blue, 1);
    }

    return 0;
}

// 设备的行为函数定义结束，下面应该注册进去
static struct file_operations led_chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = led_chr_dev_open,
    .write = led_chr_dev_write,
};

// 设备的基础行为定义结束，剩下的就是创建驱动设备，并且将刚刚定义的行为信息注册到设备中
static int led_probe(struct platform_device* pdv) {
    // 这是探针函数
    int ret = 0;

    printk(KERN_EMERG "match successed \n");

    // 根据设备信息查找对应的设备

    // 获取RGB 设备树节点
    rgb_led_device_node = of_find_node_by_path("/rgb_led");
    if (rgb_led_device_node == NULL) {
        printk(KERN_EMERG "find led device by name failed \n");
    }

    // 获取RGB灯对应的GPIO引脚
    rgb_led_red = of_get_named_gpio(rgb_led_device_node, "rgb_led_red", 0);
    rgb_led_green = of_get_named_gpio(rgb_led_device_node, "rgb_led_green", 0);
    rgb_led_blue = of_get_named_gpio(rgb_led_device_node, "rgb_led_blue", 0);

    printk("%d %d %d\n ", rgb_led_red, rgb_led_green, rgb_led_blue);

    // 这里获取到了所有的rgb gpio节点，然后对其进行初始化，让其默认输出高电平
    gpio_direction_output(rgb_led_red, 1);
    gpio_direction_output(rgb_led_green, 1);
    gpio_direction_output(rgb_led_blue, 1);

    // 初始化结束，剩下的就是创建设备的过程
    // 第一步，采用动态分配的方式，获取设备号，此设备号为0
    // 设备名称为rgb-leds
    ret = alloc_chrdev_region(&led_devno, 0, DEV_CNT, DEV_NAME);
    if (ret < 0) {
        printk("alloc error \n");
        goto alloc_error;
    }

    // 第二部，关联字符设备结构体，cdev 与文件操作
    led_chr_dev.owner = THIS_MODULE;
    cdev_init(&led_chr_dev, &led_chr_dev_fops);

    // 添加设备
    cdev_add(&led_chr_dev, led_devno, DEV_CNT);
    if (ret < 0) {
        printk("failed to add cdev\n");
        goto add_error;
    }

    // 第四部，创建类
    class_led = class_create(THIS_MODULE, DEV_NAME);
    // 创建设备
    device = device_create(class_led, NULL, led_devno, NULL, DEV_NAME);

    return 0;
add_error:
    unregister_chrdev_region(led_devno, DEV_CNT);

alloc_error:
    return -1;
}

// 这里定义rgb led 设备的属性信息
static const struct of_device_id rgb_led[] = {{.compatible = "fire,rgb-led"},
                                              {}};

/*定义平台设备结构体*/
struct platform_driver led_platform_driver = {.probe = led_probe,
                                              .driver = {
                                                  .name = "rgb-leds-platform",
                                                  .owner = THIS_MODULE,
                                                  .of_match_table = rgb_led,
                                              }};

static int __init led_platform_driver_init(void) {
    int ds;
    ds = platform_driver_register(&led_platform_driver);
    return 0;
}

static void __exit led_platform_driver_exit(void) {
    device_destroy(class_led, led_devno);
    class_destroy(class_led);
    cdev_del(&led_chr_dev);
    unregister_chrdev_region(led_devno, DEV_CNT);

    platform_driver_unregister(&led_platform_driver);
}

module_init(led_platform_driver_init);
module_exit(led_platform_driver_exit);

MODULE_LICENSE("GPL");