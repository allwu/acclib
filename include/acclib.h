#ifndef ACCLIB_H
#define ACCLIB_H

#define MSG_ALLOC		0
#define MSG_FREE		1
#define MSG_BUFWRITE	2
#define MSG_BUFREAD		3

typedef struct BUFFER{
	size_t size;
	void*  host_ptr;
	void*  virt_addr;
	void*  phys_addr;
	void*  handle;
} acclib_buffer;

typedef struct KERNEL {
	unsigned int phys_addr;	// physical base addr of IP
	unsigned int user_addr;	// base addr of IP in user space
	void* mmap_ptr;
	int argc;
	unsigned int* argv;
	int is_ret;
} acclib_kernel;

int  acclib_alloc(acclib_buffer *, size_t);
int  acclib_bufwrite(acclib_buffer *, void*, size_t);
int  acclib_bufread(acclib_buffer *, void*, size_t);
int  acclib_free(acclib_buffer *);
int  acclib_create_kernel(acclib_kernel *, unsigned int, int);
int  acclib_setargs(acclib_kernel *, int, unsigned int);
int  acclib_execute_kernel(acclib_kernel *);
void acclib_start_kernel(acclib_kernel *);
int  acclib_wait_kernel(acclib_kernel *);
int  acclib_release_kernel(acclib_kernel *);
#endif
