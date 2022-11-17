#include <stdio.h>
#include <stdlib.h>

char *
create_s(int i, int j)
{
	int size;
	char *s;

	/* A little trick for using snprintf with malloc: */
	size = snprintf(NULL, 0, "%d:%d", i, j);
	/* Now we have the number of bytes that WOULD have been written ... */

	s = malloc(size + 1);	
	if (s != NULL) {
		snprintf(s, size + 1, "%d:%d", i, j);
	}

	return s;
}

char *
borrow_s(int i, int j)
{
	/*
	 * Static values are on neither the stack or heap. They exist for the
	 * lifetime of the program and cannot be free'ed. Not thread safe!
	 */
	static char s[1024];
	snprintf(s, sizeof(s), "%d:%d", i, j);

	return s;
}

int *
never_never_never()
{
	/* Do NOT return a pointer to a stack variable! */
	int i = 42;
	return &i; /* Bad bad bad. */
}

int
main(int argc, char *argv[])
{
	/* Sometimes it is difficult to remember who should free. */
	char *mine;
	char *yours;

	/*
	 * Establish a naming convention. Here create_ means caller owns
	 * object and is responsible for free'ing.
	 */
	mine = create_s(1, 2);
	if (NULL == mine) {
		/* Malloc failed. Do something. */
	}
	printf("%s\n", mine);

	/* No create_, thus assume callee owns object. */
	yours = borrow_s(1, 2);
	printf("%s\n", yours);

	free(mine);
	/* Do not free yours, it is owned by borrow_s. */
}