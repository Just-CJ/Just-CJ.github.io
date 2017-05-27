#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kthread.h>  
#include <linux/gpio.h>

#include <linux/init.h>
#include <linux/moduleparam.h>
#include "myfont.h"

#define DEVICE_NAME                 "ledmatrix"  // device name to be created and registered


static unsigned int gpio_col[8] = {27, 19, 26, 23, 6, 4, 10, 9};
static unsigned int gpio_row[8] = {24, 22, 11, 17, 16, 5, 20, 13};

struct ledmatrix {
	struct cdev cdev;               /* The cdev structure */
	char name[20];                  /* Name of device*/
	
} *ledmatrix_devp;

static dev_t ledmatrix_number;      /* Allotted device number */
struct class *ledmatrix_class;          /* Tie with the device model */
static struct device *ledmatrix_device;
//static unsigned short eeprom_cur_pgoff;

static char *user_name = "Dear Just_CJ";

static char display_data = 'A';
//extern unsigned char Font[][8];
static int read_flag = 0;

static struct task_struct *display_tsk;  

// module_param(user_name,charp,0000);	//to get parameter from load.sh script to greet the user

static void init_gpio(void *data)
{
	unsigned int i;
	int ret1;
	for (i=0; i<8; i++){
		ret1 = gpio_request_one(gpio_row[i], GPIOF_OUT_INIT_LOW, "row");
		if (ret1 < 0){
			printk("GPIO %d request failed!\n", gpio_row[i]);
		}	
	}

	for (i=0; i<8; i++){
		ret1 = gpio_request_one(gpio_col[i], GPIOF_OUT_INIT_LOW, "col");
		if (ret1 < 0){
			printk("GPIO %d request failed!\n", gpio_col[i]);
		}	
	}
}


 static void reset_gpio(void *data)
 {
 	unsigned int i;
	int ret1;
	for (i=0; i<8; i++){
		ret1 = gpio_direction_input(gpio_row[i]);
		if (ret1 == 0){
			gpio_free(gpio_row[i]);
		}else
			printk("GPIO %d set input failed!\n", gpio_row[i]);
	}

	for (i=0; i<8; i++){
		ret1 = gpio_direction_input(gpio_col[i]);
		if (ret1 == 0){
			gpio_free(gpio_col[i]);
		}else
			printk("GPIO %d set input failed!\n", gpio_col[i]);
	}
 }


static void set_col(unsigned char c)
{
	unsigned int i;
	for (i=0; i<8; i++){
		gpio_set_value(gpio_col[i], !((c>>(7-i))&1));
	}
}

/*
* Display function
*/
static int display_thread(void *data)
{
	unsigned int i;
	init_gpio(NULL);

	while(!kthread_should_stop()){
		//printk("In display thread!\n");
		for (i=0; i<8; i++){
			set_col(0); // all to 0 for row
			gpio_set_value(gpio_row[i==0?7:(i-1)], 0); // set pre row to 0
			gpio_set_value(gpio_row[i], 1); //set cur row to 1
			//set_col(0xff);
			set_col(Font[display_data-0x20][i]);
			//msleep(1);
			udelay(1000);
		}

		//msleep(1000);  
	}
	reset_gpio(NULL);
	return 1;
}


/*
* Open ledmatrix
*/
int ledmatrix_open(struct inode *inode, struct file *file)
{
	struct ledmatrix *ledmatrix_devp;
//	printk("\nopening\n");

	/* Get the per-device structure that contains this cdev */
	ledmatrix_devp = container_of(inode->i_cdev, struct ledmatrix, cdev);


	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = ledmatrix_devp;
	printk("\n%s is openning \n", ledmatrix_devp->name);
	return 0;
}

/*
 * Release ledmatrix 
 */
int ledmatrix_close(struct inode *inode, struct file *file)
{
	struct ledmatrix *ledmatrix_devp = file->private_data;

	printk("\n%s is closing\n", ledmatrix_devp->name);
	read_flag = 0;
	
	return 0;
}



ssize_t ledmatrix_read(struct file *file, char *buf,
           size_t count, loff_t *ppos)
{
	char t;
	int ret;
	if(read_flag==2) return 0;
	else if(read_flag==1)
		t='\n';
	else
		t=display_data;
	
	if(read_flag == 0)
		printk("copy to user: %c \n", t);
	else
		printk("copy to user: \\n \n");

	read_flag++;

	ret =  copy_to_user(buf, &t, sizeof(t));
	if (ret == 0){
		printk("copy to user success!\n");
		return 1;
	}else{
		printk("copy to user failed!\n");
		return 0;
	}
}




ssize_t ledmatrix_write(struct file *file, const char *buf,
           size_t count, loff_t *ppos)
{
	int ret;
	char tmp[2] = {0};
	ret = copy_from_user(tmp, buf, 2);
	display_data = tmp[0];
	return count;
}


loff_t ledmatrix_lseek(struct file * file, loff_t offset, int whence){

	return 0;
}



/* File operations structure. Defined in linux/fs.h */
static struct file_operations ledmatrix_fops = {
    .owner		= THIS_MODULE,           /* Owner */
    .open		= ledmatrix_open,        /* Open method */
    .release	= ledmatrix_close,     /* Close method */
    .write		= ledmatrix_write,       /* Write method */
    .read		= ledmatrix_read,        /* Read method */
    .llseek		= ledmatrix_lseek,		  /* Lseek method */
};



static int __init ledmatrix_init(void) 
{ 

           	int ret;
		    int time_since_boot;

		    /* Request dynamic allocation of a device major number */
			if (alloc_chrdev_region(&ledmatrix_number, 0, 1, DEVICE_NAME) < 0) {
					printk(KERN_DEBUG "Can't register device\n"); return -1;
			}

			/* Populate sysfs entries */
			ledmatrix_class = class_create(THIS_MODULE, DEVICE_NAME);

			/* Allocate memory for the per-device structure */
			ledmatrix_devp = kmalloc(sizeof(struct ledmatrix), GFP_KERNEL);
				
			if (!ledmatrix_devp) {
				printk("Bad Kmalloc\n"); return -ENOMEM;
			}

			/* Request I/O region */
			sprintf(ledmatrix_devp->name, DEVICE_NAME);

			/* Connect the file operations with the cdev */
			cdev_init(&ledmatrix_devp->cdev, &ledmatrix_fops);
			ledmatrix_devp->cdev.owner = THIS_MODULE;

			/* Connect the major/minor number to the cdev */
			ret = cdev_add(&ledmatrix_devp->cdev, MKDEV(MAJOR(ledmatrix_number), 0), 1);

			if (ret) {
				printk("Bad cdev\n");
				return ret;
			}

			/* Send uevents to udev, so it'll create /dev nodes */
			ledmatrix_device = device_create(ledmatrix_class, NULL, MKDEV(MAJOR(ledmatrix_number), 0), NULL, DEVICE_NAME);	

		     
		    time_since_boot=(jiffies-INITIAL_JIFFIES)/HZ;//since on some systems jiffies is a very huge uninitialized value at boot and saved.
			printk("Hi %s, this machine has been on for %d seconds\n", user_name, time_since_boot);

			
			/* Create Display thread */
			display_tsk = kthread_run(display_thread, NULL, "Display thread");  
			if (IS_ERR(display_tsk)) {  
		        printk(KERN_INFO "create kthread failed!\n");  
		    }  
		    else {  
		        printk(KERN_INFO "create ktrhead ok!\n");  
		    }  

			printk("ledmatrix initialized.\n");// '%s'\n",gmem_devp->in_string);

		    return 0; 
} 
 
static void __exit ledmatrix_exit(void) 
{ 
    
    // kfree(ledmatrix_obj);
	unregister_chrdev_region((ledmatrix_number), 1);

	/* Destroy device */
	device_destroy (ledmatrix_class, MKDEV(MAJOR(ledmatrix_number), 0));
	cdev_del(&ledmatrix_devp->cdev);
	kfree(ledmatrix_devp);
	
	/* Destroy driver_class */
	class_destroy(ledmatrix_class);

	if (!IS_ERR(display_tsk)){  
        kthread_stop(display_tsk);  
        printk(KERN_INFO "Display thread stopped!\n");  
    }  

	printk("ledmatrix removed.\n");
	
} 
 
module_init(ledmatrix_init); 
module_exit(ledmatrix_exit); 
 
MODULE_DESCRIPTION("ledmatrix driver"); 
MODULE_AUTHOR("Just_CJ"); 
MODULE_LICENSE("GPL v2"); 