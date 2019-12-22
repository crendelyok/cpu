#include "transl.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("Incorrect num of args");
		return -1;
	}
	GetBinaryFromFile(argv[1]);
	return 0;
}
