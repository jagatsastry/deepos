#ifndef UTILITIES_H
#define UTILITIES_H

void print_time(int millis);

struct smap_t {
  uint64_t base, length;
  uint32_t type;
}__attribute__((packed));

void print_status_bar(int millis);
//uint64_t oct_to_dec(char *oct);
char*  getAbsoluteFilePath(char *dir, char *fname) ;

char* dirnameWithEndSlash(char *dirname) ;


#endif
