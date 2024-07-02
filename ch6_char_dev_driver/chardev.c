#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>  
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include <asm/errno.h>

static int device_open(struct inode *, struct file *); //proto type - 일반적으로 .h파일에 들어감.
static int device_release(struct inode *, struct file *); 
static ssize_t device_read(struct file *, char __user*, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user*, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"    //  /proc/devices에 "chardev"라고 장치 이름이 뜬다.
#define BUF_LEN 80    

static int major; // 장치 드라이버에 할당된 주번호

enum {
 CDEV_NOT_USED = 0,
 CDEV_EXCLUSIVE_OPEN = 1,
};

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUF_LEN+1];

static struct class *cls;

static struct file_operations chardev_fops = {
 .read = device_read,
 .write = device_write,
 .open = device_open,
 .release = device_release,
};

static int __init chardev_init(void)
{
 major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
 if(major<0)
 {  
	 pr_alert("char device 등록 실패 주번호 :%d\n", major);
 	return major;
 }
 pr_info("major number 할당 :%d\n", major);
 cls = class_create( DEVICE_NAME);
 device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

 pr_info("Device created on /dev/%s\n", DEVICE_NAME);

 return SUCCESS;
}

static void __exit chardev_exit(void){
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);

	unregister_chrdev(major, DEVICE_NAME);
}

//아래 함수는 프로세스가 장치 파일을 열려고 할 때 호출됨.
//ex -  sudo cat /dev/chardev

static int device_open(struct inode *inode , struct file *file)
{
	static int counter = 0;
	if(atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
		return -EBUSY;

	sprintf(msg, "I already told you %d times Hello world!!!\n", counter++);
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

//아래 함수는 프로세스가 장치 파일을 닫을 때 호출됨.

static int device_release(struct inode * inode, struct file * file)
{
	atomic_set(&already_open, CDEV_NOT_USED);

	module_put(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_read(struct file *flip, char __user *buffer, size_t length, loff_t *offset)
{
	int bytes_read = 0; //실제 버퍼에 기록된 바이트 수
	const char * msg_ptr = msg;

	if(!*(msg_ptr+*offset)){ 
		*offset = 0;
		return 0;
	}

	msg_ptr += *offset;

	while(length && *msg_ptr){
		put_user(*(msg_ptr++), buffer++);
		length--;
		bytes_read++;
	}

	offset += bytes_read;

	return bytes_read;
}

static ssize_t device_write(struct file *flip, const char __user *buff, size_t len, loff_t *off)
{
	pr_alert("Sorry, this operation is not supported. \n");
	return -EINVAL;
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
