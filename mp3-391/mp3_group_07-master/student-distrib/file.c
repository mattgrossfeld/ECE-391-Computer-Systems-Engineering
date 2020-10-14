#include "file.h"

/*
 * read_dentry_by_name
 *   DESCRIPTION: Find directory entry by name
 *   INPUTS: fname -- name of file
 *           dentry -- dentry_t struct to write into
 *			 boot_blk -- boot block with dentry information
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: copies the data of dentry with name fname into
 *				   the given dentry_t struct
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
	int i;

	if (strlen((int8_t*)fname) > 32) {
		printf("File name over 32 characters!\n");
		return -1;
	}

	for (i = 0;i < boot_blk->num_dir_entries;i++) {

		/* If given name matched a directory name, copy data into dentry */
		if (0 == strncmp((int8_t*)fname, boot_blk->dir_entries[i].file_name, MAX_STR_LEN)) {

 			strncpy(dentry->file_name, boot_blk->dir_entries[i].file_name, MAX_STR_LEN);
			dentry->file_type = boot_blk->dir_entries[i].file_type;
			dentry->inode_num = boot_blk->dir_entries[i].inode_num;

			return 0;
		}
	}

	return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: Find directory entry by index
 *   INPUTS: index -- index of file
 *           dentry -- dentry_t struct to write into
 *			 boot_blk -- boot block with dentry information
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: copies the data of dentry wtih index # index into
 *				   the given dentry_t struct
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

	if (index < 0 || index > boot_blk->num_dir_entries) return -1;

	/* Copy directory info to given dentry_t struct */
	strncpy(dentry->file_name, boot_blk->dir_entries[index].file_name, MAX_STR_LEN);
	dentry->file_type = boot_blk->dir_entries[index].file_type;
	dentry->inode_num = boot_blk->dir_entries[index].inode_num;

	return 0;
}

/*
 * read_data
 *   DESCRIPTION: Copies data of a file to given buf
 *   INPUTS: inode -- inode number of the file
 *           offset -- number of bytes of the file to offset
 *					   before copying into buf
 *			 buf -- buffer to copy file data into
 *			 length -- amount of bytes of file data to copy
 *					   into the buffer
 *			 boot_blk -- boot block with dentry information
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: copies the data of file into buffer
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	if (inode < 0 || inode >= boot_blk->num_inodes) {
		printf("Inode out of range!\n");
		return -1; //check if inode is within range
	}

	inode_t* inode_blk = (inode_t *) ((unsigned int)boot_blk + BLOCK_SIZE + inode*BLOCK_SIZE); //inode blocks start after boot block

	/* Initialize Variables */
	int file_length = inode_blk->length;
	int i = 0;
	int offset_byte = 0; //how many bytes to offset
	int offset_block = offset / BLOCK_SIZE; //check if offset is larger than block size
//	printf("%d\n", offset_block);
//	printf("%d\n", file_length);
	int block_num = offset_block;

	//Check for bad data block number
	if (inode_blk->data_block_num[block_num] > boot_blk->num_data_blocks || inode_blk->data_block_num[block_num] < 0) {
		printf("Bad data block number!\n");
		return -1;
	}

	/* Set data block struct to point to correct address */
	data_block* data_blk = (data_block*) ((unsigned int)boot_blk + BLOCK_SIZE + boot_blk->num_inodes*BLOCK_SIZE + inode_blk->data_block_num[block_num]*BLOCK_SIZE);

	/* Update if offset is greater than 1 block (4kB) */
	if (offset_block > 0) {
		offset_byte = offset % BLOCK_SIZE;
		//data_blk += offset % BLOCK_SIZE;
	} else {
		offset_byte = offset;
		//data_blk += offset;
	}

	int length_block = length / BLOCK_SIZE;

	/* Copy bytes to buffer until length amount of bytes is copied or reached end of file */
	int k;
	block_num++;

	for (k = 0; k < length_block+1; k++, block_num++) {
		int j;
		for (j = 0; j < (BLOCK_SIZE - offset_byte); i++, j++) {
			if (i == length || i == file_length - offset) break;
			//printf("%c", data_blk->data[offset_byte+j]);
			buf[i] = data_blk->data[offset_byte+j];
		}
		offset_byte = 0;
		if (inode_blk->data_block_num[block_num] > boot_blk->num_data_blocks || inode_blk->data_block_num[block_num] < 0) {
			printf("Bad data block number!\n");
			return -1;
		}
		
		/* Update data block address if file data spans through multiple blocks */
		data_blk = (data_block*) ((unsigned int)boot_blk + BLOCK_SIZE + boot_blk->num_inodes*BLOCK_SIZE + inode_blk->data_block_num[block_num]*BLOCK_SIZE);
	}
	return i;
}

/* read_directory
 * 
 * Prints out the names of all files
 * Inputs: boot_blk -- pointer to boot_block struct 
 * Outputs: names of all files
 * Side Effects: None
 */
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes) {

	//get file_position from fd (index of dentry) and increment file_position
	pcb_t* pcb = get_this_pcb();//(pcb_t*) ((unsigned int)tss.esp0 + EIGHT_KB);
	int index = pcb->fd_array[fd].file_pos;
	pcb->fd_array[fd].file_pos++;

	dentry_t temp;

	int8_t* str[nbytes+1];

    if (read_dentry_by_index(index, &temp) == -1) return 0;

    strncpy((int8_t*)str, (int8_t*)temp.file_name, (uint32_t)nbytes);

    str[nbytes] = '\0';

    //copy name to buf
    memcpy((char*)buf, (char*)str, nbytes);

    int32_t length = strlen((int8_t*) str);

    if (length > nbytes) length = nbytes;
	
	return length;
}

/* read_file
 * 
 * Prints out the contents of a file
 * Inputs: file descriptor, buffer to read, number of bytes
 * Outputs: number of bytes read
 * Side Effects: None
 */
int32_t read_file(int32_t fd, void* buf, int32_t nbytes) {

	//get inode from fd
	pcb_t* pcb = get_this_pcb(); //(pcb_t*) ((unsigned int)tss.esp0 + EIGHT_KB);
	uint32_t inode = pcb->fd_array[fd].inode;

	//check if file_position is at or past the end of file (file_position >= inode.length)
	//if it is return 0
	inode_t* inode_blk = (inode_t *) ((unsigned int)boot_blk + BLOCK_SIZE + inode*BLOCK_SIZE);
	if (pcb->fd_array[fd].file_pos >= inode_blk->length) return 0;

	//set file_pos and update file_array[fd].file_position
	uint32_t file_position = pcb->fd_array[fd].file_pos;
	pcb->fd_array[fd].file_pos = pcb->fd_array[fd].file_pos + nbytes;

	return read_data(inode, file_position, buf, nbytes);
}

/* open_directory
 * 
 * Opens a directory
 * Inputs: file name
 * Outputs: fd on success
 * Side Effects: None
 */
int32_t open_directory(const uint8_t* filename) {
	if (read_dentry_by_name(filename, &global_dentry) == -1) return -1;

	//allocate unused file descriptor (if no descriptors are free return -1)
	pcb_t* pcb = get_this_pcb(); //(pcb_t*) ((unsigned int)tss.esp0 + EIGHT_KB);
	if (pcb->num_files == 8) return -1;

	int i, fd;
	for (i = 0; i < MAX_FILES; i++) {
		if (pcb->fd_array[i].is_open == 0) {
			fd = i;
			break;
		}
	}

	return fd;
}

/* open_file
 * 
 * Opens a file
 * Inputs: file name
 * Outputs: fd on success
 * Side Effects: None
 */
int32_t open_file(const uint8_t* filename) {
	if (read_dentry_by_name(filename, &global_dentry) == -1) return -1;

	//allocate unused file descriptor (if no descriptors are free return -1)
	pcb_t* pcb = get_this_pcb();//(pcb_t*) ((unsigned int)tss.esp0 + EIGHT_KB);
	if (pcb->num_files == 8) return -1;

	int i, fd;
	for (i = 0; i < MAX_FILES; i++) {
		if (pcb->fd_array[i].is_open == 0) {
			fd = i;
			break;
		}
	}

	return fd;
}

/* close_directory
 * 
 * Closes a directory
 * Inputs: file descriptor
 * Outputs: 0 on success
 * Side Effects: None
 */
int32_t close_directory(int32_t fd) {
	if (fd == 0 || fd == 1) return -1;

	//close file descriptor and make it available for open
	pcb_t* pcb = get_this_pcb();//(pcb_t*) ((unsigned int)tss.esp0 + EIGHT_KB);

	pcb->num_files--;
	pcb->fd_array[fd].is_open = 0;

	return 0;
}

/* close_file
 * 
 * Closes a file
 * Inputs: file descriptor
 * Outputs: 0 on success
 * Side Effects: None
 */
int32_t close_file(int32_t fd) {
	if (fd == 0 || fd == 1) return -1;

	//close file descriptor and make it available for open
	pcb_t* pcb = get_this_pcb();//(pcb_t*) ((unsigned int)tss.esp0 + EIGHT_KB);

	pcb->num_files--;
	pcb->fd_array[fd].is_open = 0;

	return 0;
}

int32_t write_directory(int32_t fd, const void* buf, int32_t nbytes) {
	//read only so return -1
	return -1;
}

/* write_file
 * 
 * Prints out the contents of a file
 * Inputs: file descriptor, buffer to write, number of bytes
 * Outputs: number of bytes read
 * Side Effects: None
 */
int32_t write_file(int32_t fd, const void* buf, int32_t nbytes) {
	//read only so return -1
	return -1;
}
