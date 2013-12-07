#include <stdio.h>
void func(int d, int c) {
  printf("%d\n", d);
  printf("%d\n", c);
}
int main() {
  __asm__ __volatile__ (
    "movq $10, %rdi;\n"
    "movq $20, %rsi;\n"
    "movq $20, %rsi;\n"
    "callq func;\n"
    );
  return 0;
}
