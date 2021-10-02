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

void monitorCMD(char **tokens) {
  // Executes user inputted command; forks if needed.
  char *cmd = tokens[0];
  if (!strcmp("cd", cmd)) {
    chdir(tokens[1]); // changes directory to user input.
  } else if (!strcmp("cd..", cmd)) {
    chdir(".."); // goes up a directory.
  } else if (!strcmp("exit", cmd)) {
    exit(1); // terminate parent process.
  } else {
    int rc = fork();
    if (rc < 0) {
      // fork process failed.
      printf("fork failed.\n");
      exit(1);
    } else if (rc == 0) {
      // child process.
      if (!strcmp("echo", cmd)) {
	int n;
	char **argv = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	argv[0] = strdup("/bin/echo");
	for (n = 1; tokens[n] != NULL; n++)
	  argv[n] = tokens[n]; // tokens to echo back to user.
	argv[n] = NULL;
	execvp(argv[0], argv);
	for (int i = 0; argv[i] != NULL; i++)
	  free(argv[i]);
	free(argv);
      } else if (!strcmp("cat", cmd)) {
	char *argv[3];
	argv[0] = strdup("/bin/cat");
	argv[1] = tokens[1]; // file to display.
	argv[2] = NULL;
	execvp(argv[0], argv);
      } else if (!strcmp("pwd", cmd)) {
	char *argv[2];
	argv[0] = strdup("/bin/pwd");
	argv[1] = NULL;
	execvp(argv[0], argv);
      } else if (!strcmp("ls", cmd)) {
	char *argv[2];
	argv[0] = strdup("/bin/ls");
	argv[1] = NULL;
	execvp(argv[0], argv);
      } else if (!strcmp("wc", cmd)) {
	int n;
	char **argv = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	argv[0] = strdup("wc");
	for (n = 1; tokens[n] != NULL; n++)
	  argv[n] = strdup(tokens[n]); // files to word count.
	argv[n] = NULL;
	execvp(argv[0], argv);
	for (int i = 0; argv[i] != NULL; i++)
	  free(argv[i]);
	free(argv);
      } else if (!strcmp("sleep", cmd)) {
	char *argv[3];
	argv[0] = strdup("/bin/sleep");
	argv[1] = tokens[1]; // sleep time seconds.
	argv[2] = NULL;
	execvp(argv[0], argv);
      } 
    } else {
      // parent process.
      wait(NULL); // wait until child process finishes.
    }
  }
}


int main(int argc, char* argv[]) {
  char  line[MAX_INPUT_SIZE];
  char  **tokens;
  char ***commands;
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

    for (int i = 0; commands[i] != NULL; i++) {
      if (isValidCMD(commands[i][0])) {
	char *file;
	char update = 0;
	int redirect_fd;
	for (int j = 0; commands[i][j] != NULL; j++) {
	  if (!strcmp(commands[i][j], ">")) {
	    // redirect std error and std output to file
	    update = 1;
	    file = commands[i][j + 1]; // file should be the next token after '>'
	  }
	  if (update) {
	    // if update is true, we know we have the file and remove any flags inputted by user
	    // flags for this command are unsupported
	    commands[i][j] = NULL;
	  }
	}

	if (update) {
	  // if update is true, we have found a redirection flag
	  creat(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // flags give read/write permissions
	  dup2(redirect_fd, STDOUT_FILENO); // redirect output
	  dup2(redirect_fd, STDERR_FILENO); // redirect error
	  close(redirect_fd);
	}

	monitorCMD(commands[i]);
      } else {
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
      }
    }
	
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
