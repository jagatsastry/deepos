#ifndef _SYSCALL_H
#define _SYSCALL_H

#define STR2(x) #x
#define STR1(x) STR2(x)
#define STR(x) STR1(x)


#define     PRINT  0,
#define     EXITSYSCALL 1
#define     SSCANFSYSTEMCALL 2
#define     ENTERTOMEMORY 3
#define     SYS_FORK 4
#define     SYS_EXECVPE 5
#define     SYS_SLEEP 6
#define     SYS_WAITPID 7
#define     SYS_SBRK 8
#define     SYS_GETPID 9
#define     SYS_OPEN 10
#define     SYS_READ 11
#define     SYS_CLOSE 12
#define     SYS_OPENDIR 13
#define     SYS_READDIR 14
#define     SYS_CLOSEDIR 15
#define     NUM_SYSCALLS 16

#endif
