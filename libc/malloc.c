#include <defs.h>
#include <stdlib.h>
#include <stdio.h>

char *heap_end;
char *cur_ptr;
void* malloc(size_t size) {
  if (cur_ptr == NULL || cur_ptr + size > heap_end) {
    cur_ptr = (char*)sbrk();
    heap_end = (char*)cur_ptr + 4095;
  }
  if (cur_ptr == NULL) {
    printf("Err: Out of memory\n");
    return NULL;
  }
  char *temp = cur_ptr;
  cur_ptr =  cur_ptr + size;
  return (void*)temp;
}
