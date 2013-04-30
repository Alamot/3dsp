#ifndef __TDSP_FILE_H__
#define __TDSP_FILE_H__
#include <linux/fs.h>
#include <linux/uaccess.h>

void* tdsp_openfile(const char* filename);
void tdsp_closefile(void* file_p);
int tdsp_open_success(void* file_p);
unsigned long tdsp_readfile(void* file_p, void* buffer, unsigned long buff_len, unsigned long offset);

#endif
