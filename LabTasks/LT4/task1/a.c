#include<stdio.h>
int add(int a, int b)
{
    return a + b;
}

// sudo apt install qemu-system-x86

// ./prog & - will get you the Process PID as well
// echo $! - if you forgot to see the pid 

/*
sleep 1000 &
echo $!
cat /proc/$pid/cmdline
cat /proc/$pid/status | grep ctxt
ls -l /proc/$pid/fd
cat /proc/$pid/io
cat /proc/$pid/environ
*/