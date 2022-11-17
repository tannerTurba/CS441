#include <stdio.h>

/*
 * It is a good idea to draw pictures when using pointers.
 * For example: ptrptr: --> s --> "Hello, world!"
 */
void foo(char **ptrptr)
{
	/*
	 * The double pointer allows us to change the target of s,
	 * outside of this function. If ptrptr is a pointer to a
	 * pointer, then *ptrptr is a pointer.
	 */
	*ptrptr = "Goodbye, world!";
}

int
main(int argc, char *argv[])
{
	char *s = "Hello, world!";
	foo(&s); /* s is a pointer, so this passes a pointer to a pointer. */
	printf("%s\n", s);
}
