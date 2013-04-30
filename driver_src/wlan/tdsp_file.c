#include "tdsp_file.h"

void* tdsp_openfile(const char* filename)
{
	return ((void*)filp_open(filename, O_RDONLY, 0));
}

void tdsp_closefile(void* file_p)
{
	filp_close((struct file *)file_p, NULL);
}

int tdsp_open_success(void* file_p)
{
	return (file_p == NULL || IS_ERR((struct file *)file_p));
}

unsigned long tdsp_readfile(void* file_p, void* buffer, unsigned long buff_len, unsigned long offset)
{
	mm_segment_t old_fs;
	loff_t pos;
	unsigned long len;
	struct file *filp = (struct file *)file_p;
	pos = (loff_t)offset;
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	len = (filp->f_op->read(filp, (char*)buffer, buff_len, &pos));
	set_fs(old_fs);
	return len;
}
