#include <stdio.h>
#include "ext2_fs.h"
#include "read_ext2.h"
#include <string.h>

// Globals
int fd;
struct ext2_super_block super; // make this global
int DEBUG = 0;
int WRITE_EACH_BLOCK = 1;

void write_iblocks_to_file(struct ext2_inode* node, int out_fp){
	char buffer[block_size];
	__u32 file_size = node->i_size;
	if(file_size == 0){
		return;
	}
	__u32 bytes_written = 0;
	
	for(unsigned int i=0; i<EXT2_N_BLOCKS; i++) {       
		if (i < EXT2_NDIR_BLOCKS) { // direct blocks
			if(node->i_block[i] != 0) { // if not null pointer
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				if(file_size - bytes_written > block_size){ // write either the entire block
					if(DEBUG){printf("a");}
					read(fd, &buffer, block_size);
					write(out_fp, &buffer, block_size);
					bytes_written += block_size;
					if(bytes_written == file_size){
						return;
					}
				}else { // or write a partial block and return
					if(DEBUG){printf("b\n");}
					read(fd, &buffer, file_size - bytes_written);
					write(out_fp, &buffer, file_size - bytes_written);
					//printf("wrote %d bytes to file\n", file_size - bytes_written);
					return;
				}
			}
		}
		else if (i == EXT2_IND_BLOCK) {  // single-indirect block
			if(node->i_block[i] != 0) { // single indirect block contains array of 256 pointers
				__u32 first_indirect_pointers[256]; //cast contents of indirect block to array of block offset values (not byte offset values)
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				read(fd, &first_indirect_pointers, block_size);

				for(int j = 0; j < 256; j++){
					lseek(fd, BLOCK_OFFSET(first_indirect_pointers[j]), SEEK_SET);
					if(file_size - bytes_written > block_size){ // write either the entire block
						if(DEBUG){printf("c");}
						read(fd, &buffer, block_size);
						write(out_fp, &buffer, block_size);
						//fsync(out_fp);
						bytes_written += block_size;
						if(bytes_written == file_size){
							return;
						}
					}else { // or write a partial block and return
						if(DEBUG){printf("d\n");}
						read(fd, &buffer, file_size - bytes_written);
						write(out_fp, &buffer, file_size - bytes_written);
						//fsync(out_fp);
						//printf("wrote %d bytes to file\n", file_size - bytes_written);
						return;
					}
				}// j
			}// i (single-indirect)
		}
		else if (i == EXT2_DIND_BLOCK) { // double-indirect block					 
			if(node->i_block[i] != 0) { // first indirect block contains array of 256 pointers
				__u32 first_indirect_pointers[256];
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				read(fd, &first_indirect_pointers, block_size);

				for(int j = 0; j < 256; j++){
					__u32 second_indirect_pointers[256]; 
					lseek(fd, BLOCK_OFFSET(first_indirect_pointers[j]), SEEK_SET);
					read(fd, &second_indirect_pointers, block_size);

					for(int z = 0; z < 256; z++){
						lseek(fd, BLOCK_OFFSET(second_indirect_pointers[z]), SEEK_SET);
						if(file_size - bytes_written > block_size){ // write either the entire block
							if(DEBUG){printf("e");}
							read(fd, &buffer, block_size);
							write(out_fp, &buffer, block_size);
							bytes_written += block_size;
							if(bytes_written == file_size){
								return;
							}
						}else { // or write a partial block and return
							if(DEBUG){printf("f\n");}
							read(fd, &buffer, file_size - bytes_written);
							write(out_fp, &buffer, file_size - bytes_written);
							return;
						}
					}// z
				}// j
			}// i (double-indirect)
		}
		else if (i == EXT2_TIND_BLOCK) { // triple-indirect block
			if(node->i_block[i] != 0){
				printf("ERROR: load_write_buffer() tried to read from triple-indirect block\n");
			}
		}
	}// for
	printf("WARNING: load_write_buffer() did not return inside the for loop.\n");
}// write_iblocks_to_file

void load_write_buffer(char* buffer, struct ext2_inode* node){
	__u32 file_size = node->i_size;
	if(file_size == 0){
		return;
	}
	__u32 bytes_written = 0;

	for(unsigned int i=0; i<EXT2_N_BLOCKS; i++) {       
		if (i < EXT2_NDIR_BLOCKS) { // direct blocks
			if(node->i_block[i] != 0) { // if not null pointer
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				if(file_size - bytes_written > block_size){ // write either the entire block
					if(DEBUG){printf("a");}
					read(fd, buffer + bytes_written, block_size);
					bytes_written += block_size;
					if(bytes_written == file_size){
						return;
					}
				}else { // or write a partial block and return
					if(DEBUG){printf("b");}
					read(fd, buffer  + bytes_written, file_size - bytes_written);
					bytes_written += file_size - bytes_written;
					// printf("wrote %d bytes to file\n", file_size - bytes_written);
					return;
				}
			}
		}
		else if (i == EXT2_IND_BLOCK) {  // single-indirect block
			if(node->i_block[i] != 0) { // single indirect block contains array of 256 pointers
				__u32 first_indirect_pointers[256]; //cast contents of indirect block to array of block offset values (not byte offset values)
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				read(fd, &first_indirect_pointers, block_size);

				for(int j = 0; j < 256; j++){
					lseek(fd, BLOCK_OFFSET(first_indirect_pointers[j]), SEEK_SET);
					if(file_size - bytes_written > block_size){ // write either the entire block
						if(DEBUG){printf("c");}
						read(fd, buffer + bytes_written, block_size);
						bytes_written += block_size;
						if(bytes_written == file_size){
							return;
						}
					}else { // or write a partial block and return
						if(DEBUG){printf("d");}
						read(fd, buffer + bytes_written, file_size - bytes_written);
						bytes_written += file_size - bytes_written;
						// printf("wrote %d bytes to file\n", file_size - bytes_written);
						return;
					}
				}// j
			}// i (single-indirect)
		}
		else if (i == EXT2_DIND_BLOCK) { // double-indirect block					 
			if(node->i_block[i] != 0) { // first indirect block contains array of 256 pointers
				__u32 first_indirect_pointers[256];
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				read(fd, &first_indirect_pointers, block_size);

				for(int j = 0; j < 256; j++){
					__u32 second_indirect_pointers[256]; 
					lseek(fd, BLOCK_OFFSET(first_indirect_pointers[j]), SEEK_SET);
					read(fd, &second_indirect_pointers, block_size);

					for(int z = 0; z < 256; z++){
						lseek(fd, BLOCK_OFFSET(second_indirect_pointers[z]), SEEK_SET);
						if(file_size - bytes_written > block_size){ // write either the entire block
							if(DEBUG){printf("e");}
							read(fd, buffer + bytes_written, block_size);
							bytes_written += block_size;
							if(bytes_written == file_size){
								return;
							}
						}else { // or write a partial block and return
							if(DEBUG){printf("f");}
							read(fd, buffer + bytes_written, file_size - bytes_written);
							bytes_written += file_size - bytes_written;
							return;
						}
					}// z
				}// j
			}// i (double-indirect)
		}
		else if (i == EXT2_TIND_BLOCK) { // triple-indirect block
			if(node->i_block[i] != 0){
				printf("ERROR: load_write_buffer() tried to read from triple-indirect block\n");
			}
		}
	}// for
	printf("WARNING: load_write_buffer() did not return inside the for loop.\n");
}// load_write_buffer

void load_magic_buffer(char* buffer, struct ext2_inode* node) {
	// find the first non-zero block that contains data
	for(unsigned int i=0; i<EXT2_N_BLOCKS; i++) {       
		if (i < EXT2_NDIR_BLOCKS){ /* direct blocks */
			if(node->i_block[i] != 0) { // if not null pointer
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				read(fd, buffer, 4 * sizeof(char));
			}
			break;
		}        
		else if (i == EXT2_IND_BLOCK) {  /* single indirect block */
			if(node->i_block[i] != 0) { // single indirect block contains array of 256 pointers
				__u32 first_indirect_pointers[256]; //cast contents of indirect block to array of block offset values (not byte offset values)
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				read(fd, &first_indirect_pointers, block_size);

				lseek(fd, BLOCK_OFFSET(first_indirect_pointers[0]), SEEK_SET);
				read(fd, buffer, 4 * sizeof(char));
			}
			break;
		}                           
		else if (i == EXT2_DIND_BLOCK){ /* double indirect block */
			if(node->i_block[i] != 0) { // first indirect block contains array of 256 pointers
				__u32 first_indirect_pointers[256]; //cast contents of indirect block to array of block offset values (not byte offset values)
				lseek(fd, BLOCK_OFFSET(node->i_block[i]), SEEK_SET);
				read(fd, &first_indirect_pointers, block_size);

				__u32 second_indirect_pointers[256]; //cast contents of indirect block to array of block offset values (not byte offset values)
				lseek(fd, BLOCK_OFFSET(first_indirect_pointers[0]), SEEK_SET);
				read(fd, &second_indirect_pointers, block_size);

				lseek(fd, BLOCK_OFFSET(second_indirect_pointers[0]), SEEK_SET);
				read(fd, buffer, 4 * sizeof(char));
			}
			break;
		}                            	
		else if (i == EXT2_TIND_BLOCK) { /* triple indirect block */
			printf("Warning! i-node has triple indirect block\n");
			break;
		}                            
	}	
}

int check_if_jpg(char* buffer) {
	int is_jpg = 0;
	if (buffer[0] == (char)0xff && buffer[1] == (char)0xd8 && buffer[2] == (char)0xff &&
    	(buffer[3] == (char)0xe0 || buffer[3] == (char)0xe1 || buffer[3] == (char)0xe8)) 
	{
    	is_jpg = 1;
	}	
	return is_jpg;
}

int calc_num_block_groups() {
	int num_block_groups = (super.s_blocks_count - 1) / super.s_blocks_per_group; // -1 to remove boot block which is included in s_blocks_count
	if((super.s_blocks_count - 1) % super.s_blocks_per_group != 0) {
		num_block_groups++;
	}
	printf("DEBUG: num_block_groups is %d\n", num_block_groups);
	return num_block_groups;
}


// returns 1 if directory entry has valid contents, else 0
int validate_dir_entry(int inode_num, int rec_len, int name_len, int file_type){
	if(inode_num > (int)super.s_inodes_count){return 0;}
	if(rec_len > (int)block_size){return 0;}
	if(name_len == 0 || name_len > EXT2_NAME_LEN){return 0;}
	if(file_type > 7){return 0;}

	return 1;
}

/*
 * Checks if the record len for a dir_entry matches its contents.
 * if yes, returns the rec_len
 * else, this entry is covering a deleted entry, return only the true rec_len
 * of this dir_entry
 */
int check_for_deleted_file(struct ext2_dir_entry_2 *entry){
	// re-derive rec_len
	int len = entry->name_len;
	int len_rem = len % 4;
	if(len_rem != 0){
		len += 4 - len_rem; //add padding to len
	}

	if(8 + len == entry->rec_len) {
		return entry->rec_len;
	}

	//else, this is covering a hidden directory entry
	if(DEBUG){printf("deleted dir_entry found!\n");}
	return 8 + len;
}


int main(int argc, char **argv) {
	if (argc != 3) {
		printf("expected usage: ./runscan inputfile outputfile\n");
		exit(0);
	}

	if(DEBUG){printf("input arg: %s\n", argv[1]);}
	if(DEBUG){printf("output path arg: %s\n", argv[2]);}

	char* path = argv[2];
	mkdir(path, 777);
	int curr_inode = 0;
	
	// open file, read super block, and initialize shared globals
	fd = open(argv[1], O_RDONLY);    /* open disk image */
	if(fd == -1){
		printf("error: failed to open file %s\n", argv[1]);
		exit(1);
	}
	ext2_read_init(fd);
	read_super_block(fd, 0, &super);

	// detect how many block groups
	int num_block_groups = calc_num_block_groups();
	struct ext2_group_desc groups[num_block_groups];

	// for each block group
	for(int i = 0; i < num_block_groups; i++){ // i is the ngroup
		read_group_desc(fd, i, &groups[i]);

		// locate the offset to this group's i-node table
		off_t curr_inode_block_offset = locate_inode_table(i, &groups[i]);
		
		for (unsigned int z = 0; z < (inodes_per_block * itable_blocks); z++){ //for each i-node in a given block
        	struct ext2_inode *inode = malloc(sizeof(struct ext2_inode));
        	read_inode(fd, i, curr_inode_block_offset, z, inode);
			
			if(S_ISREG(inode->i_mode) ? 1 : 0) { // if file (not directory)
				char magic_buff[4];
				load_magic_buffer(magic_buff, inode);
				
				if(check_if_jpg(magic_buff)){	
					if(inode->i_size == 0){ // don't copy out empty .jpgs
						free(inode);
						curr_inode++;
						continue;
					}

					char filename[EXT2_NAME_LEN + 1];  // filepath/file-XX.jpg
					snprintf(filename, sizeof(filename), "%s/file-%d.jpg", path, curr_inode);

					int output = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
					if(output == -1){
						perror("Error: ");
						printf("call to open failed for file %s\n", filename);
						exit(1);
					}

					if(WRITE_EACH_BLOCK) { // writes 1 block at time to disk, default setting
						write_iblocks_to_file(inode, output);
					}else{ // alternate implementation with fewer I/O interrupts
						char* writebuffer = malloc(inode->i_size);
						load_write_buffer(writebuffer, inode);
						write(output, writebuffer, inode->i_size);
						free(writebuffer);
					}

					if(fsync(output) == -1){
						perror("fsync error: ");
						printf("attempt to fsync file failed\n");
						exit(1);
					}
							
					if(close(output) == -1){
						perror("Close Error: ");
						printf("attempt to close file failed\n");
						exit(1);
					}
					
				}
			}// is file
			else if(S_ISDIR(inode->i_mode) ? 1 : 0){
				// will deleted files be in a directory? yes, but they will be hidden, need to detect
				if(inode->i_size == 0){// iterate over empty i-nodes
					curr_inode++;
					free(inode);
					continue;
				}

				struct ext2_dir_entry_2 *entry;
				unsigned int size;
				unsigned char first_block[block_size];

				// the list of file pointers in a directory will be stored in the 0th i_block
				// TODO: refactor code to support directories that are larger than a single block
				lseek(fd, BLOCK_OFFSET(inode->i_block[0]), SEEK_SET);
				read(fd, first_block, block_size);
				
				// for each entry in the directory
				size = 0;
				entry = (struct ext2_dir_entry_2 *) first_block;
				while(size < inode->i_size){
					if(!validate_dir_entry(entry->inode, entry->rec_len, entry->name_len, entry->file_type)){
						if(DEBUG){printf("warning: invalid directory entry found, skipping to next i-node.\n");}
						break;
					}
					
					char file_name[EXT2_NAME_LEN + 1];
					memcpy(file_name, entry->name, entry->name_len);
					file_name[entry->name_len] = '\0';
					if(DEBUG){printf("%10u %s\n", entry->inode, file_name);}
					
					// read the inode into a new inode struct
					struct ext2_inode *inode_in_dir = malloc(sizeof(struct ext2_inode));
					read_inode(fd, i, curr_inode_block_offset, entry->inode, inode_in_dir);

					// check if regular file and .jpg
					char magic_buff[4];
					load_magic_buffer(magic_buff, inode_in_dir);
					if(S_ISREG(inode_in_dir->i_mode) && check_if_jpg(magic_buff)){
						if(inode_in_dir->i_size == 0){ // don't copy out empty files
							free(inode_in_dir);
							int len_to_next_entry = check_for_deleted_file(entry);
							entry = (void*) entry + len_to_next_entry;
							size += len_to_next_entry;
							continue;
						}

						char full_filename[1024];  // filepath/file-XX.jpg
						snprintf(full_filename, sizeof(full_filename), "%s/%s", path, file_name);
						if(DEBUG){printf("Writing file %s to disk\n", full_filename);}

						int output = open(full_filename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
						if(output == -1){
							perror("Error: ");
							printf("call to open failed for file %s\n", full_filename);
							exit(1);
						}

						if(WRITE_EACH_BLOCK) { // writes 1 block at time to disk, default setting
							write_iblocks_to_file(inode_in_dir, output);
						}else{ // alternate implementation with fewer I/O interrupts
							char* writebuffer = malloc(inode_in_dir->i_size);
							load_write_buffer(writebuffer, inode_in_dir);
							write(output, writebuffer, inode_in_dir->i_size);
							free(writebuffer);
						}

						if(fsync(output) == -1){
							perror("fsync error: ");
							printf("attempt to fsync file failed\n");
							exit(1);
						}

						if(close(output) == -1){
							perror("Close Error: ");
							printf("attempt to close file failed\n");
							exit(1);
						}		
					}//if is_reg and is_jpg
					
					free(inode_in_dir);
					int len_to_next_entry = check_for_deleted_file(entry);
					entry = (void*) entry + len_to_next_entry;
					size += len_to_next_entry;
				}// for each directory entry
				
			}// else if(ISDIR)

			curr_inode++;
			free(inode);
		}// z-loop
	}// i-loop
	
	close(fd);
}



