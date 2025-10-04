#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ext2fs/ext2_fs.h>

// fopen() would return a FILE POINTER
// open() would return a int file descriptor
int open_disk(const char *path)
{
    int fd = open(path, O_RDONLY);
    if(fd < 0)
    {
        perror("open \n");
        exit(1);
    }
    return fd;
}

void read_bytes(int fd, off_t offset, void *buf, size_t size)
{
    if(lseek(fd, offset, SEEK_SET) < 0)
    {
        perror("lseek \n");
        exit(1);
    }
    if(read(fd, buf, size) != size)
    {
        perror("read");
        exit(1);
    }
}

void read_superblock(int fd, struct ext2_super_block *sb)
{
  read_bytes(fd, 1024, sb, sizeof(struct ext2_super_block));
  
  if(sb->s_magic != EXT2_SUPER_MAGIC)
  {
        fprintf(stderr, "Error: Not a valid ext2 filesystem (magic = 0x%x)\n", sb->s_magic);
        exit(1);
  }
}

void print_superblock(struct ext2_super_block *sb) {
    int block_size = 1024 << sb->s_log_block_size;

    printf("Superblock Info:\n");
    printf("Inodes Count: %u\n", sb->s_inodes_count);
    printf("Blocks Count: %u\n", sb->s_blocks_count);
    printf("Block Size: %d\n", block_size);
    printf("Inodes per Group: %u\n", sb->s_inodes_per_group);
    printf("Inode Size: %u\n", sb->s_inode_size);
    printf("First Non-reserved Inode: %u\n", sb->s_first_ino);
}

void read_group_desc(int fd, struct ext2_super_block *sb, struct ext2_group_desc *gd) {
    int block_size = 1024 << sb->s_log_block_size;
    off_t gd_offset;
    if (block_size == 1024) {
        gd_offset = 2 * block_size;   
    } else {
        gd_offset = block_size;       
    }

    read_bytes(fd, gd_offset, gd, sizeof(struct ext2_group_desc));
}

void print_group_desc(struct ext2_group_desc *gd) {
    printf("\nGroup Descriptor:\n");
    printf("Block Bitmap: %u \n", gd->bg_block_bitmap);
    printf("Inode Bitmap: %u \n", gd->bg_inode_bitmap);
    printf("Inode Table Block: %u \n", gd->bg_inode_table);
}


void read_inode(int fd, struct ext2_super_block *sb,
                struct ext2_group_desc *gd,
                uint32_t inode_no,
                struct ext2_inode *inode) {

    int block_size = 1024 << sb->s_log_block_size;

    // which group and index inside the group
    uint32_t group = (inode_no - 1) / sb->s_inodes_per_group;
    uint32_t index = (inode_no - 1) % sb->s_inodes_per_group;

    if (group != 0) {
        fprintf(stderr, "This simple version only handles group 0 (inode in first group)\n");
        exit(1);
    }

    // compute inode offset
    off_t inode_table_offset = (off_t)gd->bg_inode_table * block_size;
    off_t inode_offset = inode_table_offset + (index * sb->s_inode_size);

    read_bytes(fd, inode_offset, inode, sizeof(struct ext2_inode));
}

void print_inode(struct ext2_inode *inode) {
    printf("\nInode Daya:\n");
    printf("Mode: %u \n", inode->i_mode);
    printf("Size: %u \n", inode->i_size);
    printf("Blocks: %u \n", inode->i_blocks);
    printf("Links count: %u \n", inode->i_links_count);

    printf("Block pointers:\n");
    for (int i = 0; i < EXT2_N_BLOCKS; i++) {
        if (inode->i_block[i])
            printf("  i_block[%d] = %u\n", i, inode->i_block[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <disk-image> <inode-number>\n", argv[0]);
        exit(1);
    }

    const char *disk_path = argv[1];
    uint32_t inode_no = atoi(argv[2]);

    int fd = open_disk(disk_path);

    struct ext2_super_block sb;
    read_superblock(fd, &sb);
    print_superblock(&sb);

    struct ext2_group_desc gd;
    read_group_desc(fd, &sb, &gd);
    print_group_desc(&gd);

    struct ext2_inode inode;
    read_inode(fd, &sb, &gd, inode_no, &inode);
    print_inode(&inode);

    close(fd);
    return 0;
}

