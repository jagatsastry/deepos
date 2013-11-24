#include <stdlib.h>
#include <defs.h>

void memset(void *init, char val, int sz) {
      int i = 0;
      for(; i < sz; i++)
          ((char*)init)[i] = 0;
}

//assume no overlap
void memcpy(void *dest, void *src, uint64_t num_bytes) { 
  while (num_bytes--) 
    ((char*)dest)[num_bytes] = ((char*)src)[num_bytes];
}
