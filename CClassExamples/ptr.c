#include <stdio.h>

/* A pointer holds the address of another object. */

/*
 * It is a good idea to draw pictures when using pointers.
 * For example: ptr: --> i: 0
 */
void foo(int *ptr) /* An int * is a pointer to an int. */
{
	*ptr = 42; /* Think of '*' here as "value at". */
}

int
main(int argc, char *argv[])
{
	int i = 0;
	foo(&i);   /* Think of '&' as "address of". */
	printf("%d\n", i);
}
