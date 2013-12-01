#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  printf("Enter stuff\n");
  printf("Back from stuff\n");
  //while(1);
  char stuff[10];
  scanf("%s", stuff);
	printf("Hello %s!\n", stuff);
  exit(1);
}
