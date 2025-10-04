#include <stdio.h>      // printf, perror
#include <stdlib.h>     // exit, malloc, atoi()
#include <fcntl.h>      // open() flags like O_RDONLY
#include <unistd.h>     // lseek(), read(), close()
#include <ext2fs/ext2_fs.h> // struct ext2_super_block, struct ext2_group_desc


int main(int argc, char *argv[])
{
    if(argc<3)
    {
        printf("Usage: %s <image> <group_number>\n", argv[0]);
        exit(1);
    }
    char *image = argv[1];
    int n = atoi(argv[2]);  // block num
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
    
    char *buff = malloc(block_size);
    if(buff == NULL)
    {
        perror("couldnt allocate memory");
        exit(1);
    }
    
    off_t block_offset = n * block_size;
    lseek(fd, block_offset, SEEK_SET);
    read(fd, buff, block_size);
    
    for(int i=0; i<block_size; i++)
    {
        printf("%04x ", (unsigned char)buff[i]); if((i+1) % 16 == 0) 
        printf("\n");
    }
    
    free(buff);
    close(fd);
    return 0;
    

}


