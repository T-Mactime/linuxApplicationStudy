#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf6bea569, "module_layout" },
	{ 0x3493d078, "class_destroy" },
	{ 0x6fb4652e, "cdev_del" },
	{ 0x3f2312f1, "device_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x2ce607fc, "device_create" },
	{ 0x1a843170, "cdev_add" },
	{ 0x7e8d8f84, "cdev_init" },
	{ 0xcc374b16, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x7e2d1370, "of_property_read_variable_u32_array" },
	{ 0x10e44cd6, "of_find_property" },
	{ 0xd07e39b6, "of_get_next_child" },
	{ 0xcb8eca16, "of_find_node_opts_by_path" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x7c32d0f0, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

