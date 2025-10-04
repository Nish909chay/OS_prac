#include <stdio.h>      // printf, perror
#include <stdlib.h>     // exit, malloc, atoi()
#include <fcntl.h>      // open() flags like O_RDONLY
#include <unistd.h>     // lseek(), read(), close()
#include <ext2fs/ext2_fs.h> // struct ext2_super_block, struct ext2_group_desc


int main(int argc, char *argv[])
{
/* this was for checking at default disk image and block 0
    int fd = open("disk.img", O_RDONLY);  
    
    if(fd < 0)
    {
        perror("error opening");
        exit(1);
    }
    
    // struct ext2_super_block super; - this gives the ext2 structure of the superblock and is defined in <ext2fs/ext2_fs/h>
    
    struct ext2_super_block super;
    lseek(fd, 1024, SEEK_SET);
    read(fd, &super, sizeof(super));
    
    // printing the block size 
    int block_size = 1024 << super.s_log_block_size;
    printf("Block Size = %d \n", block_size);
*/

    // take arguments which are image name and group number apparently 
    if(argc<3)
    {
        printf("Usage: %s <image> <group_number>\n", argv[0]);
        exit(1);
    }
    char *image = argv[1];
    int n = atoi(argv[2]);
    // atoi = ASCII to int 
    
    int fd = open(image, O_RDONLY);
    if(fd < 0)
    {
        perror("error opening");  
        exit(1);
    }
    
    // now we get the superblock - always at 1024
    struct ext2_super_block super;
    lseek(fd, 1024, SEEK_SET);
    read(fd, &super, sizeof(super));

    int block_size = 1024 << super.s_log_block_size;
    off_t gd_table_offset;
    
    if(block_size == 1024)
      gd_table_offset = 2*block_size; 
    else
      gd_table_offset = block_size;
      
    struct ext2_group_desc jd;
    lseek(fd, gd_table_offset + n * sizeof(jd), SEEK_SET);
    read(fd, &jd, sizeof(jd));
    
    printf("Block bitmap: %u\n", jd.bg_block_bitmap);
    printf("Inode bitmap: %u\n", jd.bg_inode_bitmap);
    printf("Inode table: %u\n", jd.bg_inode_table);
    printf("Free blocks: %u\n", jd.bg_free_blocks_count);
    printf("Free inodes: %u\n", jd.bg_free_inodes_count);

    
    close(fd);
    return 0;
    

}


