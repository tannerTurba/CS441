#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int rc, pipefd[2] = { -1, -1 };
	int exitcode = EXIT_FAILURE;
	pid_t pid;

	rc = pipe(pipefd);
	if (-1 == rc) {
		perror("Failed to create pipe");
		goto done;
	}

	pid = fork();
	switch (pid) {
	case 0:
		/* Child. */
		rc = dup2(pipefd[0], STDIN_FILENO);
		if (-1 == rc) {
			perror("Child failed to dup");
			goto done;
		}

		close(pipefd[0]);
		close(pipefd[1]);

		execlp("sort", "sort", NULL);
		perror("Failed to exec");
		goto done;
	case -1:
		perror("Failed to fork");
		goto done;
		break;
	default:
		/* Parent. */
		rc = dup2(pipefd[1], STDOUT_FILENO);
		if (-1 == rc) {
			perror("Parent failed to dup");
			goto done;
		}

		close(pipefd[0]);
		close(pipefd[1]);

		/* Stdout now piped to child. */
		printf("z\n");
		printf("y\n");
		printf("x\n");

		// execlp("echo", "ABC", NULL);

		break;
	}

	exitcode = EXIT_SUCCESS;

done:
	if (pipefd[0] != -1) {
		close(pipefd[0]);
	}

	if (pipefd[1] != -1) {
		close(pipefd[1]);
	}

	exit(exitcode);
}