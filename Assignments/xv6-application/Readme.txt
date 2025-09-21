understanding headers:
    types.h -> unsignwd int - no negative values  
    stat.h -> basically for metadata - stat ek sys call hai
    user.h -> 
    fs.h -> definitions related to xv6 filessys structure/layout  

Q) i didnt know what INODE is:
$ it is file or directory metabdata - type, permissionss, size, owner etc
$ inode number is used to index inode - when OS walnt to access a file -> OS searches the directory -> the directory will have inode num for that file -> OS retrives metadata for that inode from inode table  
$ inode table hota hai disk pe 

xv6answers.tQ) stat ka structure :
struct stat {
    int dev;     // file system device
    uint ino;    // inode number
    short type;  // file type (T_FILE, T_DIR, etc.)
    short nlink; // number of links
    uint size;   // file size in bytes
};

---- sys call is an interface so that user can req servce from kernel

Q) user.h kyu hota hai
$ to tell the kernel ki jo function mene call kiya voh kernel ko execute karna hai
$ uske bina usse lagega ki ye koi user function hai 
$ jese open() ek sys call hai - ye kenel pe bhi defined hai but usse kese pata ki ye muje karna hai ki nahi
$ hotel me agar tum seeti bajake waiter ko buloage toh usse kese oata chalega ki usse bulare ho

Q) fs.h me kya hai
#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device
$ ye OS ko distinguish karne ke liye(shayad)

-- abhi program samjhte hai -- 
as said "before reading the code ye samjho ki code ka kaam kya hai
fir toh code me toh bas syntax hai" -italics


globals counters - files_found and files_deleted 
main(argc, argv)
{
    2 char ptr - *direcvtory (curr dir {points to '.'}) and  *search_pattern(set to null)

    find ko atleast 2 arg chahiye jese li ki "find me.txt"
    so agar argc usse kam hoga toh galat hai toh simply exit() sys call karo (exit() ko hamne user.h ke madad se define kara hai)

    abhi "find" command/text ko directorty se alag karo
    ---------- $ ./find /home/user/docs "*.txt" --------
    argv[0] =  "./find" (prog name)
    argv[1] =  "directory" -- /home/user/docs
    argv[2] =  "search pattern" -- "*.txt"

    -- there was a logical error - fixed

    now we check if argv[2] is a is options such as -r or -l and we use vars such as recursive and list_files to indicate what option is invoked

     we then call the searchFiles function that will seacrh the pattern 
     we pass:
            directory : passed by value
            search_pattern : passed by value
            recursive : passed by value
            list_files : passed by value

        all passed by value cuz no changes are intendded 

    to have more option i defined a struct:
    struct find_options    
    {
    char *name_pattern;     // -name 
    int print_mode;         // -print 
    int delete_mode;        // -delete
    int empty_only;         // -empty
    int recursive;          // -r
    int list_files;         // -l
    char *exec_cmd;         // -exec command
    };

    struct find_options opts = {0}; // init all members to 0

    after this i pass only directory and opts to searchFiles()

}

Q) LOGICAL ERROR {spotted during this review sesh} 
$ the condtion was :
if(argc >= 3) {
        search_pattern = argv[2];
}
$ here what is argv[2] is an option such as "-r" and we declare it as a search pattern 
ye toh valid nahi hai
so just added checks to see if it were options {-l or -r} [only 2 implented yet] 
could add -name, -atime, -user, -print, -empty, -delete
Usage: 
     1. Find files by name pattern 
        find /path/to/search -name "*.txt"

     2. Find files accessed in the last 7 days
        find /path/to/search -atime -7
            [this canty be done as xb6 stat doesnt have access time field ]

    3. Find files owned by a specific user
        find /path/to/search -user username
            [xv6 has no user support - cant be done]

    4. Print matching file paths (default action)
        find /path/to/search -name "*.txt" -print

    5. Execute a command on each matched file
        find /path/to/search -name "*.txt" -exec cat {} \;

    6. Find empty files or directories
        find /path/to/search -empty
            [no rmdir]

    7. Delete matching files
        find /path/to/search -name "*.bak" -delete

    8. Find empty `.txt` files and delete them
        find /path/to/search -name "*.txt" -empty -delete

    9. Find `.txt` files modified in the last 30 days and execute `cat` on them
        find /path/to/search -name "*.txt" -atime -30 -exec cat {} \;
