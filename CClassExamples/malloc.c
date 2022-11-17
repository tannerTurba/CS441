#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	/* This object is stored on the stack, which C manages. */
	int x[3] = { 0, 1, 2 };

	/* The object pointed to by J exists on the heap, which you must manage. */
	int *y = malloc(3 * sizeof(int));
	if (NULL == y) {
		/* Allocation failed! */
	}
	/*
	 * Note that this is a silly use of malloc! Use an array for small- to
	 * medium-sized objects with a size known a compile time.
	 */

	/* The function malloc does not initialize. */
	y[0] = 10;
	y[1] = 11;
	y[2] = 12;

	/*
	 * I can play games to "remember" the size of an array.
	 * sizeof(i) is sizeof(int) * 3, so we can calculate
	 * the number of elements in i like this:
	 */
	for (int i = 0; i < sizeof(x) / sizeof(x[0]); i++) {
		printf("%d\n", x[i]);
	}

	/*
	 * The compiler cannot help me with the size of a malloc'ed range,
	 * because that size is a run-time construct.  For this I need
	 * to remember the number of items, or I need to NULL terminate
	 * my range. Here I remember the length:
	 */
	for (int i = 0; i < 3; i++) {
		printf("%d\n", y[i]);
	}

	/*
	 * Note: it would be better to #define YSIZE as 3 and replace all uses
	 * of 3 with YSIZE. You should abhor "magic numbers" by now.
	 */

	/* If we malloc, we must free. */
	free(y);
}