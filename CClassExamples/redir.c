#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int rc, fd = -1, exitcode = EXIT_FAILURE;
	pid_t pid;

	fd = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (-1 == fd) {
		perror("Failed to open file");
		goto done;
	}

	rc = dup2(fd, STDOUT_FILENO);
	if (-1 == rc) {
		perror("Failed to duplicate file descriptor");
		goto done;
	}

	close(fd);

	/*
	 * File descriptors inherited by exec'ed process.
	 * Stdout now goes to file, not terminal.
	 */
	execlp("echo", "echo", "Hello, redirected world!", NULL); 
	perror("Failed to exec echo");

done:
	exit(exitcode);
}