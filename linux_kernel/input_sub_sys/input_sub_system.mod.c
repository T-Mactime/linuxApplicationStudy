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
	{ 0x5e85ece, "input_unregister_device" },
	{ 0xd4e60711, "input_register_device" },
	{ 0x45d587f7, "input_set_capability" },
	{ 0xf3657854, "input_allocate_device" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x2072ee9b, "request_threaded_irq" },
	{ 0xc0fd0fb6, "irq_of_parse_and_map" },
	{ 0x6fb48941, "gpiod_direction_input" },
	{ 0xfe990052, "gpio_free" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x70966f66, "of_get_named_gpio_flags" },
	{ 0xcb8eca16, "of_find_node_opts_by_path" },
	{ 0x7c32d0f0, "printk" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x6da2fe2d, "input_event" },
	{ 0x8ce81b87, "gpiod_get_raw_value" },
	{ 0x243eeffb, "gpio_to_desc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

