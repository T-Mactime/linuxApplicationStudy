#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include <asm/io.h>
#include <asm/mach/map.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/ide.h>
#include <linux/kernel.h>
#include <linux/types.h>

#include <linux/of.h>
#include <linux/of_address.h>

#define DEV_NAME "get_dts_info"
#define DEV_CNT (1)

// 定义字符设备的设备号
static dev_t led_devno;
// 定义字符设备结构体
static struct cdev led_chr_dev;
// create class
// 这里的类说的应该是设备类
struct class* led_chr_dev_class;

struct device_node* led_device_node;         // led 的设备树节点
struct device_node* rgb_led_red_device_node; // rgb_led_red 红灯节点
struct property* rgb_led_red_property;       // 定义属性结构体指针
int size = 0;
unsigned int out_values[18]; // 保存读取得到REG属性值

/*。open 属性*/
static int led_chr_dev_open(struct inode* inode, struct file* filp) {
    int error_status = 1;
    printk("open from device \n");

    // 获取dts属性值
    led_device_node = of_find_node_by_path("/led_test");
    if (led_device_node == NULL) {
        printk(KERN_ALERT "get led device node failed \n");
        return -1;
    }

    // 根据LED device node 设备节点结构体输出节点的基本信息
    printk(KERN_ALERT "name %s, ", led_device_node->name);
    printk(KERN_ALERT "child name %s\n", led_device_node->child->name);

    // 获取 rgb_led_red_device_node 的子节点
    rgb_led_red_device_node = of_get_next_child(led_device_node, NULL);
    if (rgb_led_red_device_node == NULL) {
        printk(KERN_ALERT "get rgb led red device node failed \n");
        return -1;
    }

    printk(KERN_ALERT "name %s ,", rgb_led_red_device_node->name);
    printk(KERN_ALERT "parent name %s \n", rgb_led_red_device_node->parent->name);

    // 获取 rgb_led_red_device_node 节点的compatible 属性
    rgb_led_red_property = of_find_property(rgb_led_red_device_node, "compatible", &size);
    if (rgb_led_red_property == NULL) {
        return -1;
    }

    printk(KERN_ALERT "size = %d ", size);
    printk(KERN_ALERT "name %s", rgb_led_red_property->name);
    printk(KERN_ALERT "length %d ", rgb_led_red_property->length);
    printk(KERN_ALERT "value %s", (char*)rgb_led_red_property->value);

    error_status = of_property_read_u32_array(rgb_led_red_device_node, "reg", out_values, 2);
    if (error_status != 0) {
        return -1;
    }
    printk(KERN_ALERT " 0x%08x", out_values[0]);
    printk(KERN_ALERT " 0x%08x", out_values[1]);

    return 0;
}

static int led_chr_dev_release(struct inode* inode, struct file* filp) {
    printk("release \n");
    return 0;
}

// 这里主要是注册两种不同的函数
static struct file_operations led_chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = led_chr_dev_open,
    .release = led_chr_dev_release,
};

static int __init
led_chrdev_init(void) {
    int ret = 0;
    printk("led chr dec inti \n");
    // 第一步，
    //  采用动态分布的方式获取设备号，此设备号为0
    //  设备in过程为embedchardev， 可以通过命令cat /proc/devices查看
    //  dev_cnt 为1  当前之申请一个设备编号
    ret = alloc_chrdev_region(&led_devno, 0, DEV_CNT, DEV_NAME);
    if (ret < 0) {
        goto alloc_error;
    }

    led_chr_dev_class = class_create(THIS_MODULE, "led_chrdev");

    // 第二步
    // 关联字符设备结构题cdev与文件操作结构体 file operations
    cdev_init(&led_chr_dev, &led_chr_dev_fops);
    // 第三步
    // 添加设备到cdev_cdev 散列表中
    ret = cdev_add(&led_chr_dev, led_devno, DEV_CNT);
    if (ret < 0) {
        goto add_err;
    }

    // 创建设备
    device_create(led_chr_dev_class, NULL, led_devno, NULL, DEV_NAME);

add_err:
    unregister_chrdev_region(led_devno, DEV_CNT);

alloc_error:
    return ret;
}

module_init(led_chrdev_init);

static void __exit led_chrdev_exit(void) {
    device_destroy(led_chr_dev_class, led_devno); //   清除设备
    cdev_del(&led_chr_dev);                       // 清除设备号
    unregister_chrdev_region(led_devno, DEV_CNT);
    class_destroy(led_chr_dev_class);
}

module_exit(led_chrdev_exit);

MODULE_LICENSE("GPL");