#ifndef READ_EXT2
#define READ_EXT2
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/stat.h>
#include "ext2_fs.h"

#define BASE_OFFSET 1024                   /* locates beginning of the super block (first group) */
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*block_size)

extern unsigned int block_size;				/* default 1kB block size */
extern unsigned int inodes_per_block;		/* number of inodes per block */
extern unsigned int itable_blocks;			/* size in blocks of the inode table */
extern unsigned int blocks_per_group;		/* number of blocks per block group */
extern unsigned int num_groups;				/* number of block groups in the image */
extern unsigned int inodes_per_group;

extern int debug;		//turn on/off debug prints

/* read the first super block to initialize common variables */
void ext2_read_init(int fd);

/* read the specified super block */
int read_super_block(	int fd,        						/* the disk image file descriptor */
						int ngroup,        					/* which block group to access */
					  	struct ext2_super_block *super      /* where to put the super block */
					);

/* Read the group-descriptor in the specified block group */
void read_group_desc(   int fd,        						/* the disk image file descriptor */
					    int ngroup,        					/* which block group to access */
					  	struct ext2_group_desc *group     	/* where to put the group-descriptor */
					);

/* calculate the start address of the inode table in the specified group */
off_t locate_inode_table(	int ngroup,        						  /* which block group to access */
					  		const struct ext2_group_desc *group);     /* the first group-descriptor */

/* calculate the start address of the data blocks in the specified group */
off_t locate_data_blocks(	int ngroup,        						   /* which block group to access */
					  		const struct ext2_group_desc *group);      /* the first group-descriptor */

/* read an inode with specified inode number and group number */
void read_inode( int fd,        								/* the disk image file descriptor */
				 int ngroup,        							/* which block group to access */
				 off_t offset,    								/* offset to the start of the inode table */
				 int inode_no,  								/* the inode number to read  */
				 struct ext2_inode *inode   					/* where to put the inode */
				); 

#endif

