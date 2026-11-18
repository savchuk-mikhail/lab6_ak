// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/bug.h>

static unsigned int count = 1;

module_param(count, uint, 0644);
MODULE_PARM_DESC(count, "Number of Hello, world! messages");

struct hello_event {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_events);

static int __init hello_init(void)
{
	struct hello_event *event;
	struct hello_event *tmp;
	unsigned int i;

	BUG_ON(count > 10);

	if (count == 0 || (count >= 5 && count <= 10))
		pr_warn("Unusual count value: %u\n", count);

	for (i = 0; i < count; i++) {
		if (count == 3 && i == 2)
			event = NULL;
		else
			event = kmalloc(sizeof(*event), GFP_KERNEL);

		if (!event) {
			list_for_each_entry_safe(event, tmp,
						 &hello_events, list) {
				list_del(&event->list);
				kfree(event);
			}
			return -ENOMEM;
		}

		event->time = ktime_get();
		list_add_tail(&event->list, &hello_events);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_event *event;
	struct hello_event *tmp;

	list_for_each_entry_safe(event, tmp, &hello_events, list) {
		pr_info("Hello event time: %lld ns\n",
			(long long)ktime_to_ns(event->time));
		list_del(&event->list);
		kfree(event);
	}

	pr_info("Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Hello World module with parameters and timestamps");

