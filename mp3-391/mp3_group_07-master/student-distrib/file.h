#ifndef FILE_H
#define FILE_H

#include "lib.h"
#include "systemcall.h"
#include "x86_desc.h"

#define BYTE_SIZE	8
#define MAX_STR_LEN 32
#define MAX_DIR_NUM 63
#define BLOCK_SIZE 	4096 //4kB

/* Define structs of the file system */

/* Directory entry (see figure in Appendix A) */
typedef struct dentry_t {
	char file_name[32];
	unsigned int file_type;
	unsigned int inode_num;
	unsigned char reserved[24];
} dentry_t;

/* Boot Block (see figure in Appendix A) */
typedef struct boot_block {
	unsigned int num_dir_entries;
	unsigned int num_inodes;
	unsigned int num_data_blocks;
	unsigned char reserved[52];
	dentry_t dir_entries[63];
} boot_block;

/* Inode (see figure in Appendix A) */
typedef struct inode_t {
	unsigned int length;
	int data_block_num[1023];
} inode_t;

/* Data Block (see figure in Appendix A) */
typedef struct data_block {
	unsigned char data[BLOCK_SIZE];
} data_block;

/* Global Variables */
dentry_t global_dentry;
boot_block* boot_blk;

/* Function headers */

/* copy dentry (searched by name) information to given struct */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);

/* copy dentry (searched by index) information to given struct */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);

/* copy length amount of data (given by inode and offset) into buf */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/*  */
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes);

/*  */
int32_t read_file(int32_t fd, void* buf, int32_t nbytes);

/*  */
int32_t open_directory(const uint8_t* filename);

/*  */
int32_t open_file(const uint8_t* filename);

/*  */
int32_t close_directory(int32_t fd);

/*  */
int32_t close_file(int32_t fd);

/*  */
int32_t write_directory(int32_t fd, const void* buf, int32_t nbytes);

/*  */
int32_t write_file(int32_t fd, const void* buf, int32_t nbytes);

#endif
