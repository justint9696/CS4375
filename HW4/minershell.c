#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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

char ***splitCommands(char **tokens) {
  // Checks each token to see if user has called a pipeline, in which case we split them into seperate commands.
  int commandIndex = 0, n = 0;
  char ***commands = (char ***)malloc(MAX_NUM_TOKENS * sizeof(char **));
  commands[commandIndex] = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  for (int i = 0; tokens[i] != NULL; i++) {
    if (!strcmp("|", tokens[i])) {
      n = 0;
      commandIndex++;
      commands[commandIndex] = (char **)malloc(MAX_NUM_TOKENS * sizeof(char));
    } else {
      commands[commandIndex][n] = tokens[i];
      n++;
    }
  }
  return commands;
}

char isValidCMD(char *cmd) {
  // Checks to see if user entered command is valid.
  char *tokens[10] = { "ls", "wc", "cat", "echo", "pwd", "sleep", "cd", "cd..", "exit", NULL };
  for (int i = 0 ; tokens[i] != NULL; i++) {
    if (!strcmp(tokens[i], cmd))
      return 1;
  }
  return 0;
}
 
void executeCommand(char **tokens) {
  int i;
  char *command = tokens[0];
  if (!strcmp(command, "cd")) {
    chdir(tokens[1]);
  } else if (!strcmp(command, "cd..")) {
    chdir("..");
  } else if (!strcmp(command, "exit")) {
    exit(1);
  } else {
    char **argv = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    if (!strcmp(command, "ls")) {
      argv[0] = strdup("/bin/ls"); 
    } else if (!strcmp(command, "sort")) {
      argv[0] = strdup("sort");
    } else if (!strcmp(command, "echo")) {
      argv[0] = strdup("/bin/echo");
    } else if (!strcmp(command, "cat")) {
      argv[0] = strdup("/bin/cat");
    } else if (!strcmp(command, "pwd")) {
      argv[0] = strdup("/bin/pwd");
    } else if (!strcmp(command, "wc")) {
      argv[0] = strdup("wc");
    } else if (!strcmp(command, "sleep")) {
      argv[0] = strdup("/bin/sleep");
    } else {
      char error_message[30] = "An error has occurred\n";
      write(STDERR_FILENO, error_message, strlen(error_message));
      exit(1);
    }
    char *file;
    int redirect_fd;
    for (i = 1; tokens[i] != NULL; i++) {
      if (!strcmp(tokens[i], ">")) {
	printf("redirect found\n");
	// redirect output and error.
	file = tokens[i+1];
	creat(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // flags give read/write permissions
	redirect_fd = open(file, O_WRONLY | O_TRUNC);
	dup2(redirect_fd, STDOUT_FILENO); // redirect output
	dup2(redirect_fd, STDERR_FILENO); // redirect error
	close(redirect_fd);
	break;
      }
      argv[i] = tokens[i];
    }
    argv[i] = NULL;
    execvp(argv[0], argv);
  }
}

int countCommands(char ***commands) {
  int count = 0;
  for (int i = 0; commands[i] != NULL; i++)
    count += 1;
  return count;
}

void executeCommands(char ***commands, int inputfd) {
  // only supports one pipe.
  if (commands[1] == NULL) {
    // this is the only command in array, no need to pipe.
    if (!strcmp(commands[0][0], "exit") || !strcmp(commands[0][0], "cd") || !strcmp(commands[0][0], "cd..")) {
      // these are the commands which do not require a fork.
      executeCommand(commands[0]);
    } else {
      int id = fork();
      if (id < 0) {
	// fork failed.
	exit(1);
      } else if (id == 0) {
	executeCommand(commands[0]);
      } else {
	// parent process.
	wait(NULL);
      }
    }
  } else {
    // there is more than one command in array.
    int fd[2];
    if (pipe(fd) == -1) {
      // pipe failed.
      exit(1);
    }
    int id1 = fork();
    if (id1 < 0) {
      // fork failed.
      exit(1);
    } else if (id1 == 0) {
      // child process.
      dup2(fd[1], STDOUT_FILENO); // redirect output of first command.
      close(fd[0]);
      close(fd[1]);
      executeCommand(commands[0]);
    }
    
    int id2 = fork();
    if (id2 < 0) {
      // fork failed.
      exit(1);
    } else if (id2 == 0) {
      // child process.
      dup2(fd[0], STDIN_FILENO);
      close(fd[0]);
      close(fd[1]);
      executeCommand(commands[1]);
    }

    close(fd[0]);
    close(fd[1]);

    wait(NULL);
  }
}

int main(int argc, char* argv[]) {
  char  line[MAX_INPUT_SIZE];
  char  **tokens;
  char ***commands;
  int i;
  int fd[2];
  
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

   
    commands = splitCommands(tokens);

    /*
    // print each command after splitting them by '|'
    for (int i = 0; commands[i] != NULL; i++) {
      printf("Command %d: ", i+1);
      for (int j = 0; commands[i][j] != NULL; j++) {
	printf("%s ", commands[i][j]);
      }
      printf("\n");
    }
    */
   
    executeCommands(commands, -1);
	
    // Freeing the allocated memory
    for(i=0;tokens[i]!=NULL;i++){
      free(tokens[i]);
    }
    free(tokens);

    // Could not figure out how to free memory; resulted in a segmentation fault.
    /*
    for (int i = 0; commands[i] != NULL; i++) {
      for (int j = 0; commands[i][j] != NULL; j++) {
	free(commands[i][j]);
      }
      free(commands[i]);
    }
    free(commands);
    */

  }
  return 0;
}
