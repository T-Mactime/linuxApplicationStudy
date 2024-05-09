#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <asm/mach/map.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/io.h>
#include <linux/device.h>

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

struct i2c_client *mpu6050_client = NULL;

static int i2c_write_mpu6050(struct i2c_client *mpu6050_client, u8 address, u8 data){
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
    if(error != 1){
        printk(KERN_DEBUG "\n i2c transfer error \n");
        return -1;
    }
    return 0;
}

static int i2c_read_mpu6050(struct i2c_client *mpu6050_client, u8 address, void*data, u32 length){
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
    if(error != 2){
        printk(KERN_DEBUG "\n i2c read mpu 6050 error \n");
        return -1;
    }
    return 0;
}

