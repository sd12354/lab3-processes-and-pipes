#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "cat scores | grep <argument> | sort".
 *
 * Three processes are chained together with two pipes:
 *
 *   P1 (parent)         executes "cat scores"
 *   P2 (child)          executes "grep <argument>"
 *   P3 (child's child)  executes "sort"
 *
 *   cat scores --pipe1--> grep <argument> --pipe2--> sort --> stdout
 *
 * As in pipes_processes2.c, this quick-and-dirty implementation has the
 * parent exec without waiting for its child, so the command prompt may
 * reappear before the last process in the chain terminates.
 *
 * Usage: ./pipes_processes3 <argument>      e.g. ./pipes_processes3 28
 */

int main(int argc, char **argv)
{
  int pipefd1[2];   // pipe 1: cat  -> grep
  int pipefd2[2];   // pipe 2: grep -> sort
  pid_t pid;

  // the one command line argument is the pattern handed to grep

  if (argc != 2)
    {
      fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
      return 1;
    }

  char *cat_args[]  = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};

  // make the first pipe (fds go in pipefd1[0] and pipefd1[1])

  if (pipe(pipefd1) == -1)
    {
      perror("pipe");
      return 1;
    }

  pid = fork();

  if (pid < 0)
    {
      perror("fork");
      return 1;
    }

  if (pid == 0)
    {
      // child (P2) gets here and handles "grep <argument>"

      // make the second pipe (fds go in pipefd2[0] and pipefd2[1])

      if (pipe(pipefd2) == -1)
        {
          perror("pipe");
          exit(1);
        }

      pid = fork();

      if (pid < 0)
        {
          perror("fork");
          exit(1);
        }

      if (pid == 0)
        {
          // child's child (P3) gets here and handles "sort"

          // replace standard input with input part of the second pipe

          dup2(pipefd2[0], 0);

          // close every pipe fd we inherited; sort only needs its stdin.
          // (leaving a write end open would keep sort from ever seeing EOF)

          close(pipefd2[0]);
          close(pipefd2[1]);
          close(pipefd1[0]);
          close(pipefd1[1]);

          // execute sort

          execvp("sort", sort_args);
          perror("execvp sort");
          exit(1);
        }
      else
        {
          // replace standard input with input part of the first pipe

          dup2(pipefd1[0], 0);

          // replace standard output with output part of the second pipe

          dup2(pipefd2[1], 1);

          // close the original pipe fds now that they are duplicated

          close(pipefd1[0]);
          close(pipefd1[1]);
          close(pipefd2[0]);
          close(pipefd2[1]);

          // execute grep

          execvp("grep", grep_args);
          perror("execvp grep");
          exit(1);
        }
    }
  else
    {
      // parent (P1) gets here and handles "cat scores"

      // replace standard output with output part of the first pipe

      dup2(pipefd1[1], 1);

      // close unused input half of the pipe (and the duplicated output half)

      close(pipefd1[0]);
      close(pipefd1[1]);

      // execute cat

      execvp("cat", cat_args);
      perror("execvp cat");
      exit(1);
    }
}
