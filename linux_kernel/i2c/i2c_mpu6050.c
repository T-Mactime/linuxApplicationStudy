#include <asm/io.h>
#include <asm/mach/map.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include <linux/platform_device.h>

#include "i2c_mpu6050.h"

#define DEV_NAME "I2C1_mpu6050"
#define DEV_CNT (1)

// 定义led资源结构题，获取到的节点信息以及转换后的虚拟寄存器地址
static dev_t mpu6050_devno;
static struct cdev mpu6050_chr_dev;
struct class* class_mpu6050;
struct device* device_mpu6050;
struct device_node* mpu6050_device_node;

struct i2c_client* mpu6050_client = NULL;

static int i2c_write_mpu6050(struct i2c_client* mpu6050_client, u8 address,
                             u8 data)
{
    int error = 0;
    u8 write_data[2];
    struct i2c_msg send_msg;

    write_data[0] = address;
    write_data[1] = data;

    send_msg.addr = mpu6050_client->addr;
    send_msg.flags = 0;
    send_msg.buf = write_data;
    send_msg.len = 2;

    error = i2c_transfer(mpu6050_client->adapter, &send_msg, 1);
        if (error != 1) {
            printk(KERN_DEBUG "\n i2c transfer error \n");
            return -1;
    }
    return 0;
}

static int i2c_read_mpu6050(struct i2c_client* mpu6050_client, u8 address,
                            void* data, u32 length)
{
    int error = 0;
    u8 address_data = address;
    struct i2c_msg mpu6050_msg[2];

    mpu6050_msg[0].addr = mpu6050_client->addr;
    mpu6050_msg[0].flags = 0;
    mpu6050_msg[0].buf = &address_data;
    mpu6050_msg[0].len = 1;

    mpu6050_msg[0].addr = mpu6050_client->addr;
    mpu6050_msg[0].flags = 0;
    mpu6050_msg[0].buf = &address_data;
    mpu6050_msg[0].len = 1;

    error = i2c_transfer(mpu6050_client->adapter, mpu6050_msg, 2);
        if (error != 2) {
            printk(KERN_DEBUG "\n i2c read mpu 6050 error \n");
            return -1;
    }
    return 0;
}

static int mpu6050_init(void)
{
    int error = 0;

    error += i2c_write_mpu6050(mpu6050_client, PWR_MGMT_1, 0x00);
    error += i2c_write_mpu6050(mpu6050_client, SMPLRT_DIV, 0x07);
    error += i2c_write_mpu6050(mpu6050_client, CONFIG, 0x06);
    error += i2c_write_mpu6050(mpu6050_client, ACCEL_CONFIG, 0x01);

        if (error < 0) {
            printk(KERN_DEBUG "\n mpu6050 init error \n");
            return -1;
    }

    return 0;
}

static int mpu6050_open(struct inode* inode, struct file* filp)
{
    mpu6050_init();
    return 0;
}

static int mpu6050_read(struct file* filp, char __user* buf, size_t cnt,
                        loff_t* off)
{
    char data_H = 0x00;
    char data_L = 0x00;

    int error;

    short mpu6050_result[6];

    i2c_read_mpu6050(mpu6050_client, ACCEL_XOUT_H, &data_H, 1);
    i2c_read_mpu6050(mpu6050_client, ACCEL_XOUT_L, &data_L, 1);
    mpu6050_result[0] = data_H << 8;
    mpu6050_result[0] += data_L;

    i2c_read_mpu6050(mpu6050_client, ACCEL_YOUT_H, &data_H, 1);
    i2c_read_mpu6050(mpu6050_client, ACCEL_YOUT_L, &data_H, 1);
    mpu6050_result[1] = data_H << 8;
    mpu6050_result[1] += data_L;

    i2c_read_mpu6050(mpu6050_client, ACCEL_ZOUT_H, &data_H, 1);
    i2c_read_mpu6050(mpu6050_client, ACCEL_ZOUT_L, &data_L, 1);
    mpu6050_result[2] = data_H << 8;
    mpu6050_result[2] += data_L;

    i2c_read_mpu6050(mpu6050_client, GYRO_XOUT_H, &data_H, 1);
    i2c_read_mpu6050(mpu6050_client, GYRO_XOUT_L, &data_L, 1);
    mpu6050_result[3] = data_H << 8;
    mpu6050_result[3] += data_L;

    i2c_read_mpu6050(mpu6050_client, GYRO_YOUT_H, &data_H, 1);
    i2c_read_mpu6050(mpu6050_client, GYRO_YOUT_L, &data_L, 1);
    mpu6050_result[4] = data_H << 8;
    mpu6050_result[4] += data_L;

    i2c_read_mpu6050(mpu6050_client, GYRO_ZOUT_H, &data_H, 1);
    i2c_read_mpu6050(mpu6050_client, GYRO_ZOUT_L, &data_L, 1);
    mpu6050_result[5] = data_H << 8;
    mpu6050_result[5] += data_L;

    error = copy_to_user(buf, mpu6050_result, cnt);

        if (error != 0) {
            printk("copu to user error\n");
            return -1;
    }

    return 0;
}

static int mpu6050_release(struct inode* inode, struct file* filp)
{
    return 0;
}

static struct file_operations mpu6050_chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = mpu6050_open,
    .read = mpu6050_read,
    .release = mpu6050_release,
};

static int mpu6050_probe(struct i2c_client* client,
                         const struct i2c_device_id* id)
{
    int ret = -1;

    printk(KERN_EMERG "\t match success \n");

    ret = alloc_chrdev_region(&mpu6050_devno, 0, DEV_CNT, DEV_NAME);
        if (ret < 0) {
            printk("failed to alloc devno \n");
            goto alloc_err;
    }

    mpu6050_chr_dev.owner = THIS_MODULE;
    cdev_init(&mpu6050_chr_dev, &mpu6050_chr_dev_fops);

    ret = cdev_add(&mpu6050_chr_dev, mpu6050_devno, DEV_CNT);
        if (ret < 0) {
            printk("fail to add cdev \n");
            goto add_err;
    }

    class_mpu6050 = class_create(THIS_MODULE, DEV_NAME);

    device_mpu6050 =
        device_create(class_mpu6050, NULL, mpu6050_devno, NULL, DEV_NAME);
    mpu6050_client = client;

    return 0;

add_err:
    unregister_chrdev_region(mpu6050_devno, DEV_CNT);
    printk("\t error\n");

alloc_err:
    return -1;
}

static int mpu6050_remove(struct i2c_client* client)
{
    device_destroy(class_mpu6050, mpu6050_devno);
    class_destroy(class_mpu6050);
    cdev_del(&mpu6050_chr_dev);
    unregister_chrdev_region(mpu6050_devno, DEV_CNT);
    return 0;
}

static const struct i2c_device_id gtp_device_id[] = {{"file, i2c_mpu6050", 0},
                                                     {}};

static const struct of_device_id mpu6050_of_match_table[] = {
    {.compatible = "file, i2c_mpu6050"}, {}};

struct i2c_driver mpu6050_driver = {
    .probe = mpu6050_probe,
    .remove = mpu6050_remove,
    .id_table = gtp_device_id,
    .driver =
        {
            .name = "fire, i2c_mpu6050",
            .owner = THIS_MODULE,
            .of_match_table = mpu6050_of_match_table,
        },
};

static int __init mpu6050_driver_init(void)
{
    int ret;
    pr_info("mpu6050_driver_init\n");
    ret = i2c_add_driver(&mpu6050_driver);
    return ret;
}

static void __exit mpu6050_driver_exit(void)
{
    pr_info("mpu6050_driver_exit \n");
    i2c_del_driver(&mpu6050_driver);
}

module_init(mpu6050_driver_init);
module_exit(mpu6050_driver_exit);

MODULE_LICENSE("GPL");