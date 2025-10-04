Lab Task 9

1) Write a program to print n'th group descriptor from an ext2 file system.  filename: nthgd.c 
2) Write another program to print n'th data block from an ext2 file system.   Filename: nthblock.c

Steps
    • Get the tools
      sudo apt update
      sudo apt install build-essential e2fsprogs e2tools
    • ext2 superblock is always at offset = 1024 bytes – you will have to lseek()
    • superblock will occupy always 1KB space = 1024 bytes
    • superblock fields we have to know 
      s_log_block_size → gives block size as 1024 << s_log_block_size.
      s_blocks_per_group → how many blocks per group.
      s_inodes_per_group → how many inodes per group.
      s_first_data_block → index of first block.
    • Group Descriptors fields we have to know 
      bg_block_bitmap (block number of block bitmap)
      bg_inode_bitmap (block number of inode bitmap)
      bg_inode_table (start block of inode table)
      bg_free_blocks_count (how many free blocks in this group)
      bg_free_inodes_count (how many free inodes in this group)
    • The Group Descriptor Table (GDT) is a block of metadata that describes all the block groups in the ext2 file system.
    • A single group descriptor contains info about one block group:
    • Block bitmap block
    • Inode bitmap block
    • Start of inode table
    • Free blocks count
    • Free inodes count
    • Its size is fixed (usually 32 bytes in ext2).
    • ❯ ./nthgd disk.img 0
    • Block bitmap: 5
    • Inode bitmap: 6
    • Inode table: 7
    • Free blocks: 15347
    • Free inodes: 16373
