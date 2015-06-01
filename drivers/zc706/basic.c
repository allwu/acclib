#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/semaphore.h>
#include <linux/highmem.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>  
#include <asm/page.h>  
#include <asm/signal.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/time.h>

#include "basic.h"
#include "protocol.h"

MODULE_LICENSE("Dual BSD/GPL");
static    DEFINE_SEMAPHORE(my_mutex);

int acc_major = 1027;

#define SUCCESS 0
#define CRIT_ERR -1 

struct file_operations ACC_Intf = {
            read:			ACC_read,
            write:			ACC_write,
            unlocked_ioctl: ACC_ioctl,
            open:			ACC_open,
            release:		ACC_release
};

int ACC_read(
		struct file *filp,
		int* argv, 
		size_t argc, 
		loff_t *f_pos) 
{

	int msg_type = argv[0];
	int length;
	dma_addr_t handle;
	void* src_ptr;
	void* dst_ptr;
	switch (msg_type) {
		case MSG_ALLOC:
			length = argv[1];
			if (length > 0) {
				void* dma_ptr = dma_alloc_coherent(NULL, length, &handle, GFP_KERNEL);
				if (!dma_ptr) {
					printk("[ACC Err] Cannot allocate buffers\n");
					return -1;
				}
				printk("[ACC Info] Allocated DMA buffers: virt_ptr=%x \n", (unsigned int)dma_ptr);

				void* phys_ptr = virt_to_phys(dma_ptr);

				argv[2] = (unsigned int)dma_ptr;
				argv[3] = (unsigned int)phys_ptr;
				argv[4] = (unsigned int)handle;
			}
			else {
				printk("[ACC Err] Invalid length.\n");
				return -1;
			}
			break;
		case MSG_FREE:
			length = argv[1];
			dma_addr_t handle = (dma_addr_t)argv[2];
			void*	   dma_ptr = (void*)argv[3];
			dma_free_coherent(NULL, length, dma_ptr, handle);
			break;
		case MSG_BUFREAD:
			length = argv[1];
			dst_ptr = (void*)argv[2];
			src_ptr = (void*)argv[3];
			copy_to_user(dst_ptr, src_ptr, length);
			break;
		case MSG_BUFWRITE:
			length = argv[1];
			dst_ptr = (void*)argv[2];
			src_ptr = (void*)argv[3];

			copy_from_user(dst_ptr, src_ptr, length);
			break;
		default:
			printk("[ACC Err] Unrecognized Request.\n");
			return -1;
	}

    return SUCCESS;
}

int ACC_write(struct inode *inode, struct file *filp) {
    return SUCCESS;
}
int ACC_open(struct inode *inode, struct file *filp) {
    return SUCCESS;
}
int ACC_release(struct inode *inode, struct file *filp) {
    return SUCCESS;
}

int ACC_ioctl(struct file* flip, unsigned int cmd) {
    down(&my_mutex);
    //	flag=cmd;

    return(SUCCESS);
}

static int ACC_init(void)
{
	int result;
	result = register_chrdev(acc_major, "ACC", &ACC_Intf);

	if (result < 0) {
		printk("[ACC Error] cannot obtain major number %d\n", acc_major);
		return result;
	}

    printk(KERN_INFO "[ACC Info] Hello, amazing world from ACC!\n");

    return SUCCESS;
}

static void ACC_exit(void)
{
	unregister_chrdev(acc_major, "ACC");
    printk(KERN_INFO "[ACC Info] Goodbye, cruel world from ACC\n");
}

module_init(ACC_init);
module_exit(ACC_exit);

