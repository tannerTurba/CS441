#include <stdio.h>
#include <stdlib.h>

int
compare(const void *_i, const void *_j)
{
	const int *i = _i;
	const int *j = _j;

	if (*i < *j) {
		return -1;
	} else if (*i == *j) {
		return 0;
	} else {
		return 1;
	}
}

int
main(int argc, char *argv[])
{
	int data[] = { 10, 8, 3, 6, 4, 1, 2, 5, 7, 9 };

	printf("unsorted:\n");
	for (int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		printf("%d\n", data[i]);
	}

	/* Here we pass a pointer to the compare function to qsort. */
	qsort(data, sizeof(data) / sizeof(data[0]), sizeof(data[0]), compare);

	printf("sorted:\n");
	for (int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		printf("%d\n", data[i]);
	}
}