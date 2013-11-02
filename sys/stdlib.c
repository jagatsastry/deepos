#include <stdlib.h>

void memset(void *init, char val, int sz) {
      int i = 0;
      for(; i < sz; i++)
          ((char*)init)[i] = 0;
}
