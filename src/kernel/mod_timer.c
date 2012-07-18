/*
	This file is part of the linuxapi project.
	Copyright (C) 2011, 2012 Mark Veltzer <mark.veltzer@gmail.com>

	The linuxapi package is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The linuxapi package is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with the GNU C Library; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA.
*/

//#define DEBUG
#include<linux/module.h> // for MODULE_*, module_*
#include<linux/timer.h> // for setup_timer, mod_timer, del_timer

//#define DO_DEBUG
#include"kernel_helper.h" // our own helper

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Showing how IS_ERR_VALUE works");

/*
* This module shows how to use timers (non-hi-res) in the linux kernel.
*
* References:
* http://www.ibm.com/developerworks/linux/library/l-timers-list/
*/

static struct timer_list my_timer;

static void my_timer_callback(unsigned long data) {
	printk("my_timer_callback called (%ld).\n",jiffies);
}

static int __init mod_init(void) {
	int ret;
	printk("Timer module installing\n");
	setup_timer(&my_timer,my_timer_callback,0);
	printk("Starting timer to fire in 200ms (%ld)\n",jiffies);
	ret=mod_timer(&my_timer,jiffies+msecs_to_jiffies(200));
	if(ret) {
		printk("Error in mod_timer\n");
		return ret;
	}
	return 0;
}

static void __exit mod_exit(void) {
	int ret;
	ret=del_timer(&my_timer);
	if(ret) {
		printk("The timer is still in use...\n");
	}
	printk("Timer module uninstalling\n");
}

module_init(mod_init);
module_exit(mod_exit);
