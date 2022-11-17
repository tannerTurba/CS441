#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int openreadprint(char *filename)
{
	char buf[1024];
	/* Initialize variables to pessimistic values when relevant. */
	int fd = -1, count, retval = -1;

	/* Handle PROGRAMMER errors with assertions. */
	assert(NULL != filename);

	fd = open(filename, O_RDONLY);
	if (-1 == fd) {
		/* Global variable errno holds actual error code. */
		goto done;
	}

	for (;;) { //while true(infinte loop)
		count = read(fd, buf, sizeof(buf));
		switch (count) {
		case 0:
			/* EOF */
			retval = 0; /* Done. Now things get optimistic. */
			goto done;
		case -1:
			/* Error. */
			goto done;
		default:
			/* Read count bytes; print them. */
			write(STDOUT_FILENO, buf, count);
			continue;
		}
	}
	
	/* Handle all teardown (e.g., close) in one place if possible. */
done:
	/* Due to pessimistic initialization, fd holds -1 unless file opened. */
	if (fd != -1) {
		close(fd);
	}

	return retval;
}

int
main(int argc, char *argv[])
{
	/* Pessimistic initialization of exitcode. */
	int rc, exitcode = EXIT_FAILURE;

	rc = openreadprint("/etc/passwd");
	if (-1 == rc) {
		perror("Error");
		goto done;
	}

	/* Everything went fine, let's change our gloomy outlook! */
	exitcode = EXIT_SUCCESS;

done:
	exit(exitcode);
}