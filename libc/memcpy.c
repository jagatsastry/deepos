#include <defs.h>
//assume no overlap
void memcpy(void *dest, void *src, uint64_t num_bytes) {
    while (num_bytes--)
          ((char*)dest)[num_bytes] = ((char*)src)[num_bytes];
}

