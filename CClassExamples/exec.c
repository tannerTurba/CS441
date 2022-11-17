#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	char *file;

	if (argc != 2) {
		fprintf(stderr, "usage: %s FILE\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	file = argv[1];

	pid = fork();
	switch (pid) {
	case 0: /* Child. */
		char *childargv[] = { "cat", file, NULL };
		execvp("/usr/bin/cat", childargv);
		perror("Running program failed"); /* Should never get here. */
		exit(EXIT_FAILURE);
	case -1: /* Error. */
		perror("Fork failed");
		exit(EXIT_SUCCESS);
	default: /* Parent. */
		wait(&status);
		if (WIFEXITED(status) && EXIT_SUCCESS == WEXITSTATUS(status)) {
			/* Child terminated by exit and without error. */
			printf("Running cat worked.\n");
			exit(EXIT_SUCCESS);
		} else {
			fprintf(stderr, "Running cat failed.\n");
			exit(WEXITSTATUS(status));
		}
	}
}