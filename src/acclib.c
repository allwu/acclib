#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <math.h>
#include <fcntl.h>

#include "acclib.h"

#define reg_write(base, offset, val) 	*((unsigned int*)((base)+(offset))) = (val)
#define reg_read(base, offset) 			*((unsigned int*)((base)+(offset)))

static int verbose = 1;

int acclib_alloc(acclib_buffer *handle, size_t size) {

	int acc_dev_id = open("/dev/ACC", O_RDWR);
	if (acc_dev_id == 0) {
		if (verbose) printf("Cannot open accelerator\n");
		return -1;	
	}

	int args[6];
	args[0] = MSG_ALLOC;
	args[1] = size;

	int status;

	status = read(acc_dev_id, args, 0);
	if (status != 0) {
		if (verbose) printf("Allocation failed\n");
		return status;
	}
	handle->size = size;
	handle->virt_addr = (void*) args[2];
	handle->phys_addr = (void*) args[3];
	handle->handle = (void*) args[4];

	return 0;
}

int acclib_bufwrite(acclib_buffer *handle, void* src, size_t length) {

	int args[4];
	args[0] = MSG_BUFWRITE;
	args[1] = length;
	args[2] = (unsigned int)handle->virt_addr;
	args[3] = (unsigned int)src;

	int acc_dev_id = open("/dev/ACC", O_RDWR);
	int status = read(acc_dev_id, args, 0);
  close(acc_dev_id);

	return status;
}

int acclib_bufread(acclib_buffer *handle, void* dst, size_t length) {

	int args[4];
	args[0] = MSG_BUFREAD;
	args[1] = length;
	args[2] = (unsigned int)dst;
	args[3] = (unsigned int)handle->virt_addr;

	int acc_dev_id = open("/dev/ACC", O_RDWR);
	int status = read(acc_dev_id, args, 0);
  close(acc_dev_id);

	return status;
}
int acclib_free(acclib_buffer *handle) {

	int args[4];
	args[0] = MSG_FREE;
	args[1] = handle->size;
	args[2] = (unsigned int)handle->handle;
	args[3] = (unsigned int)handle->virt_addr;

	int acc_dev_id = open("/dev/ACC", O_RDWR);
	int status = read(acc_dev_id, args, 0);
  close(acc_dev_id);

	//if (status != 0) {
	//	if (verbose) printf("De-allocation failed\n");
	//}
	return status;
}

int acclib_create_kernel(acclib_kernel *kernel, unsigned int addr, int argc) {
	if (!kernel) {
		if (verbose) printf("Invalid kernel ptr\n");
		return -1;
	}
	unsigned page_addr, page_offset;
	unsigned page_size = sysconf(_SC_PAGESIZE);

	int fd = open("/dev/mem", O_RDWR);
	if (fd < 1) {
		if (verbose) printf("Cannot open /dev/mem for read/write. \n");
		return -1;
	}
	page_addr = (addr & (~(page_size-1)));
	page_offset = addr - page_addr;

	void* ptr = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);

	if (!ptr) {
		if (verbose) printf("Cannot open map accelerator to userspace. \n");
		return -1;
	}

	kernel->phys_addr = addr;
	kernel->user_addr = (unsigned int)ptr+page_offset;
	kernel->mmap_ptr  = ptr;
	kernel->argc = argc;

	return 0;
} 

int acclib_setargs(acclib_kernel *kernel, int index,  unsigned int value) {
	
	if (!kernel) {
		if (verbose) printf("Invalid kernel\n");
		return -1;
	}
  // changed because of new vivado_hls version
	unsigned int offset = 0x10;
  // old vivado_hls version
	//unsigned int offset = 0x14;
	if (index < 0 || index >= kernel->argc) {
		if (verbose) printf("Invalid Index\n");
		return -2;
	}
	offset += 0x8 * index;
	
	//kernel->argv[index] = value;
	reg_write(kernel->user_addr, offset, value);

	return 0;
}

// legacy API for blocking accelerator execution
int acclib_execute_kernel(acclib_kernel *kernel) {
	acclib_start_kernel(kernel);
	return acclib_wait_kernel(kernel);
}

void acclib_start_kernel(acclib_kernel *kernel) {
	if (!kernel) {
		if (verbose) printf("Invalid kernel\n");
		return ;
	}
	reg_write(kernel->user_addr, 0, 1);
}

int acclib_wait_kernel(acclib_kernel *kernel) {
	if (!kernel) {
		if (verbose) printf("Invalid kernel\n");
		return -1;
	}
	while ((reg_read(kernel->user_addr, 0) & 0x2) == 0) {
		//usleep(10);
	}

	if (kernel -> is_ret) {
		printf("return value is in %x\n", 0x18+(kernel->argc-1)*0x8);
		return reg_read(kernel->user_addr, 0x18+(kernel->argc-1)*0x8); // return val
	}
	else {
		return 0;
	}
}

int acclib_release_kernel(acclib_kernel *kernel) {
	if (!kernel) {
		if (verbose) printf("Invalid kernel\n");
		return -1;
	}
	unsigned page_size = sysconf(_SC_PAGESIZE);
	munmap(kernel->mmap_ptr, page_size);
	
	return 0;
}
