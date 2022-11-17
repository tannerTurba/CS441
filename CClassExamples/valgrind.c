#include <stdio.h>
#include <stdlib.h>

//gcc -g -fsanitize=address 10-valgrind
// valgrind <executable>

int
main(int argc, char *argv[])
{
	char *p;
	int data[16];

	/*
	 * data is uninitialized!
	 * Compile with -g.
	 * Run with valgrind ... See the error?
	 */
	if (data[16]) {
		printf("luck\n");
	}

	/*
	 * Heap overflow!
	 * Compile with -g.
	 * Run with valgrind ... See the error?
	 */
	p = malloc(1);
	p[1] = 'x';
	
	/*
	 * Stack overflow!
	 * Compile with -fsanitize=address.
	 * Run. See the error?
	 */
	for (int i = 0; i <= 16; i++) {
		data[i] = i;
	}

	for (int i = 0; i < 16; i++) {
		printf("%d\n", data[i]);
	}
}