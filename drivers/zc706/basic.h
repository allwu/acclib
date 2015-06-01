int ACC_read(struct file*, int*, size_t, loff_t*);
int ACC_write(struct inode *inode, struct file *filp);
int ACC_open(struct inode *inode, struct file *filp);
int ACC_release(struct inode *inode, struct file *filp);
int ACC_ioctl(struct file* flip, unsigned int cmd);
