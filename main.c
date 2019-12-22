#include "cpu.h"
#include "stack.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("Incorrect num of args");
		return -1;
	}
	assert(Execution(argv[1]) == 0);
	return 0;
}
