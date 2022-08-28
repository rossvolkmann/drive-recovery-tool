# drive-recovery-tool installation
**What is this?**

The runscan program recovers all files, including hidden and deleted files, given a Linux ext2 drive image.  Even if the file system itself has been deleted,
so long as the disk has not been reformatted it should recover all files still referenced by an inode.  

This repository includes a Linux Makefile as well as some sample drive images in the disk_images folder.


**How do I install this?**

Clone down the repository to your machine.  You will need to have gcc installed in order to compile C code.  If you are on a Linux machine you can simply call
"make" inside the program directory and runscan should compile.  On a machine that does not recognize the make command (most MacOS or Windows machines) you can
manually compile runscan by calling:

$ gcc -Wall -Wextra -Werror -g runscan.c read_ext2.c -o runscan


**How do I use this?**

runscan takes two arguments, a drive image and an output directory

usage: $ runscan disk_images/image-01 output_dir

If the output directory does not exist, runscan will create it.  Ensure that runscan has the permissions required to create directories.  Windows machines may or may
not support calls to mkdir() depending on their specific version.  

# Usage

**Options**

There are two global variables you can alter by editing the runscan.c source code.

DEBUG - will enable or disable debug statements

WRITE_EACH_BLOCK - determines whether the drive is fully cached in memory and then written out to disk in a single operaton (0) or whether each block will be written individually. (1)

Depending on the size of the files vs. the number of files you are trying to recover, one may be more efficient than the other.  


**Expected Outputs**

For each file on the disk you should recover up to 2 copies of it.  One will be an anonymous file named after the inodef the file was located at.  The second will be a named file recovered from searching through the directory system.  Deleted files may or may not be recovered with their filenames intact depending on how they were deleted.  


**Limitations**

Currently this program is hardcoded to only return .jpg files.  This limitation should be easy to remove or alter if you want to recover all files or other specific types of files.  If you want to recover other types of files you can check the first data block stored in the inode and compare the magic numbers to your desired filetype signature.  


**Why make this?**

This program was written as a personal excercise and portfolio project.  My goals were to learn more about coding in C and better understand simple file structures like those found in Linux.  To learn more, check out the online textbook Operating Systems: Three Easy Pieces: https://pages.cs.wisc.edu/~remzi/OSTEP/
