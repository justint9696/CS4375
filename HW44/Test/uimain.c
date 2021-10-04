#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void executeCommands(char ***commands) {
  // Executes all commands split by '|' identifier.
  if (commands[1] == NULL) {
    // 
  }
}

int main() {
  int fd[2];
  if (pipe(fd) == -1) {
    printf("pipe failed\n");
    return 1;
  }
  
  int id1 = fork();
  if (id1 < 0) {
    printf("fork failed\n");
    return 2;
  } else if (id1 == 0) {
    // child process 1
    dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    close(fd[1]);
    char *argv[2];
    argv[0] = strdup("/bin/ls");
    argv[1] = NULL;
    execvp(argv[0], argv);
  }

  int id2 = fork();
  if (id2 < 0) {
    printf("fork failed\n");
    return 3;
  } else if (id2 == 0) {
    // child process 2
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    close(fd[1]);
    char *argv[3];
    argv[0] = strdup("sort");
    argv[1] = "-R";
    argv[2] = NULL;
    execvp(argv[0], argv);
  }

  close(fd[0]);
  close(fd[1]);

  wait(NULL);

  return 0;
}
