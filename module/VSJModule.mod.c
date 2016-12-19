#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x64ab665b, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x725e4d29, __VMLINUX_SYMBOL_STR(param_ops_ulong) },
	{ 0xa231a3de, __VMLINUX_SYMBOL_STR(param_ops_charp) },
	{ 0x2617fb1, __VMLINUX_SYMBOL_STR(rhashtable_free_and_destroy) },
	{ 0xcb3d4bd4, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0x64df917f, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xde72ed6e, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xbda84bfc, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x7b6fbe60, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x17cf767f, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x97301fa6, __VMLINUX_SYMBOL_STR(rhashtable_init) },
	{ 0x1fec93b0, __VMLINUX_SYMBOL_STR(rhashtable_insert_slow) },
	{ 0xe483ca2c, __VMLINUX_SYMBOL_STR(rhashtable_insert_rehash) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xd33a328c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xb99584d5, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xb5419b40, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x2e0d2f7f, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0x449ad0a7, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0xc671e369, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x88db9f48, __VMLINUX_SYMBOL_STR(__check_object_size) },
	{ 0xbba70a2d, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_bh) },
	{ 0xd9d3bcd3, __VMLINUX_SYMBOL_STR(_raw_spin_lock_bh) },
	{ 0x8bcafc9e, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x2f1826cb, __VMLINUX_SYMBOL_STR(nonseekable_open) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "4D92BF4C9DF3F1DBCD2955D");
