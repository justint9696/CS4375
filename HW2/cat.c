#include <stdio.h>
#include <string.h>

#define MAX_INPUT_SIZE 1024

int main(int argc, char *argv[]) {
  // opens file and displays content to user.

  FILE *file;
  char text[MAX_INPUT_SIZE];
  char *filename = argv[1];
  file = fopen(filename, "r");
  if (file == NULL) {
    printf("File not found: '%s'.\n", filename);
  } else {
    while (fgets(text, MAX_INPUT_SIZE, file) != NULL) {
      printf("%s", text);
    }
  }
  exit(0);
}
