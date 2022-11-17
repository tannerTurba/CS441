#include <stdio.h>

int
main(int argc, char *argv[])
{
	/* 
	 * For demonstration of pointer arithmetic only!
	 * *i as an expression would generate a segmentation fault.
	 */
	int *i = (int *) 1;

	printf("n + 1 is not always n + 1! i:     %p\n", i);
	printf("n + 1 is not always n + 1! i + 1: %p\n", i + 1);
	printf("n + 1 is not always n + 1! &i[1]: %p\n", &i[1]);
}
