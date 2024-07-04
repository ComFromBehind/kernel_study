#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define HAVE_PROC_OPS
#endif

#define procfs_name "iamjjangjjangman"

static struct proc_dir_entry *our_proc_file;

static ssize_t procfile_read(struct file * file_pointer, char __user *buffer, // /proc/iamjjangjjangman 파일을 읽을 때 값과 버퍼반환 
		size_t buffer_length, loff_t *offset)
{
	char s[13] = "youcandoit!\n";
	int len = sizeof(s);
	ssize_t ret = len;

	if(*offset >= len || copy_to_user(buffer, s, len)) {
		pr_info("copy_to_user failed \n");
		ret = 0;
	}
	else {
		pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
		*offset += len;
	}

	return ret;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
	.proc_read = procfile_read,
};

#else
static const struct file_operations proc_file_fops = {
	.read = procfile_read,
};
#endif

static int __init procfs1_init(void) //   /proc/iamjjang.. 파일을 만듬. 
{
	our_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops);
	if(NULL == our_proc_file){
		proc_remove(our_proc_file);
		pr_alert("Error :Could not initialize /proc/%s\n", procfs_name);
		return -ENOMEM;
	}

	pr_info("/proc/%s created\n", procfs_name);
	return 0;
}

static void __exit procfs1_exit(void)  //  /proc/imajjang.. 파일을 삭제
{
	proc_remove(our_proc_file);
	pr_info("/proc/%s removed\n", procfs_name);
}

module_init(procfs1_init);
module_exit(procfs1_exit);

MODULE_LICENSE("GPL");

/*
 /proc/iamjjangjjangman 파일을 읽을 떄마다 procfile_read 함수가 호출됨.
 buffer의 내용은 buffer를 읽은 application에 반환됨. (cat 명령어같은 것)
 
 오프셋은 파일의 현재 위치. 함수의 반환값이 null이 아니면, 다시 호출. 0을 리턴할때까지 읽기 함수 무한 반복


 */
