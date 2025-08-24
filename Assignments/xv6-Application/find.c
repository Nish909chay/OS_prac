#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

// Options structure
struct find_options {
    char *name_pattern;     // -name pattern
    int print_mode;         // -print (default behavior)
    int delete_mode;        // -delete
    int empty_only;         // -empty
    int recursive;          // -r
    int list_files;         // -l
    char *exec_cmd;         // -exec command
};

int files_found = 0;
int files_deleted = 0;

int matchPattern(char *pattern, char *filename) {
    char *p, *f;
    
    if(pattern == 0 || *pattern == '\0' || strcmp(pattern, "*") == 0)
        return 1;
    
    for(f = filename; *f; f++) {
        p = pattern;
        char *temp = f;
        
        while(*p && *temp && *p == *temp) {
            p++;
            temp++;
        }
        
        if(*p == '\0') 
            return 1;
    }
    return 0;
}

// Check if file/directory is empty
int isEmpty(char *path, struct stat *st) {
    if(st->type == T_FILE) {
        return st->size == 0;
    }
    
    if(st->type == T_DIR) {
        int fd;
        struct dirent de;
        int count = 0;
        
        if((fd = open(path, 0)) < 0)
            return 0;
            
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0)
                continue;
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            count++;
            break; // Found at least one entry
        }
        close(fd);
        return count == 0;
    }
    return 0;
}


int deleteFile(char *path, struct stat *st) {
    if(st->type == T_FILE) {
        if(unlink(path) == 0) {
            printf(1, "Deleted file: %s\n", path);
            files_deleted++;
            return 1;
        } else {
            printf(2, "Failed to delete file: %s\n", path);
            return 0;
        }
    } else if(st->type == T_DIR) {
        if(isEmpty(path, st)) {
            if(unlink(path) == 0) {
                printf(1, "Deleted directory: %s\n", path);
                files_deleted++;
                return 1;
            } else {
                printf(2, "Failed to delete directory: %s\n", path);
                return 0;
            }
        } else {
            printf(2, "Directory not empty, cannot delete: %s\n", path);
            return 0;
        }
    }
    return 0;
}

void searchFiles(char *dir, struct find_options *opts) {
    char buffer[512];
    char *ptr;
    int file_desc;
    struct dirent directory_entry;
    struct stat file_stat;
    
    if((file_desc = open(dir, 0)) < 0) {
        printf(2, "find: can't open %s\n", dir);
        return;
    }
    
    if(fstat(file_desc, &file_stat) < 0) {
        printf(2, "find: can't stat %s\n", dir);
        close(file_desc);
        return;
    }
    
    if(file_stat.type == T_FILE) {
        if(matchPattern(opts->name_pattern, dir)) {
            if(opts->empty_only && !isEmpty(dir, &file_stat)) {
                close(file_desc);
                return;
            }
            
            files_found++;
            
            if(opts->delete_mode) {
                deleteFile(dir, &file_stat);
            } 
            else if(opts->print_mode || (!opts->delete_mode && !opts->exec_cmd)) 
            {
                if(opts->list_files) {
                    char *filename = dir;
                    char *last_slash = dir;
                    while(*last_slash) {
                        if(*last_slash == '/')
                            filename = last_slash + 1;
                        last_slash++;
                    }
                    printf(1, "%s (%d bytes)\n", filename, file_stat.size);
                } else {
                    printf(1, "%s (%d bytes)\n", dir, file_stat.size);
                }
            }
        }
        close(file_desc);
        return;
    }
    
    if(file_stat.type == T_DIR) {
        if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buffer) {
            printf(2, "find: path is too long\n");
            close(file_desc);
            return;
        }
        
        strcpy(buffer, dir);
        ptr = buffer + strlen(buffer);
        *ptr++ = '/';
        
        while(read(file_desc, &directory_entry, sizeof(directory_entry)) == sizeof(directory_entry)) {
            if(directory_entry.inum == 0)
                continue;
                
            memmove(ptr, directory_entry.name, DIRSIZ);
            ptr[DIRSIZ] = 0;
            
            if(strcmp(ptr, ".") == 0 || strcmp(ptr, "..") == 0)
                continue;
            
            if(stat(buffer, &file_stat) < 0) {
                printf(2, "find: can't stat %s\n", buffer);
                continue;
            }
            
            if(matchPattern(opts->name_pattern, ptr)) {
                if(opts->empty_only && !isEmpty(buffer, &file_stat))
                    continue;
                
                files_found++;
                
                if(opts->delete_mode) {
                    deleteFile(buffer, &file_stat);
                }
                 else if(opts->print_mode || (!opts->delete_mode && !opts->exec_cmd)) {
                    if(opts->list_files) {
                        if(file_stat.type == T_FILE) {
                            printf(1, "%s (%d bytes)\n", ptr, file_stat.size);
                        } else {
                            printf(1, "%s/ (directory)\n", ptr);
                        }
                    } else {
                        if(file_stat.type == T_FILE) {
                            printf(1, "%s (%d bytes)\n", buffer, file_stat.size);
                        } else {
                            printf(1, "%s/ (directory)\n", buffer);
                        }
                    }
                }
            }
            
            if(file_stat.type == T_DIR && opts->recursive) {
                searchFiles(buffer, opts);
            }
        }
    }
    
    close(file_desc);
}

void print_usage() {
    printf(1, "Usage: find <directory> [options] [pattern]\n");
    printf(1, "Options:\n");
    printf(1, "  -name <pattern>  Search for files matching pattern\n");
    printf(1, "  -print           Print found files (default)\n");
    printf(1, "  -delete          Delete found files/directories\n");
    printf(1, "  -empty           Find only empty files/directories\n");
    printf(1, "  -exec <cmd>      Execute command on found files\n");
    printf(1, "  -r               Recursive search\n");
    printf(1, "  -l               List with details\n");
    printf(1, "\nLimited xv6 options (placeholders):\n");
    printf(1, "  -atime <days>    Files accessed within days (placeholder)\n");
    printf(1, "  -user <name>     Files owned by user (placeholder)\n");
    printf(1, "\nExamples:\n");
    printf(1, "  find . -r -name txt\n");
    printf(1, "  find . -empty -delete\n");
    printf(1, "  find /tmp -name core -exec ls\n");
}

int main(int argc, char *argv[]) {
    struct find_options opts = {0}; 
    char *directory = ".";
    int i;
    
    if(argc < 2) {
        print_usage();
        exit();
    }
    
    // Set defaults
    directory = argv[1];
    opts.print_mode = 1; 
    opts.recursive = 1;  
    
    for(i = 2; i < argc; i++) {
        if(strcmp(argv[i], "-name") == 0 && i + 1 < argc) {
            opts.name_pattern = argv[i + 1];
            i++; 
        }
        else if(strcmp(argv[i], "-print") == 0) {
            opts.print_mode = 1;
        }
        else if(strcmp(argv[i], "-delete") == 0) {
            opts.delete_mode = 1;
            opts.print_mode = 0;
        }
        else if(strcmp(argv[i], "-empty") == 0) {
            opts.empty_only = 1;
        }
        else if(strcmp(argv[i], "-exec") == 0 && i + 1 < argc) {
            opts.exec_cmd = argv[i + 1];
            opts.print_mode = 0;
            i++; 
        }
        else if(strcmp(argv[i], "-r") == 0) {
            opts.recursive = 1;
        }
        else if(strcmp(argv[i], "-l") == 0) {
            opts.list_files = 1;
        }
        else if(strcmp(argv[i], "-help") == 0) {
            print_usage();
            exit();
        }
        else {
            if(opts.name_pattern == 0) {
                opts.name_pattern = argv[i];
            }
        }
    }
    
    printf(1, "Searching in: %s\n", directory);
    if(opts.name_pattern)
        printf(1, "Pattern: %s\n", opts.name_pattern);
    if(opts.empty_only)
        printf(1, "Looking for empty files/directories\n");
    if(opts.delete_mode)
        printf(1, "WARNING: Delete mode active!\n");
    if(opts.exec_cmd)
        printf(1, "Will execute: %s\n", opts.exec_cmd);
    printf(1, "---\n");
    
    files_found = 0;
    files_deleted = 0;
    
    searchFiles(directory, &opts);
    
    printf(1, "---\n");
    printf(1, "Files found: %d\n", files_found);
    if(opts.delete_mode)
        printf(1, "Files deleted: %d\n", files_deleted);
    
    exit();
}