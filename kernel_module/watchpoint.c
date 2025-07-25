#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/kprobes.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/uaccess.h>
#include <linux/ptrace.h>
#include <linux/sched/debug.h>

static unsigned long watch_addr = 0;
module_param(watch_addr, ulong, 0664);
MODULE_PARM_DESC(watch_addr, "Address to watch");

static struct kobject *watch_kobj;


#define DR7_LOCAL_ENABLE 0x1
#define DR7_RW_READWRITE 0x3
#define DR7_LEN_4 0xC

static void set_watchpoint(void)
{
    unsigned long dr7;

    asm volatile("mov %0, %%dr0" :: "r" (watch_addr));
    asm volatile("mov %%dr7, %0" : "=r"(dr7));
    dr7 &= ~0xFF; // clear existing

    dr7 |= DR7_LOCAL_ENABLE | (DR7_RW_READWRITE << 16) | (DR7_LEN_4 << 18);
    asm volatile("mov %0, %%dr7" :: "r"(dr7));

    pr_info("Watchpoint set on address %lx\n", watch_addr);
}

static void clear_watchpoint(void)
{
    asm volatile("mov $0, %%dr7" ::: "memory");
    asm volatile("mov $0, %%dr0" ::: "memory");
    pr_info("Watchpoint cleared\n");
}

// debug exception
static int __kprobes dbg_handler(struct kprobe *p, struct pt_regs *regs)
{
    pr_info("Watchpoint hit at %p\n", (void *)watch_addr);
    dump_stack();  
    return 0;
}

static struct kprobe kp = {
    .symbol_name = "do_debug",
    .pre_handler = dbg_handler
};

static ssize_t addr_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%lx\n", watch_addr);
}

static ssize_t addr_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    kstrtoul(buf, 0, &watch_addr);
    set_watchpoint();
    return count;
}

static struct kobj_attribute addr_attr = __ATTR(watch_addr, 0664, addr_show, addr_store);

static int __init watch_init(void)
{
    pr_info("Watchpoint module loaded\n");
    kprobe_register(&kp);

    watch_kobj = kobject_create_and_add("watchpoint", kernel_kobj);
    if (!watch_kobj)
        return -ENOMEM;
    sysfs_create_file(watch_kobj, &addr_attr.attr);

    if (watch_addr)
        set_watchpoint();

    return 0;
}

static void __exit watch_exit(void)
{
    clear_watchpoint();
    kprobe_unregister(&kp);
    sysfs_remove_file(watch_kobj, &addr_attr.attr);
    kobject_put(watch_kobj);
    pr_info("Watchpoint module unloaded\n");
}

module_init(watch_init);
module_exit(watch_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Watchpoint kernel module");
