#ifndef _TARFS_H
#define _TARFS_H

#include <elf.h>

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
} __attribute__ ((packed));

void print_posix_header( struct posix_header_ustar * );

struct posix_header_ustar* get_elf_file(char*, Exe_Format*, uint64_t *elfStart);

int matchString( char * , char *);

int atoi(char *);

void map_exe_format();

uint64_t get_entry_address();

#endif
