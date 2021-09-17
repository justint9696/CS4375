#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
 *
 */
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0;
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }

  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void freeArgs(char **argv) {
  for (int i = 0 ; argv[i] != NULL; i++) {
    free(argv[i]);
  }
  free(argv);
}

char isValidCMD(char *cmd) {
  char *tokens[7] = { "ls", "wc", "cat", "echo", "pwd", "sleep", NULL };
  for (int i = 0 ; tokens[i] != NULL; i++) {
    if (!strcmp(tokens[i], cmd))
      return 1;
  }
  return 0;
}

void monitorCMD(char **tokens) {
  int rc = fork();
  if (rc < 0) {
    // fork process failed
    printf("fork failed.\n");
    exit(0);
  } else if (rc == 0) {
    // child process
    char *cmd = tokens[0];
    if (!strcmp("echo", cmd)) {
      int n;
      char **argv = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
      argv[0] = strdup("./echo");
      for (n = 1; tokens[n] != NULL; n++)
	argv[n] = tokens[n];
      argv[n] = NULL;
      execvp(argv[0], argv);
      freeArgs(argv);
    } else if (!strcmp("cat", cmd)) {
      char *argv[3];
      argv[0] = strdup("./cat");
      argv[1] = tokens[1];
      argv[2] = NULL;
      execvp(argv[0], argv);
    }
  } else {
    // parent process
    wait();
  }
}


int main(int argc, char* argv[]) {
  char  line[MAX_INPUT_SIZE];
  char  **tokens;
  int i;


  while(1) {
    /* BEGIN: TAKING INPUT */
    bzero(line, sizeof(line));
    printf("$ ");
    scanf("%[^\n]", line);
    getchar();

    // printf("Command entered: %s (remove this debug output later)\n", line);
    /* END: TAKING INPUT */

    line[strlen(line)] = '\n'; //terminate with new line
    tokens = tokenize(line);

    //do whatever you want with the commands, here we just print them

    // for(i=0;tokens[i]!=NULL;i++){
    // printf("found token %s (remove this debug output later)\n", tokens[i]);
    // }

    if (isValidCMD(tokens[0])) {
      monitorCMD(tokens);
    } else {
      for (int i = 0; tokens[i] != NULL; i++) {
	if (i == 0) printf("'");
	else printf(" ");
	printf("%s", tokens[i]);
      }
      printf("' is not a recognized command.\n", line);
    }

    // Freeing the allocated memory
    for(i=0;tokens[i]!=NULL;i++){
      free(tokens[i]);
    }
    free(tokens);

  }
  return 0;
}
