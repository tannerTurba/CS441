#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

char *x = "foo" "bar";
//char *y = "foo" y;   WRONG

int
main(int argc, char *argv[])
{
	char a = 'x';
	int b = 42;
	uint64_t c = 0xdeadbeef;
	double d = 3.14;
	char *e = "Hello, world";

	/* See the printf(3) man page for MANY format-string options. */
	printf("%c %d %x %" PRIx64 " %f %s\n", a, b, b, c, d, e);

	/* We can print the address stored in a pointer variable with "%p". */
	printf("%p\n", e);

	/* These compiler-provided macros are useful for quick debugging. */
	printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
}