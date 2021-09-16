#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
  for (int i = 1; argv[i] != NULL; i++)
    printf("%s ", argv[i]);
  printf("\n");
  exit(0);
}
