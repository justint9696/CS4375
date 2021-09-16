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

char isValidCMD(char *cmd) {
  char *tokens[3] = { "echo", "exit", NULL };
  for (int i = 0 ; tokens[i] != NULL; i++) {
    if (!strcmp(tokens[i], cmd))
      return 1;
  }
  return 0;
}

void monitorCMD(char **tokens) {
  char *cmd = tokens[0];
  int rc = fork();
  if (rc < 0) {
    printf("fork failed.\n");
    exit(0);
  } else if (rc == 0) {
    if (!strcmp("echo", cmd)) {
      int n;
      char **argv = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
      argv[0] = strdup("./echo");
      for (n = 1; tokens[n] != NULL; n++)
	argv[n] = tokens[n];
      tokens[n] = NULL;
      execvp(argv[0], argv);
      for (int i = 0; i < n; i++)
	free(argv[i]);
      free(argv);
    } else if (!strcmp("exit", cmd)) {
      exit(0);
    } else {
      printf("unk\n");
      for (int i = 0; tokens[i] != NULL; i++)
	printf("'%s' not a recognized command.\n");
    }
  } else {
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

    printf("Command entered: %s (remove this debug output later)\n", line);
    /* END: TAKING INPUT */

    line[strlen(line)] = '\n'; //terminate with new line
    tokens = tokenize(line);

    //do whatever you want with the commands, here we just print them

    for(i=0;tokens[i]!=NULL;i++){
      printf("found token %s (remove this debug output later)\n", tokens[i]);
    }

    if (isValidCMD(tokens[0]))
      monitorCMD(tokens);

    // Freeing the allocated memory
    for(i=0;tokens[i]!=NULL;i++){
      free(tokens[i]);
    }
    free(tokens);

  }
  return 0;
}
