#include <stdio.h>

const int CMD_MAX = 1024;
const int HISTORY_MAX = 16;

int
main(int argc, char *argv[])
{
	char history[HISTORY_MAX][CMD_MAX];

	for (int i = 0; i < HISTORY_MAX; i++) {
		sprintf(history[i], "command-%d", i);
	}

	for (int i = 0; i < HISTORY_MAX; i++) {
		printf("%s\n", history[i]);
	}
}
