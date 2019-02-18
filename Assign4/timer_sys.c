/**************************************************************************
*						INCLUDES
**************************************************************************/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

/**************************************************************************
*						Module Information
**************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steve Antony");
MODULE_DESCRIPTION("Module for timer");
MODULE_VERSION("0.01");

/**************************************************************************
*						Local Variables
**************************************************************************/
static struct timer_list my_timer;
static int delay_ms = 500;
static char *name = "default";

/**************************************************************************
*						Module Parameters
**************************************************************************/
module_param(name, charp, 0644);
module_param(delay_ms, int, S_IRUSR | S_IRGRP| S_IROTH| S_IWUSR);



/* Interrupt handler*/
void my_timer_handler(unsigned long data)
{
	static int count = 0;

	count++;
	printk(KERN_ALERT "Interrupt Trigger\n%s\nTrigger count %d\n",name,count);

	//reactivating the interrupt trigger
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(delay_ms));

}

/**************************************************************************
*						 Init Module
**************************************************************************/

static int lkm_example_init(void)
{

	printk(KERN_ALERT "Initiated timer!\n");

	/*
	1000 msec = 100jiffies
	500ms = 50 jiffies
	*/

	my_timer.expires = jiffies + msecs_to_jiffies(delay_ms);
	my_timer.function = &my_timer_handler;
	my_timer.data = 0;
	init_timer(&my_timer);

	/*activating the timer*/
	add_timer(&my_timer);

	return 0;
}

/**************************************************************************
*						 Exit Module
**************************************************************************/
static void lkm_example_exit(void)
{
	printk(KERN_ALERT "Destroyed timer!\n");
	del_timer(&my_timer);

}


module_init(lkm_example_init);
module_exit(lkm_example_exit);




